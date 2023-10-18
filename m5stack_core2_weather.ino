#include <M5Unified.h>
#include <WiFi.h>

#include "fonts/all.h"
#include "icons/icons.hpp"
#include "settings.hpp"

void setup()
{
  M5.Log.setEnableColor(m5::log_target_serial, false);
  M5.Log.setLogLevel(m5::log_target_serial, ESP_LOG_DEBUG);

  auto cfg = M5.config();
  cfg.serial_baudrate = 115200;
  M5.begin(cfg);
  M5.Display.setBrightness(63);
  M5.Display.fillScreen(settings::colors::background);

  WiFi.begin(settings::wifi::ssid, settings::wifi::password);
}

void on_wifi_connected()
{
  M5_LOGI("WiFi connected");
  M5.Display.pushGrayscaleImage(
      2, 2, icons::wifi::width, icons::wifi::height,
      icons::wifi::data,
      m5gfx::grayscale_8bit,
      settings::colors::icons::wifiUp, settings::colors::background);
}

void on_wifi_disconnected()
{
  M5_LOGI("WiFi disconnected");
  M5.Display.pushGrayscaleImage(
      2, 2, icons::wifi::width, icons::wifi::height,
      icons::wifi::data,
      m5gfx::grayscale_8bit,
      settings::colors::icons::wifiDown, settings::colors::background);
}

void wifi_loop()
{
  static bool status;
  bool new_status = WiFi.isConnected();
  if (status == new_status)
    return;
  status = new_status;
  if (status)
    on_wifi_connected();
  else
    on_wifi_disconnected();
}

void loop()
{
  wifi_loop();
}
