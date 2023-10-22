#include <M5Unified.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

#include "fonts/all.h"
#include "icons/icons.hpp"
#include "lang.hpp"
#include "settings.hpp"

bool wifi_status;
bool mqtt_status;
int displayWidth, displayHeight;
M5Canvas *displayCanvas;

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

time_t timestamp;
float temperature = NAN;
float humidity = NAN;
float pressure = NAN;

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
  M5.Log.setEnableColor(m5::log_target_serial, false);
  M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_DEBUG);

  auto cfg = M5.config();
  cfg.serial_baudrate = 115200;
  M5.begin(cfg);
  M5.Display.setBrightness(63);
  displayWidth = M5.Display.width();
  displayHeight = M5.Display.height();
  displayCanvas = new M5Canvas(&M5.Display);
  displayCanvas->setColorDepth(M5.Display.getColorDepth());
  displayCanvas->createSprite(displayWidth, displayHeight);
  displayCanvas->setBaseColor(settings::colors::background);
  displayCanvas->setTextColor(settings::colors::text, settings::colors::background);

  WiFi.begin(settings::wifi::ssid, settings::wifi::password);

  mqtt_client.setServer(settings::mqtt::server, settings::mqtt::port);
  mqtt_client.setCallback(mqtt_callback);

  configTime(0, 0, settings::time::ntpServer);
  setenv("TZ", settings::time::tz, 1);
  tzset();
}

bool should_redraw()
{
  static bool last_wifi_status;
  static time_t last_timetamp;
  time_t now;
  time(&now);

  return changed(&last_wifi_status, wifi_status) ||
         changed(&last_timetamp, now);
}

void redraw()
{
  displayCanvas->clear();

  draw_icons();
  draw_time();

  M5.Display.waitDisplay();
  displayCanvas->pushSprite(&M5.Display, 0, 0);
}

void draw_icons()
{
  displayCanvas->pushGrayscaleImage(
      2, 2, icons::wifi::width, icons::wifi::height,
      icons::wifi::data,
      m5gfx::grayscale_8bit,
      wifi_status ? settings::colors::icons::wifi : settings::colors::icons::down,
      settings::colors::background);

  displayCanvas->pushGrayscaleImage(
      24, 2, icons::mqtt::width, icons::mqtt::height,
      icons::mqtt::data,
      m5gfx::grayscale_8bit,
      mqtt_status ? settings::colors::icons::mqtt : settings::colors::icons::down,
      settings::colors::background);
}

void draw_time()
{
  time_t now;
  struct tm timeinfo;
  const int buflen = 64;
  char buf[buflen];

  time(&now);
  localtime_r(&now, &timeinfo);
  if (timeinfo.tm_year < 120)
  {
    return;
  }
  const char *weekday = weekdays_short[timeinfo.tm_wday][settings::lang];
  strcpy(buf, weekday);
  strftime(buf + strlen(weekday),
           buflen - strlen(weekday),
           time_format[settings::lang],
           &timeinfo);
  displayCanvas->setTextDatum(m5gfx::textdatum::baseline_right);
  displayCanvas->drawString(buf, displayWidth - 2, 17, settings::fonts::currentTime);
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
  const int json_doc_capacity = JSON_OBJECT_SIZE(7); // WSDCGQ11LM
  StaticJsonDocument<json_doc_capacity> doc;

  DeserializationError error = deserializeJson(doc, payload, length);
  if (error)
  {
    M5_LOGE("Json deserialization error: %s", error.c_str());
    return;
  }
  temperature = doc["temperature"] | NAN;
  humidity = doc["humidity"] | NAN;
  pressure = doc["pressure"] | NAN;
  M5_LOGD("Received new reading: %.1f %.0f %.0f", temperature, humidity, pressure);
}

void mqtt_loop()
{

  if (wifi_status && !mqtt_status)
  {
    static time_t last_timetamp;
    time_t now;
    time(&now);
    if (difftime(now, last_timetamp) >= settings::mqtt::reconnect)
    {
      last_timetamp = now;
      M5_LOGI("Trying to connect MQTT");
      mqtt_client.connect((String("M5Stack Core2 ") + WiFi.macAddress()).c_str());
    }
  }
  if (!wifi_status && mqtt_status)
    mqtt_client.disconnect();

  if (changed(&mqtt_status, mqtt_client.connected()))
    if (mqtt_status)
      on_mqtt_connected();
    else
      on_mqtt_disconnected();

  mqtt_client.loop();
}

void loop()
{
  wifi_loop();
  mqtt_loop();
  if (should_redraw())
    redraw();
}
