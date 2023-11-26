#include <M5Unified.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <time.h>

#include "icons/icons.hpp"
#include "lang.hpp"
#include "settings.hpp"

bool wifi_status;
bool mqtt_status;
int displayWidth, displayHeight;
M5Canvas *displayCanvas;

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

time_t reading_timestamp = 0;
float reading_temperature = NAN;
float reading_humidity = NAN;
float reading_pressure = NAN;

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
  static bool last_drawn_wifi_status;
  static bool last_drawn_mqtt_status;
  static time_t last_drawn_time;
  static time_t last_drawn_reading_timestamp;
  time_t now;
  time(&now);

  if (reading_timestamp != 0 && difftime(now, reading_timestamp) > 3600)
    reading_timestamp = 0;

  return changed(&last_drawn_wifi_status, wifi_status) |
         changed(&last_drawn_mqtt_status, mqtt_status) |
         changed(&last_drawn_time, now) |
         changed(&last_drawn_reading_timestamp, reading_timestamp);
}

void redraw()
{
  displayCanvas->clear();

  draw_icons();
  draw_time();
  draw_reading();

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

void draw_reading()
{
  const int buflen = 16;
  char buf_temp[buflen], buf_hum[buflen], buf_press[buflen];

  if (reading_timestamp)
  {
    sprintf(buf_temp, "%.1f°C", reading_temperature);
    sprintf(buf_hum, "%.0f%%", reading_humidity);
    sprintf(buf_press, "%.0f hPa", reading_pressure);
  }
  else
  {
    displayCanvas->setTextSize(3, 1);
    strcpy(buf_temp, "-");
    strcpy(buf_hum, "");
    strcpy(buf_press, "");
  }

  displayCanvas->setTextDatum(m5gfx::textdatum::baseline_center);
  displayCanvas->drawString(buf_temp,
                            displayWidth / 2, 138,
                            settings::fonts::temperature);

  displayCanvas->setTextDatum(m5gfx::textdatum::baseline_left);
  displayCanvas->drawString(buf_hum,
                            14, displayHeight - 14,
                            settings::fonts::humidity);

  displayCanvas->setTextDatum(m5gfx::textdatum::baseline_right);
  displayCanvas->drawString(buf_press,
                            displayWidth - 14, displayHeight - 14,
                            settings::fonts::pressure);

  displayCanvas->setTextSize(1, 1);
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
  time(&reading_timestamp);
  reading_temperature = doc["temperature"] | NAN;
  reading_humidity = doc["humidity"] | NAN;
  reading_pressure = doc["pressure"] | NAN;
  M5_LOGD("Received new reading: %.1f %.0f %.0f",
          reading_temperature, reading_humidity, reading_pressure);
}

void mqtt_loop()
{
  if (wifi_status && !mqtt_status)
  {
    static time_t last_timestamp;
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    if (timeinfo.tm_year < 120)
    {
      // do not connect to MQTT until time is set
      return;
    }
    if (difftime(now, last_timestamp) >= settings::mqtt::reconnect)
    {
      last_timestamp = now;
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
