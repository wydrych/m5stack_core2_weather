#pragma once

#define NUM_LANG 2
enum Lang
{
    LANG_EN = 0,
    LANG_PL
};

// translations in order of Lang
extern const char *const weekdays_short[7][NUM_LANG];

// excluding the weekday in front - is covered by weekdays_short
extern const char *const time_format[NUM_LANG];