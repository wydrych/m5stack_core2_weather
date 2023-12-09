#include <esp_task_wdt.h>
#include <M5Unified.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

#include "icons/icons.hpp"
#include "forecast.hpp"
#include "lang.hpp"
#include "settings.hpp"
#include "panels/header.hpp"
#include "panels/main.hpp"
#include "panels/forecast.hpp"

bool wifi_status;
bool mqtt_status;

M5Canvas *displayCanvas;

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

HeaderPanel *headerPanel;
MainPanel *mainPanel;
ForecastPanel *temperatureForecastPanel,
    *rainForecastPanel,
    *pressureForecastPanel;
Panel *currentPanel,
    *lastDrawnPlanel;

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
  esp_task_wdt_init(settings::watchdog_timer, true);
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

  headerPanel = new HeaderPanel(displayCanvas, displayWidth, settings::header_height, M5.Display.getColorDepth());
  mainPanel = new MainPanel(displayCanvas, displayWidth, displayHeight - settings::header_height, M5.Display.getColorDepth());
  temperatureForecastPanel = new ForecastPanel(displayCanvas, displayWidth, displayHeight - settings::header_height, M5.Display.getColorDepth());
  rainForecastPanel = new ForecastPanel(displayCanvas, displayWidth, displayHeight - settings::header_height, M5.Display.getColorDepth());
  pressureForecastPanel = new ForecastPanel(displayCanvas, displayWidth, displayHeight - settings::header_height, M5.Display.getColorDepth());

  mainPanel->setForecastPanels(temperatureForecastPanel, rainForecastPanel, pressureForecastPanel);

  currentPanel = mainPanel;

  WiFi.begin(settings::wifi::ssid, settings::wifi::password);

  mqtt_client.setServer(settings::mqtt::server, settings::mqtt::port);
  mqtt_client.setCallback(mqtt_callback);

  configTime(0, 0, settings::time::ntpServer);
  setenv("TZ", settings::time::tz, 1);
  tzset();
}

void redraw()
{
  if (currentPanel == nullptr)
    currentPanel = mainPanel;
  bool headerRedrawn = headerPanel->redraw();
  bool currentPanelRedrawn = currentPanel->redraw();
  if (!headerRedrawn && !currentPanelRedrawn && lastDrawnPlanel == currentPanel)
    return;

  displayCanvas->clear();
  headerPanel->canvas->pushSprite(0, 0);
  currentPanel->canvas->pushSprite(0, settings::header_height);
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
  mqtt_client.subscribe(settings::mqtt::topic);
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
  const int json_doc_capacity = JSON_OBJECT_SIZE(7); // WSDCGQ11LM
  StaticJsonDocument<json_doc_capacity> doc;

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
    if (last_reconnect == 0 || now - last_reconnect >= settings::mqtt::reconnect * 1000)
    {
      last_reconnect = now;
      M5_LOGI("Trying to connect MQTT");
      mqtt_client.connect((String("M5Stack Core2 ") + WiFi.macAddress()).c_str());
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
  if (M5.Touch.getCount() != 1)
    return;

  auto t = M5.Touch.getDetail();
  if (!t.wasClicked())
    return;

  M5_LOGI("touch (%d,%d)", t.x, t.y);
  if (t.y >= settings::header_height)
    currentPanel = currentPanel->touch(t.x, t.y - settings::header_height);
}

void on_forecast_successfull_fetch(time_t timestamp) {
  headerPanel->forecastFetched(timestamp);
}

void loop()
{
  esp_task_wdt_reset();
  touch_loop();
  wifi_loop();
  mqtt_loop();
  forecast_loop();
  redraw();
}
