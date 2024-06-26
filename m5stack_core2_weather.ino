#include <esp_task_wdt.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <lgfx/utility/miniz.h>

#include "src/icons/icons.hpp"
#include "src/forecast.hpp"
#include "src/lang.hpp"
#include "src/settings.hpp"
#include "src/panels/header.hpp"
#include "src/panels/main.hpp"
#include "src/panels/forecast.hpp"
#include "src/panels/temperature.hpp"
#include "src/panels/pressure.hpp"
#include "src/panels/precipitation.hpp"

bool wifi_status;
bool mqtt_status;

M5Canvas *displayCanvas;

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

HeaderPanel *headerPanel;
MainPanel *mainPanel;
ForecastPanel *temperatureForecastPanel,
    *precipitationForecastPanel,
    *pressureForecastPanel;
Panel *currentPanel,
    *lastDrawnPlanel;
unsigned long lastPanelChange;

template <typename T>
inline bool changed(T *storage, T val)
{
    if (*storage == val)
        return false;
    *storage = val;
    return true;
}

void setup()
{
    esp_task_wdt_init(settings.watchdog_timer, true);
    esp_task_wdt_add(NULL);

    M5.Log.setEnableColor(m5::log_target_serial, false);
    M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_DEBUG);

    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);
    M5.Display.setBrightness(63);

    auto displayWidth = M5.Display.width();
    auto displayHeight = M5.Display.height();

    displayCanvas = new M5Canvas(&M5.Display);

    displayCanvas->setColorDepth(M5.Display.getColorDepth());
    displayCanvas->createSprite(displayWidth, displayHeight);

    headerPanel = new HeaderPanel(displayCanvas, displayWidth, settings.header_height, M5.Display.getColorDepth());
    mainPanel = new MainPanel(displayCanvas, displayWidth, displayHeight - settings.header_height, M5.Display.getColorDepth());
    temperatureForecastPanel = new TemperatureForecastPanel(displayCanvas, displayWidth, displayHeight - settings.header_height, M5.Display.getColorDepth());
    precipitationForecastPanel = new PrecipitationForecastPanel(displayCanvas, displayWidth, displayHeight - settings.header_height, M5.Display.getColorDepth());
    pressureForecastPanel = new PressureForecastPanel(displayCanvas, displayWidth, displayHeight - settings.header_height, M5.Display.getColorDepth());

    mainPanel->setForecastPanels(temperatureForecastPanel, precipitationForecastPanel, pressureForecastPanel);

    currentPanel = mainPanel;

    WiFi.begin(settings.wifi.ssid, settings.wifi.password);

    mqtt_client.setServer(settings.mqtt.server, settings.mqtt.port);
    mqtt_client.setCallback(mqtt_callback);

    configTzTime(settings.time.tz, settings.time.ntpServer);
}

void redraw()
{
    if (currentPanel == nullptr)
        currentPanel = mainPanel;
    bool headerRedrawn = headerPanel->redraw();
    bool currentPanelRedrawn = currentPanel->redraw();
    if (!headerRedrawn && !currentPanelRedrawn && lastDrawnPlanel == currentPanel)
        return;

    if (lastDrawnPlanel != currentPanel)
        lastPanelChange = millis();

    displayCanvas->clear();
    headerPanel->canvas->pushSprite(0, 0);
    currentPanel->canvas->pushSprite(0, settings.header_height);
    lastDrawnPlanel = currentPanel;

    M5.Display.waitDisplay();
    displayCanvas->pushSprite(0, 0);
}

void on_wifi_connected()
{
    M5_LOGI("WiFi connected");
}

void on_wifi_disconnected()
{
    M5_LOGI("WiFi disconnected");
}

void wifi_loop()
{
    if (!changed(&wifi_status, WiFi.isConnected()))
        return;

    headerPanel->setWifiStatus(wifi_status);

    if (wifi_status)
        on_wifi_connected();
    else
        on_wifi_disconnected();
}

void on_mqtt_connected()
{
    M5_LOGI("MQTT connected");
    mqtt_client.subscribe(settings.mqtt.topic);
}

void on_mqtt_disconnected()
{
    M5_LOGI("MQTT disconnected");
}

void mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    float reading_temperature;
    float reading_humidity;
    float reading_pressure;
    JsonDocument doc;

    DeserializationError error = deserializeJson(doc, payload, length);
    if (error)
    {
        M5_LOGE("Json deserialization error: %s", error.c_str());
        return;
    }
    reading_temperature = doc["temperature"] | NAN;
    reading_humidity = doc["humidity"] | NAN;
    reading_pressure = doc["pressure"] | NAN;
    M5_LOGD("Received new reading: %f %f %f",
            reading_temperature, reading_humidity, reading_pressure);
    mainPanel->setReading(reading_temperature, reading_humidity, reading_pressure);
}

void mqtt_loop()
{
    if (wifi_status && !mqtt_status)
    {
        static unsigned long last_reconnect;
        unsigned long now = millis();
        if (last_reconnect == 0 || now - last_reconnect >= settings.mqtt.reconnect * 1000)
        {
            last_reconnect = now;
            M5_LOGI("Trying to connect MQTT");
            mqtt_client.connect(settings.mqtt.client_name,
                                settings.mqtt.user, settings.mqtt.password);
        }
    }
    if (!wifi_status && mqtt_status)
        mqtt_client.disconnect();

    if (changed(&mqtt_status, mqtt_client.connected()))
    {
        headerPanel->setMqttStatus(mqtt_status);

        if (mqtt_status)
            on_mqtt_connected();
        else
            on_mqtt_disconnected();
    }
    mqtt_client.loop();
}

void touch_loop()
{
    M5.update();

    if (M5.BtnPWR.wasClicked())
    {
        M5_LOGI("PNG screenshot requested");
        size_t pngSize;
        uint8_t *screenshot = (uint8_t *)M5.Display.createPng(&pngSize, 0, 0, M5.Display.width(), M5.Display.height());
        M5_LOGI("PNG screenshot buffer size: %zu, sending hexdump to serial", pngSize);
        // use a following command to save to PNG file:
        //   python3 -c 'import sys; open("screenshot.png", "wb").write(bytes.fromhex(sys.stdin.read()))'
        for (size_t i = 0; i < pngSize; i++)
        {
            Serial.printf("%02x", screenshot[i]);
        }
        Serial.println();
        mz_free(screenshot);
    }

    if (M5.Touch.getCount() != 1)
        return;

    auto t = M5.Touch.getDetail();
    if (!t.wasClicked())
        return;

    M5_LOGI("touch (%d,%d)", t.x, t.y);
    if (t.y >= settings.header_height)
        currentPanel = currentPanel->touch(t.x, t.y - settings.header_height);
}

void panel_timeout_loop()
{
    if (currentPanel != mainPanel &&
        millis() - lastPanelChange > settings.panel_timeout * 1000)
        currentPanel = mainPanel;
}

void status_loop()
{
    static int64_t next = 0;
    if (esp_timer_get_time() < next)
        return;
    next += settings.mqtt.status_interval * 1000000;

    multi_heap_info_t info;
    JsonDocument doc;

    doc["uptime"] = esp_timer_get_time() / 1000000;

    heap_caps_get_info(&info, MALLOC_CAP_8BIT);
    doc["heap_8bit_total_free_bytes"] = info.total_free_bytes;
    doc["heap_8bit_total_allocated_bytes"] = info.total_allocated_bytes;
    doc["heap_8bit_largest_free_block"] = info.largest_free_block;
    doc["heap_8bit_minimum_free_bytes"] = info.minimum_free_bytes;
    doc["heap_8bit_allocated_blocks"] = info.allocated_blocks;
    doc["heap_8bit_free_blocks"] = info.free_blocks;
    doc["heap_8bit_total_blocks"] = info.total_blocks;

    heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
    doc["heap_spi_total_free_bytes"] = info.total_free_bytes;
    doc["heap_spi_total_allocated_bytes"] = info.total_allocated_bytes;
    doc["heap_spi_largest_free_block"] = info.largest_free_block;
    doc["heap_spi_minimum_free_bytes"] = info.minimum_free_bytes;
    doc["heap_spi_allocated_blocks"] = info.allocated_blocks;
    doc["heap_spi_free_blocks"] = info.free_blocks;
    doc["heap_spi_total_blocks"] = info.total_blocks;

    char json[1024];
    size_t json_length = serializeJson(doc, json);

    if (!mqtt_client.beginPublish(settings.mqtt.status_topic_name, json_length, false))
        return;
    mqtt_client.write((uint8_t *)json, json_length);
    mqtt_client.endPublish();
}

void loop()
{
    esp_task_wdt_reset();
    wifi_loop();
    mqtt_loop();
    forecast_loop();
    panel_timeout_loop();
    touch_loop();
    redraw();
    status_loop();
}
