#pragma once

#define NUM_LANG 2
enum Lang
{
    LANG_EN = 0,
    LANG_PL
};

// translations in order of Lang

const char *const weekdays_short[7][NUM_LANG] = {
    {"Sun", "nd."},
    {"Mon", "pn."},
    {"Tue", "wt."},
    {"Wed", "śr."},
    {"Thu", "czw."},
    {"Fri", "pt."},
    {"Sat", "sb."},
};

// excluding the weekday in front - is covered by weekdays_short
const char *const time_format[NUM_LANG] = {
    ", %b %d %I:%M %p",
    ", %d.%m %H:%M",
};