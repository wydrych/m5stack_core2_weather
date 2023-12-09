#pragma once

#include <M5Unified.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "settings.hpp"

extern bool wifi_status;

struct forecast_entry_t
{
    time_t start;
    time_t interval;
    std::vector<float> points;
};

struct forecast_t
{
    forecast_entry_t airtmp_point;
    forecast_entry_t wchill_point;
};

bool forecast_convert(JsonObject &data, const char *key, forecast_entry_t &target)
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
        if (!value.is<float>())
        {
            M5_LOGE("forecast does not contain valid data points for %s", key);
            return false;
        }
        target.points.push_back(value.as<float>());
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
    DynamicJsonDocument doc(1024);
    String available_url = String(settings::forecast::base_url) + settings::forecast::available;
    if (!https_json_request(doc, settings::forecast::root_ca, "GET", available_url.c_str(), nullptr, 0))
        return 0;

    JsonArray available = doc[settings::forecast::model];
    if (available.isNull())
    {
        M5_LOGE("%s does not contain model %s", available_url.c_str(), settings::forecast::model);
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
    DynamicJsonDocument doc(256);
    doc["date"] = timestamp;
    JsonObject point = doc.createNestedObject("point");
    point["lat"] = std::to_string(settings::forecast::lat);
    point["lon"] = std::to_string(settings::forecast::lon);
    return doc.as<String>();
}

bool forecast_fetch(time_t timestamp, forecast_t &target)
{
    String payload = forecast_fetch_prepare_payload(timestamp);
    M5_LOGD("request payload: %s", payload.c_str());

    DynamicJsonDocument doc(64 * 1024);
    String model_url = String(settings::forecast::base_url) + settings::forecast::model;

    if (!https_json_request(doc, settings::forecast::root_ca, "GET", model_url.c_str(), (uint8_t *)payload.c_str(), payload.length()))
        return false;

    JsonObject data = doc["data"];
    if (data.isNull())
    {
        M5_LOGE("%s does not contain data", model_url.c_str());
        return false;
    }

    return forecast_convert(data, "airtmp_point", target.airtmp_point) &&
           forecast_convert(data, "wchill_point", target.wchill_point);
}

void forecast_loop()
{
    if (!wifi_status)
        return;
    static time_t last_fetched;
    static unsigned long last_refresh;
    static unsigned long last_retry;
    unsigned long now = millis();
    if (last_refresh != 0 && now - last_refresh < settings::forecast::refresh * 1000)
        return;
    if (last_retry != 0 && now - last_retry < settings::forecast::retry * 1000)
        return;

    last_retry = now;
    M5_LOGI("checking for newer %s forecast", settings::forecast::model);

    time_t latest_available = forecast_latest_available();
    if (!latest_available)
        return;
    M5_LOGD("lastest available: %d", latest_available);

    if (last_fetched >= latest_available)
    {
        last_refresh = now;
        return;
    }

    M5_LOGI("newer forecast available: %d", latest_available);

    forecast_t forecast;
    if (!forecast_fetch(latest_available, forecast))
    {
        M5_LOGE("failed to fetch and convert forecast");
        return;
    }

    M5_LOGI("new forecast fetched successfully", latest_available);
    M5_LOGD("new forecast fetched successfully: airtmp_point: %d+%dx%d, [0]: %f", forecast.airtmp_point.start, forecast.airtmp_point.points.size(), forecast.airtmp_point.interval, forecast.airtmp_point.points[0]);
    M5_LOGD("new forecast fetched successfully: wchill_point: %d+%dx%d, [0]: %f", forecast.wchill_point.start, forecast.wchill_point.points.size(), forecast.wchill_point.interval, forecast.wchill_point.points[0]);
    last_fetched = latest_available;
    last_refresh = now;
}