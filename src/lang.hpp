#pragma once

#include <time.h>

class Lang
{
private:
    union
    {
        struct
        {
            const char *const sun_short;
            const char *const mon_short;
            const char *const tue_short;
            const char *const wed_short;
            const char *const thu_short;
            const char *const fri_short;
            const char *const sat_short;
        };
        const char *const weekdays_short[7];
    };

    const char *const time_format;

public:
    Lang(const char *const sun_short,
         const char *const mon_short,
         const char *const tue_short,
         const char *const wed_short,
         const char *const thu_short,
         const char *const fri_short,
         const char *const sat_short,
         const char *time_format);
    size_t formatTime(char *buf, size_t bufsize, const struct tm *timeinfo) const;
};

#define LANG_PL Lang("nd.", "pn.", "wt.", "śr.", "czw.", "pt.", "sb.", ", %d.%m %H:%M")
#define LANG_EN Lang("", "", "", "", "", "", "", "%a, %b %d %I:%M %p")
