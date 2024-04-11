#include <M5Unified.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "forecast.hpp"
#include "settings.hpp"

extern bool wifi_status;

forecast_t forecast;

template <typename T>
bool forecast_convert(JsonObject &data, const char *key, forecast_entry_t<T> &target)
{
    JsonObject entry = data[key];
    if (entry.isNull())
    {
        M5_LOGE("forecast does not contain data for %s", key);
        return false;
    }

    M5_LOGD("converting %s", key);

    target.start = entry["first_timestamp"];
    target.interval = entry["interval"];
    JsonArray entry_data = entry["data"];

    if (target.start == 0 || target.interval == 0 ||
        entry_data.isNull() || entry_data.size() == 0)
    {
        M5_LOGE("forecast does not contain valid data for %s", key);
        return false;
    }

    target.points.reserve(entry_data.size());
    for (JsonVariant value : entry_data)
    {
        if (!value.is<JsonFloat>())
        {
            M5_LOGE("forecast does not contain valid data points for %s", key);
            return false;
        }
        target.points.push_back(value.as<T>());
    }
    return true;
}

bool https_json_request(JsonDocument &doc, const char *root_ca, const char *method, const char *url, uint8_t *payload, size_t payload_size)
{
    WiFiClientSecure wifi_secure_client;
    HTTPClient https_client;

    wifi_secure_client.setCACert(root_ca);

    if (!https_client.begin(wifi_secure_client, url))
    {
        M5_LOGE("unable to create HTTPS client to %s", url);
        return false;
    }
    int response_code = https_client.sendRequest(method, payload, payload_size);

    if (response_code <= 0)
    {
        M5_LOGE("HTTPS request %s %s failed: %s", method, url, https_client.errorToString(response_code).c_str());
        https_client.end();
        return false;
    }

    if (response_code != HTTP_CODE_OK)
    {
        M5_LOGE("HTTPS request %s %s unexpexted return code: %d", method, url, response_code);
        https_client.end();
        return false;
    }

    String response_body = https_client.getString();
    https_client.end();

    DeserializationError error = deserializeJson(doc, response_body);
    if (error)
    {
        M5_LOGE("%s %s: JSON deserialization error: %s", method, url, error.c_str());
        return false;
    }

    return true;
}

time_t forecast_latest_available()
{
    JsonDocument doc;
    String available_url = String(settings.forecast.base_url) + settings.forecast.available;
    if (!https_json_request(doc, settings.forecast.root_ca, "GET", available_url.c_str(), nullptr, 0))
        return 0;

    JsonArray available = doc[settings.forecast.model];
    if (available.isNull())
    {
        M5_LOGE("%s does not contain model %s", available_url.c_str(), settings.forecast.model);
        return 0;
    }

    time_t max = 0;
    for (JsonVariant value : available)
    {
        if (value.as<time_t>() > max)
            max = value.as<time_t>();
    }
    return max;
}

String forecast_fetch_prepare_payload(time_t timestamp)
{
    JsonDocument doc;
    doc["date"] = timestamp;
    JsonObject point = doc.createNestedObject("point");
    point["lat"] = std::to_string(settings.forecast.lat);
    point["lon"] = std::to_string(settings.forecast.lon);
    return doc.as<String>();
}

bool forecast_fetch(time_t timestamp, forecast_t &target)
{
    String payload = forecast_fetch_prepare_payload(timestamp);
    M5_LOGD("request payload: %s", payload.c_str());

    JsonDocument doc;
    String model_url = String(settings.forecast.base_url) + settings.forecast.model;

    if (!https_json_request(doc, settings.forecast.root_ca, "GET", model_url.c_str(), (uint8_t *)payload.c_str(), payload.length()))
        return false;

    JsonObject data = doc["data"];
    if (data.isNull())
    {
        M5_LOGE("%s does not contain data", model_url.c_str());
        return false;
    }

    return forecast_convert(data, "airtmp_point", target.airtmp_point) &&
           forecast_convert(data, "airtmp_min", target.airtmp_min) &&
           forecast_convert(data, "airtmp_max", target.airtmp_max) &&
           forecast_convert(data, "wchill_point", target.wchill_point) &&
           forecast_convert(data, "grdtmp_min", target.grdtmp_min) &&
           forecast_convert(data, "grdtmp_max", target.grdtmp_max) &&
           forecast_convert(data, "pcpttl_aver", target.pcpttl_aver) &&
           forecast_convert(data, "pcpttl_max", target.pcpttl_max) &&
           forecast_convert(data, "pcpttl_type_max", target.pcpttl_type_max) &&
           forecast_convert(data, "trpres_point", target.trpres_point);
}

