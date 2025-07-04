#include "en.h"
#include "de.h"

const LocaleStrings* get_locale_by_code(const char* code) {
    if(strcmp(code, "de") == 0) return &LOCALE_DE;
    return &LOCALE_EN;
}

