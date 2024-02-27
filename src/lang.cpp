#include <string.h>

#include "lang.hpp"

Lang::Lang(const char *const sun_short,
           const char *const mon_short,
           const char *const tue_short,
           const char *const wed_short,
           const char *const thu_short,
           const char *const fri_short,
           const char *const sat_short,
           const char *time_format)
    : sun_short(sun_short),
      mon_short(mon_short),
      tue_short(tue_short),
      wed_short(wed_short),
      thu_short(thu_short),
      fri_short(fri_short),
      sat_short(sat_short),
      time_format(time_format) {}

size_t Lang::formatTime(char *buf, size_t bufsize, const struct tm *timeinfo) const
{
    const char *weekday = weekdays_short[timeinfo->tm_wday];
    size_t weekday_len = strlen(weekday);
    strncpy(buf, weekday, bufsize);
    if (weekday_len >= bufsize)
        return bufsize;
    return weekday_len + strftime(buf + weekday_len,
                                    bufsize - weekday_len,
                                    time_format,
                                    timeinfo);
}