void forecast_loop()
{
    if (!wifi_status)
        return;
    static unsigned long last_refresh;
    static unsigned long last_retry;
    unsigned long now = millis();
    if (last_refresh != 0 && now - last_refresh < settings.forecast.refresh * 1000)
        return;
    if (last_retry != 0 && now - last_retry < settings.forecast.retry * 1000)
        return;

    last_retry = now;
    M5_LOGI("checking for newer %s forecast", settings.forecast.model);

    time_t latest_available = forecast_latest_available();
    if (!latest_available)
        return;
    M5_LOGD("lastest available: %d", latest_available);

    if (forecast.timestamp >= latest_available)
    {
        last_refresh = now;
        return;
    }

    M5_LOGI("newer forecast available: %d", latest_available);

    forecast_t new_forecast;
    new_forecast.timestamp = latest_available;
    if (!forecast_fetch(latest_available, new_forecast))
    {
        M5_LOGE("failed to fetch and convert forecast");
        return;
    }

    M5_LOGI("new forecast fetched successfully", latest_available);
    M5_LOGD("new forecast: airtmp_point: %d+%dx%d, [0]: %f", new_forecast.airtmp_point.start, new_forecast.airtmp_point.points.size(), new_forecast.airtmp_point.interval, new_forecast.airtmp_point.points[0]);
    M5_LOGD("new forecast: airtmp_min: %d+%dx%d, [0]: %f", new_forecast.airtmp_min.start, new_forecast.airtmp_min.points.size(), new_forecast.airtmp_min.interval, new_forecast.airtmp_min.points[0]);
    M5_LOGD("new forecast: airtmp_max: %d+%dx%d, [0]: %f", new_forecast.airtmp_max.start, new_forecast.airtmp_max.points.size(), new_forecast.airtmp_max.interval, new_forecast.airtmp_max.points[0]);
    M5_LOGD("new forecast: wchill_point: %d+%dx%d, [0]: %f", new_forecast.wchill_point.start, new_forecast.wchill_point.points.size(), new_forecast.wchill_point.interval, new_forecast.wchill_point.points[0]);
    M5_LOGD("new forecast: grdtmp_min: %d+%dx%d, [0]: %f", new_forecast.grdtmp_min.start, new_forecast.grdtmp_min.points.size(), new_forecast.grdtmp_min.interval, new_forecast.grdtmp_min.points[0]);
    M5_LOGD("new forecast: grdtmp_max: %d+%dx%d, [0]: %f", new_forecast.grdtmp_max.start, new_forecast.grdtmp_max.points.size(), new_forecast.grdtmp_max.interval, new_forecast.grdtmp_max.points[0]);
    M5_LOGD("new forecast: pcpttl_aver: %d+%dx%d, [0]: %f", new_forecast.pcpttl_aver.start, new_forecast.pcpttl_aver.points.size(), new_forecast.pcpttl_aver.interval, new_forecast.pcpttl_aver.points[0]);
    M5_LOGD("new forecast: pcpttl_max: %d+%dx%d, [0]: %f", new_forecast.pcpttl_max.start, new_forecast.pcpttl_max.points.size(), new_forecast.pcpttl_max.interval, new_forecast.pcpttl_max.points[0]);
    M5_LOGD("new forecast: pcpttl_type_max: %d+%dx%d, [0]: %d", new_forecast.pcpttl_type_max.start, new_forecast.pcpttl_type_max.points.size(), new_forecast.pcpttl_type_max.interval, new_forecast.pcpttl_type_max.points[0]);
    M5_LOGD("new forecast: trpres_point: %d+%dx%d, [0]: %f", new_forecast.trpres_point.start, new_forecast.trpres_point.points.size(), new_forecast.trpres_point.interval, new_forecast.trpres_point.points[0]);
    forecast = new_forecast;
    last_refresh = now;
}