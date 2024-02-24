#include "icons.hpp"

#include "wifi.hpp"
#include "mqtt.hpp"
#include "meteo.hpp"

Icons::Icons()
    : wifi({wifi_width, wifi_height, wifi_data}),
      mqtt({mqtt_width, mqtt_height, mqtt_data}),
      meteo({meteo_width, meteo_height, meteo_data}) {}

Icons icons;
