# Generating WiFi icon data
(C) [Google](https://fonts.google.com/icons)
```
curl -L https://fonts.gstatic.com/s/i/short-term/release/materialsymbolsoutlined/wifi/wght600/20px.svg | \
   inkscape -h 640 --pipe --export-filename=- --export-type=png | \
   convert png:- -crop 640x512+0+64 -resize 20x16 txt: | \
   sed '1d;s/.*#000000/0x/;s/ .*/,/' | \
   xargs -n 20
```

# Generating MQTT icon data
(C) [MQTT.org](https://mqtt.org/)
```
curl -L https://github.com/mqtt/mqttorg-graphics/raw/master/png/mqtt-icon-transparent.png | \
   convert png:- -resize 16x16 txt: | \
   sed '1d;s/.*#660066/0x/;s/ .*/,/' | \
   xargs -n 16
```

# Generating [meteo.pl](https://www.meteo.pl/) icon data
(C) [ICM UW](https://icm.edu.pl/en/)
```
curl -L https://beta.meteo.pl/wp-content/uploads/2021/10/logo-1.svg | \
   sed '/ 133 /s/path/path id="logo"/' | \
   inkscape --export-id=logo -h 640 --pipe --export-filename=- --export-type=png | \
   convert png:- -resize 16x16 txt: | \
   sed '1d;s/.*#FFFFFF/0x/;s/ .*/,/' | \
   xargs -n 16
```