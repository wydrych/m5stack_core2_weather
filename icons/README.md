# Generating WiFi icon data

```
curl -L https://fonts.gstatic.com/s/i/short-term/release/materialsymbolsoutlined/wifi/wght600/20px.svg | \
   inkscape -h 640 --pipe --export-filename=- --export-type=png | \
   convert png:- -crop 640x512+0+64 -resize 20x16 txt: | \
   sed '1d;s/.*#000000/0x/;s/ .*/,/' | \
   xargs -n 20
```

# Generating MQTT icon data

```
curl -L https://github.com/mqtt/mqttorg-graphics/raw/master/png/mqtt-icon-transparent.png | \
   convert png:- -resize 16x16 txt: | \
   sed '1d;s/.*#660066/0x/;s/ .*/,/' | \
   xargs -n 16
```
