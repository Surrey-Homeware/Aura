#ifndef LOCALES_H
#define LOCALES_H

#include <Arduino.h>

typedef struct {
    const char *code; // e.g. "en"
    const char *name; // for dropdown
    const char *settings_title;
    const char *brightness;
    const char *location_label;
    const char *location_button;
    const char *use_f;
    const char *clock_24hr;
    const char *reset_wifi;
    const char *close;
    const char *change_location_title;
    const char *city;
    const char *search_results;
    const char *save;
    const char *cancel;
    const char *seven_day_forecast;
    const char *hourly_forecast;
    const char *feels_like;
    const char *now;
    const char *today;
    const char *language_label;
    const char *weekdays[7];
} LocaleStrings;

extern const LocaleStrings LOCALE_EN;
extern const LocaleStrings LOCALE_DE;

const LocaleStrings* get_locale_by_code(const char* code);

#endif // LOCALES_H
