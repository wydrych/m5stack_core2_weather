# Fixed-cap-height fonts

Requires [fractional point support](https://github.com/adafruit/Adafruit-GFX-Library/pull/436).

## Calculate cap height of font in fractional points

```
for file in *.ttf; do
  fontconvert $file 5120f 64 95 | egrep "'[AHIM]'" | sed "s|^|$file |"
done
```

For NotoSans, 5120 fractional points result in cap height of 112 px.

## Calculate size in fractional points for desired cap height

* For both Noto Sans and Open Sans, 731 fractional points = 16 px cap size

## Generate header files

```
(
    echo '#pragma once'
    echo
    for file in *.ttf; do
        fontconvert $file 731f 32 383 >> tmp
        awk '($2 == "GFXfont") {print "#include \"" $3 ".hpp\""}' tmp
        mv tmp `awk '($2 == "GFXfont") {print $3 ".hpp"}' tmp`
    done
) > all.h
```