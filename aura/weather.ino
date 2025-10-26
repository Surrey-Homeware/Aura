#include <Arduino.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <Preferences.h>
#include "esp_system.h"
#include "translations.h"

#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS
#define LCD_BACKLIGHT_PIN 21
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

#define LATITUDE_DEFAULT "51.5074"
#define LONGITUDE_DEFAULT "-0.1278"
#define LOCATION_DEFAULT "London"
#define DEFAULT_CAPTIVE_SSID "Aura"
#define UPDATE_INTERVAL 600000UL  // 10 minutes

LV_FONT_DECLARE(lv_font_montserrat_latin_12);
LV_FONT_DECLARE(lv_font_montserrat_latin_14);
LV_FONT_DECLARE(lv_font_montserrat_latin_16);
LV_FONT_DECLARE(lv_font_montserrat_latin_20);
LV_FONT_DECLARE(lv_font_montserrat_latin_42);

static Language current_language = LANG_EN;

// Font selection based on language
const lv_font_t* get_font_12() {
  return &lv_font_montserrat_latin_12;
}

const lv_font_t* get_font_14() {
  return &lv_font_montserrat_latin_14;
}

const lv_font_t* get_font_16() {
  return &lv_font_montserrat_latin_16;
}

const lv_font_t* get_font_20() {
  return &lv_font_montserrat_latin_20;
}

const lv_font_t* get_font_42() {
  return &lv_font_montserrat_latin_42;
}

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
int x, y, z;

// Preferences
static Preferences prefs;
static bool use_fahrenheit = false;
static bool use_24_hour = false; 
static char latitude[16] = LATITUDE_DEFAULT;
static char longitude[16] = LONGITUDE_DEFAULT;
static String location = String(LOCATION_DEFAULT);
static char dd_opts[512];
static DynamicJsonDocument geoDoc(24 * 1024);
static JsonArray geoResults;

static uint32_t day_brightness = 255;
static uint32_t night_brightness = 50;
static int night_start_hour = 22; // 10 PM
static int night_end_hour = 6;    // 6 AM

bool manual_brightness_active = false;
int manual_brightness_counter = 0;
const int MANUAL_BRIGHTNESS_THRESHOLD = 5; // 3 виклики функції = 3 секунди

// UI components
static lv_obj_t *lbl_today_temp;
static lv_obj_t *lbl_today_feels_like;
static lv_obj_t *img_today_icon;
static lv_obj_t *lbl_forecast;
static lv_obj_t *box_daily;
static lv_obj_t *box_hourly_1;
static lv_obj_t *box_hourly_2;
static lv_obj_t *box_hourly_3;
static lv_obj_t *lbl_daily_day[7];
static lv_obj_t *lbl_daily_high[7];
static lv_obj_t *lbl_daily_low[7];
static lv_obj_t *img_daily[7];
static lv_obj_t *lbl_hourly[21];
static lv_obj_t *lbl_precipitation_probability[21];
static lv_obj_t *lbl_hourly_temp[21];
static lv_obj_t *img_hourly[21];
static lv_obj_t *lbl_loc;
static lv_obj_t *loc_ta;
static lv_obj_t *results_dd;
static lv_obj_t *btn_close_loc;
static lv_obj_t *btn_close_obj;
static lv_obj_t *kb;
static lv_obj_t *settings_win;
static lv_obj_t *location_win = nullptr;
static lv_obj_t *unit_switch;
static lv_obj_t *clock_24hr_switch;
static lv_obj_t *language_dropdown;
static lv_obj_t *lbl_clock;

// Weather icons
LV_IMG_DECLARE(icon_blizzard);
LV_IMG_DECLARE(icon_blowing_snow);
LV_IMG_DECLARE(icon_clear_night);
LV_IMG_DECLARE(icon_cloudy);
LV_IMG_DECLARE(icon_drizzle);
LV_IMG_DECLARE(icon_flurries);
LV_IMG_DECLARE(icon_haze_fog_dust_smoke);
LV_IMG_DECLARE(icon_heavy_rain);
LV_IMG_DECLARE(icon_heavy_snow);
LV_IMG_DECLARE(icon_isolated_scattered_tstorms_day);
LV_IMG_DECLARE(icon_isolated_scattered_tstorms_night);
LV_IMG_DECLARE(icon_mostly_clear_night);
LV_IMG_DECLARE(icon_mostly_cloudy_day);
LV_IMG_DECLARE(icon_mostly_cloudy_night);
LV_IMG_DECLARE(icon_mostly_sunny);
LV_IMG_DECLARE(icon_partly_cloudy);
LV_IMG_DECLARE(icon_partly_cloudy_night);
LV_IMG_DECLARE(icon_scattered_showers_day);
LV_IMG_DECLARE(icon_scattered_showers_night);
LV_IMG_DECLARE(icon_showers_rain);
LV_IMG_DECLARE(icon_sleet_hail);
LV_IMG_DECLARE(icon_snow_showers_snow);
LV_IMG_DECLARE(icon_strong_tstorms);
LV_IMG_DECLARE(icon_sunny);
LV_IMG_DECLARE(icon_tornado);
LV_IMG_DECLARE(icon_wintry_mix_rain_snow);

// Weather Images
LV_IMG_DECLARE(image_blizzard);
LV_IMG_DECLARE(image_blowing_snow);
LV_IMG_DECLARE(image_clear_night);
LV_IMG_DECLARE(image_cloudy);
LV_IMG_DECLARE(image_drizzle);
LV_IMG_DECLARE(image_flurries);
LV_IMG_DECLARE(image_haze_fog_dust_smoke);
LV_IMG_DECLARE(image_heavy_rain);
LV_IMG_DECLARE(image_heavy_snow);
LV_IMG_DECLARE(image_isolated_scattered_tstorms_day);
LV_IMG_DECLARE(image_isolated_scattered_tstorms_night);
LV_IMG_DECLARE(image_mostly_clear_night);
LV_IMG_DECLARE(image_mostly_cloudy_day);
LV_IMG_DECLARE(image_mostly_cloudy_night);
LV_IMG_DECLARE(image_mostly_sunny);
LV_IMG_DECLARE(image_partly_cloudy);
LV_IMG_DECLARE(image_partly_cloudy_night);
LV_IMG_DECLARE(image_scattered_showers_day);
LV_IMG_DECLARE(image_scattered_showers_night);
LV_IMG_DECLARE(image_showers_rain);
LV_IMG_DECLARE(image_sleet_hail);
LV_IMG_DECLARE(image_snow_showers_snow);
LV_IMG_DECLARE(image_strong_tstorms);
LV_IMG_DECLARE(image_sunny);
LV_IMG_DECLARE(image_tornado);
LV_IMG_DECLARE(image_wintry_mix_rain_snow);

void create_ui();
void fetch_and_update_weather();
void create_settings_window();
static void screen_event_cb(lv_event_t *e);
static void settings_event_handler(lv_event_t *e);
const lv_img_dsc_t *choose_image(int wmo_code, int is_day);
const lv_img_dsc_t *choose_icon(int wmo_code, int is_day);

int day_of_week(int y, int m, int d) {
  static const int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  if (m < 3) y -= 1;
  return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

String hour_of_day(int hour) {
  const LocalizedStrings* strings = get_strings(current_language);
  if(hour < 0 || hour > 23) return String(strings->invalid_hour);

  if (use_24_hour) {
    if (hour < 10)
      return String("0") + String(hour) + ":00";
    else
      return String(hour) + ":00";
  } else {
    if(hour == 0)   return String("12:00") + strings->am;
    if(hour == 12)  return String(strings->noon);

    bool isMorning = (hour < 12);
    String suffix = isMorning ? strings->am : strings->pm;

    int displayHour = hour % 12;

    return String(displayHour) + ":00" + suffix;
  }
}

String urlencode(const String &str) {
  String encoded = "";
  char buf[5];
  for (size_t i = 0; i < str.length(); i++) {
    char c = str.charAt(i);
    // Unreserved characters according to RFC 3986
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += c;
    } else {
      // Percent-encode others
      sprintf(buf, "%%%02X", (unsigned char)c);
      encoded += buf;
    }
  }
  return encoded;
}

static void update_clock(lv_timer_t *timer) {
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) return;

  const LocalizedStrings* strings = get_strings(current_language);
  char buf[16];
  if (use_24_hour) {
    snprintf(buf, sizeof(buf), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  } else {
    int hour = timeinfo.tm_hour % 12;
    if(hour == 0) hour = 12;
    const char *ampm = (timeinfo.tm_hour < 12) ? strings->am : strings->pm;
    snprintf(buf, sizeof(buf), "%d:%02d%s", hour, timeinfo.tm_min, ampm);
  }
  lv_label_set_text(lbl_clock, buf);   

  apply_current_brightness(timeinfo.tm_hour);
}

static void ta_event_cb(lv_event_t *e) {
  lv_obj_t *ta = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t *kb = (lv_obj_t *)lv_event_get_user_data(e);

  // Show keyboard
  lv_keyboard_set_textarea(kb, ta);
  lv_obj_move_foreground(kb);
  lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
}

static void kb_event_cb(lv_event_t *e) {
  lv_obj_t *kb = static_cast<lv_obj_t *>(lv_event_get_target(e));
  lv_obj_add_flag((lv_obj_t *)lv_event_get_target(e), LV_OBJ_FLAG_HIDDEN);

  if (lv_event_get_code(e) == LV_EVENT_READY) {
    const char *loc = lv_textarea_get_text(loc_ta);
    if (strlen(loc) > 0) {
      do_geocode_query(loc);
    }
  }
}

static void ta_defocus_cb(lv_event_t *e) {
  lv_obj_add_flag((lv_obj_t *)lv_event_get_user_data(e), LV_OBJ_FLAG_HIDDEN);
}

void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data) {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();

    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  TFT_eSPI tft = TFT_eSPI();
  tft.init();
  pinMode(LCD_BACKLIGHT_PIN, OUTPUT);

  lv_init();

  // Init touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(0);

  lv_display_t *disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_indev_t *indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, touchscreen_read);

  // Load saved prefs
  prefs.begin("weather", false);
  String lat = prefs.getString("latitude", LATITUDE_DEFAULT);
  lat.toCharArray(latitude, sizeof(latitude));
  String lon = prefs.getString("longitude", LONGITUDE_DEFAULT);
  lon.toCharArray(longitude, sizeof(longitude));
  use_fahrenheit = prefs.getBool("useFahrenheit", false);
  location = prefs.getString("location", LOCATION_DEFAULT);
  uint32_t brightness = prefs.getUInt("brightness", 255);
  use_24_hour = prefs.getBool("use24Hour", false);
  current_language = (Language)prefs.getUInt("language", LANG_EN);
  analogWrite(LCD_BACKLIGHT_PIN, brightness);
			 
  // --- Load new brightness prefs ---
  day_brightness = prefs.getUInt("dayBright", 255);
  night_brightness = prefs.getUInt("nightBright", 50);
  night_start_hour = prefs.getInt("nightStart", 22);
  night_end_hour = prefs.getInt("nightEnd", 6);

  // Check for Wi-Fi config and request it if not available
  WiFiManager wm;
  wm.setTimeout(120); // Setting the connection wait timeout to 120 seconds
  wm.setAPCallback(apModeCallback);
  wm.autoConnect(DEFAULT_CAPTIVE_SSID);

  lv_timer_create(update_clock, 1000, NULL);

  lv_obj_clean(lv_scr_act());
  create_ui();
  fetch_and_update_weather();	

  // --- Call initial brightness update after time config ---
  // Ensure time is configured before setting brightness based on time
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
      apply_current_brightness(timeinfo.tm_hour);
  } else {
      // Fallback if time not yet available, use day brightness
      analogWrite(LCD_BACKLIGHT_PIN, day_brightness);
  }
}

void flush_wifi_splashscreen(uint32_t ms = 200) {
  uint32_t start = millis();
  while (millis() - start < ms) {
    lv_timer_handler();
    delay(5);
  }
}

void apModeCallback(WiFiManager *mgr) {
  wifi_splash_screen();
  flush_wifi_splashscreen();
}

void loop() {
  lv_timer_handler();
  static uint32_t last = millis();

  if (millis() - last >= UPDATE_INTERVAL) {
    fetch_and_update_weather();
    last = millis();
  }

  lv_tick_inc(5);
  delay(5);
}

void apply_current_brightness(int current_hour) {
  uint32_t current_set_brightness;
  if (manual_brightness_active) {
    manual_brightness_counter++; // We increment the counter with each call to apply_current_brightness (every second)

    if (manual_brightness_counter >= MANUAL_BRIGHTNESS_THRESHOLD) {
      // 5 seconds have passed, turn off manual mode and allow automatic brightness to apply
      manual_brightness_active = false;
      manual_brightness_counter = 0; // Reset the counter

    } else {
      // 5 seconds have not yet passed, leave the brightness as it is (it has already been set with the slider)
      return; // Exit the function without changing the brightness
    }
  }

  if (night_start_hour <= night_end_hour) { 
    if (current_hour >= night_start_hour && current_hour < night_end_hour) {
      current_set_brightness = night_brightness;
    } else {
      current_set_brightness = day_brightness;
    }
  } else { 
    if (current_hour >= night_start_hour || current_hour < night_end_hour) {
      current_set_brightness = night_brightness;
    } else {
      current_set_brightness = day_brightness;
    }
  }
  analogWrite(LCD_BACKLIGHT_PIN, current_set_brightness);
}

void wifi_splash_screen() {
  lv_obj_t *scr = lv_scr_act();
  lv_obj_clean(scr);
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x4c8cb9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0xa6cdec), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_t *lbl = lv_label_create(scr);
  lv_label_set_text(lbl, strings->wifi_config);
  lv_obj_set_style_text_font(lbl, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(lbl, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(lbl);
  lv_scr_load(scr);
}

void create_ui() {
  lv_obj_t *scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x4c8cb9), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0xa6cdec), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);

  // Trigger settings screen on touch
  lv_obj_add_event_cb(scr, screen_event_cb, LV_EVENT_CLICKED, NULL);

  img_today_icon = lv_img_create(scr);
  lv_img_set_src(img_today_icon, &image_partly_cloudy);
  lv_obj_align(img_today_icon, LV_ALIGN_TOP_MID, -64, 4);

  static lv_style_t default_label_style;
  lv_style_init(&default_label_style);
  lv_style_set_text_color(&default_label_style, lv_color_hex(0xFFFFFF));
  lv_style_set_text_opa(&default_label_style, LV_OPA_COVER);

  const LocalizedStrings* strings = get_strings(current_language);

  lbl_today_temp = lv_label_create(scr);
  lv_label_set_text(lbl_today_temp, strings->temp_placeholder);
  lv_obj_set_style_text_font(lbl_today_temp, get_font_42(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_today_temp, LV_ALIGN_TOP_MID, 45, 35);
  lv_obj_add_style(lbl_today_temp, &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);

  lbl_today_feels_like = lv_label_create(scr);
  lv_label_set_text(lbl_today_feels_like, strings->feels_like_temp);
  lv_obj_set_style_text_font(lbl_today_feels_like, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_today_feels_like, lv_color_hex(0xe4ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_today_feels_like, LV_ALIGN_TOP_MID, 45, 75);

  lbl_forecast = lv_label_create(scr);
  lv_label_set_text(lbl_forecast, strings->seven_day_forecast);
  lv_obj_set_style_text_font(lbl_forecast, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_forecast, lv_color_hex(0xe4ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_forecast, LV_ALIGN_TOP_LEFT, 20, 110);

  box_daily = lv_obj_create(scr);
  lv_obj_set_size(box_daily, 220, 180);
  lv_obj_align(box_daily, LV_ALIGN_TOP_LEFT, 10, 135);
  lv_obj_set_style_bg_color(box_daily, lv_color_hex(0x5e9bc8), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(box_daily, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(box_daily, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_daily, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_daily, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_daily, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_daily, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_daily, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_daily, daily_cb, LV_EVENT_CLICKED, NULL);

  for (int i = 0; i < 7; i++) {
    lbl_daily_day[i] = lv_label_create(box_daily);
    lbl_daily_high[i] = lv_label_create(box_daily);
    lbl_daily_low[i] = lv_label_create(box_daily);
    img_daily[i] = lv_img_create(box_daily);

    lv_obj_add_style(lbl_daily_day[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_daily_day[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_daily_day[i], LV_ALIGN_TOP_LEFT, 2, i * 24);

    lv_obj_add_style(lbl_daily_high[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_daily_high[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_daily_high[i], LV_ALIGN_TOP_RIGHT, 0, i * 24);

    lv_label_set_text(lbl_daily_low[i], "");
    lv_obj_set_style_text_color(lbl_daily_low[i], lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_daily_low[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_daily_low[i], LV_ALIGN_TOP_RIGHT, -50, i * 24);

    lv_img_set_src(img_daily[i], &icon_partly_cloudy);
    lv_obj_align(img_daily[i], LV_ALIGN_TOP_LEFT, 80, i * 24);
  }

  box_hourly_1 = lv_obj_create(scr);
  lv_obj_set_size(box_hourly_1, 220, 180);
  lv_obj_align(box_hourly_1, LV_ALIGN_TOP_LEFT, 10, 135);
  lv_obj_set_style_bg_color(box_hourly_1, lv_color_hex(0x5e9bc8), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(box_hourly_1, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(box_hourly_1, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_hourly_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_hourly_1, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_hourly_1, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_hourly_1, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_hourly_1, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_hourly_1, hourly_1_cb, LV_EVENT_CLICKED, NULL);

  for (int i = 0; i < 7; i++) {
    lbl_hourly[i] = lv_label_create(box_hourly_1);
    lbl_precipitation_probability[i] = lv_label_create(box_hourly_1);
    lbl_hourly_temp[i] = lv_label_create(box_hourly_1);
    img_hourly[i] = lv_img_create(box_hourly_1);

    lv_obj_add_style(lbl_hourly[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly[i], LV_ALIGN_TOP_LEFT, 2, i * 24);

    lv_obj_add_style(lbl_hourly_temp[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly_temp[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly_temp[i], LV_ALIGN_TOP_RIGHT, 0, i * 24);

    lv_label_set_text(lbl_precipitation_probability[i], "");
    lv_obj_set_style_text_color(lbl_precipitation_probability[i], lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_precipitation_probability[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_precipitation_probability[i], LV_ALIGN_TOP_RIGHT, -55, i * 24);

    lv_img_set_src(img_hourly[i], &icon_partly_cloudy);
    lv_obj_align(img_hourly[i], LV_ALIGN_TOP_LEFT, 80, i * 24);
  }

  box_hourly_2 = lv_obj_create(scr);
  lv_obj_set_size(box_hourly_2, 220, 180);
  lv_obj_align(box_hourly_2, LV_ALIGN_TOP_LEFT, 10, 135);
  lv_obj_set_style_bg_color(box_hourly_2, lv_color_hex(0x5e9bc8), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(box_hourly_2, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(box_hourly_2, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_hourly_2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_hourly_2, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_hourly_2, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_hourly_2, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_hourly_2, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_hourly_2, hourly_2_cb, LV_EVENT_CLICKED, NULL);

  for (int i = 7; i < 14; i++) {
    lbl_hourly[i] = lv_label_create(box_hourly_2);
    lbl_precipitation_probability[i] = lv_label_create(box_hourly_2);
    lbl_hourly_temp[i] = lv_label_create(box_hourly_2);
    img_hourly[i] = lv_img_create(box_hourly_2);

    lv_obj_add_style(lbl_hourly[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly[i], LV_ALIGN_TOP_LEFT, 2, (i - 7) * 24);

    lv_obj_add_style(lbl_hourly_temp[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly_temp[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly_temp[i], LV_ALIGN_TOP_RIGHT, 0, (i - 7) * 24);

    lv_label_set_text(lbl_precipitation_probability[i], "");
    lv_obj_set_style_text_color(lbl_precipitation_probability[i], lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_precipitation_probability[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_precipitation_probability[i], LV_ALIGN_TOP_RIGHT, -55, (i - 7) * 24);

    lv_img_set_src(img_hourly[i], &icon_partly_cloudy);
    lv_obj_align(img_hourly[i], LV_ALIGN_TOP_LEFT, 80, (i - 7) * 24);
  }

  box_hourly_3 = lv_obj_create(scr);
  lv_obj_set_size(box_hourly_3, 220, 180);
  lv_obj_align(box_hourly_3, LV_ALIGN_TOP_LEFT, 10, 135);
  lv_obj_set_style_bg_color(box_hourly_3, lv_color_hex(0x5e9bc8), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(box_hourly_3, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(box_hourly_3, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_hourly_3, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_hourly_3, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_hourly_3, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_hourly_3, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_hourly_3, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_hourly_3, hourly_3_cb, LV_EVENT_CLICKED, NULL);

  for (int i = 14; i < 21; i++) {
    lbl_hourly[i] = lv_label_create(box_hourly_3);
    lbl_precipitation_probability[i] = lv_label_create(box_hourly_3);
    lbl_hourly_temp[i] = lv_label_create(box_hourly_3);
    img_hourly[i] = lv_img_create(box_hourly_3);

    lv_obj_add_style(lbl_hourly[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly[i], LV_ALIGN_TOP_LEFT, 2, (i - 14) * 24);

    lv_obj_add_style(lbl_hourly_temp[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly_temp[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly_temp[i], LV_ALIGN_TOP_RIGHT, 0, (i - 14) * 24);

    lv_label_set_text(lbl_precipitation_probability[i], "");
    lv_obj_set_style_text_color(lbl_precipitation_probability[i], lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_precipitation_probability[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_precipitation_probability[i], LV_ALIGN_TOP_RIGHT, -55, (i - 14) * 24);

    lv_img_set_src(img_hourly[i], &icon_partly_cloudy);
    lv_obj_align(img_hourly[i], LV_ALIGN_TOP_LEFT, 80, (i - 14) * 24);
  }

  lv_obj_add_flag(box_hourly_1, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(box_hourly_2, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(box_hourly_3, LV_OBJ_FLAG_HIDDEN);

  // Create clock label in the top-right corner
  lbl_clock = lv_label_create(scr);
  lv_obj_set_style_text_font(lbl_clock, get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_clock, lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(lbl_clock, "");
  lv_obj_align(lbl_clock, LV_ALIGN_TOP_RIGHT, -10, 2);
}

void populate_results_dropdown() {
  dd_opts[0] = '\0';

  for (JsonObject item : geoResults) {
    // Get name from JSON and replace apostrophe
    String name = item["name"].as<const char*>();
    name.replace("’", "'");

    strcat(dd_opts, name.c_str());

    if (item["admin1"]) {
      String admin1 = item["admin1"].as<const char*>();
      admin1.replace("’", "'");
      strcat(dd_opts, ", ");
      strcat(dd_opts, admin1.c_str());
    }

    strcat(dd_opts, "\n");
  }

  if (geoResults.size() > 0) {
    lv_dropdown_set_options_static(results_dd, dd_opts);
    lv_obj_add_flag(results_dd, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(btn_close_loc, lv_palette_main(LV_PALETTE_GREEN), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(btn_close_loc, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(btn_close_loc, lv_palette_darken(LV_PALETTE_GREEN, 1), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_flag(btn_close_loc, LV_OBJ_FLAG_CLICKABLE);
  }
}

static void location_save_event_cb(lv_event_t *e) {
  JsonArray *pres = static_cast<JsonArray *>(lv_event_get_user_data(e));
  uint16_t idx = lv_dropdown_get_selected(results_dd);

  JsonObject obj = (*pres)[idx];
  double lat = obj["latitude"].as<double>();
  double lon = obj["longitude"].as<double>();

  snprintf(latitude, sizeof(latitude), "%.6f", lat);
  snprintf(longitude, sizeof(longitude), "%.6f", lon);
  prefs.putString("latitude", latitude);
  prefs.putString("longitude", longitude);

  // We get name, admin as String and replace the apostrophe
  String name = obj["name"].as<const char*>();
  name.replace("’", "'");

  String admin;
  if (obj["admin1"]) {
    admin = obj["admin1"].as<const char*>();
    admin.replace("’", "'");
  }

  String opts = name;
  if (admin.length() > 0) {
    opts += ", ";
    opts += admin;
  }
  
  prefs.putString("location", opts);
  location = prefs.getString("location");

  // Re‐fetch weather immediately
  lv_label_set_text(lbl_loc, opts.c_str());
  fetch_and_update_weather();

  lv_obj_del(location_win);
  location_win = nullptr;
}

static void location_cancel_event_cb(lv_event_t *e) {
  lv_obj_del(location_win);
  location_win = nullptr;
}

void screen_event_cb(lv_event_t *e) {
  create_settings_window();
}

void daily_cb(lv_event_t *e) {
  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_add_flag(box_daily, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(lbl_forecast, strings->hourly_forecast);
  lv_obj_clear_flag(box_hourly_1, LV_OBJ_FLAG_HIDDEN);
}

void hourly_1_cb(lv_event_t *e) {
  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_add_flag(box_hourly_1, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(lbl_forecast, strings->hourly_forecast);
  lv_obj_clear_flag(box_hourly_2, LV_OBJ_FLAG_HIDDEN);
}

void hourly_2_cb(lv_event_t *e) {
  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_add_flag(box_hourly_2, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(lbl_forecast, strings->hourly_forecast);
  lv_obj_clear_flag(box_hourly_3, LV_OBJ_FLAG_HIDDEN);
}

void hourly_3_cb(lv_event_t *e) {
  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_add_flag(box_hourly_3, LV_OBJ_FLAG_HIDDEN);
  lv_label_set_text(lbl_forecast, strings->seven_day_forecast);
  lv_obj_clear_flag(box_daily, LV_OBJ_FLAG_HIDDEN);
}

static void reset_wifi_event_handler(lv_event_t *e) {
  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_t *mbox = lv_msgbox_create(lv_scr_act());
  lv_obj_t *title = lv_msgbox_add_title(mbox, strings->reset);
  lv_obj_set_style_margin_left(title, 10, 0);
  lv_obj_set_style_text_font(title, get_font_16(), 0);

  lv_obj_t *text = lv_msgbox_add_text(mbox, strings->reset_confirmation);
  lv_obj_set_style_text_font(text, get_font_12(), 0);
  lv_msgbox_add_close_button(mbox);

  lv_obj_t *btn_no = lv_msgbox_add_footer_button(mbox, strings->cancel);
  lv_obj_set_style_text_font(btn_no, get_font_12(), 0);
  lv_obj_t *btn_yes = lv_msgbox_add_footer_button(mbox, strings->reset);
  lv_obj_set_style_text_font(btn_yes, get_font_12(), 0);

  lv_obj_set_style_bg_color(btn_yes, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(btn_yes, lv_palette_darken(LV_PALETTE_RED, 1), LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_text_color(btn_yes, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_obj_set_width(mbox, 230);
  lv_obj_center(mbox);

  lv_obj_set_style_border_width(mbox, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(mbox, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_border_opa(mbox, LV_OPA_COVER,   LV_PART_MAIN);
  lv_obj_set_style_radius(mbox, 4, LV_PART_MAIN);

  lv_obj_add_event_cb(btn_yes, reset_confirm_yes_cb, LV_EVENT_CLICKED, mbox);
  lv_obj_add_event_cb(btn_no, reset_confirm_no_cb, LV_EVENT_CLICKED, mbox);
}

static void reset_confirm_yes_cb(lv_event_t *e) {
  lv_obj_t *mbox = (lv_obj_t *)lv_event_get_user_data(e);
  Serial.println("Clearing Wi-Fi creds and rebooting");
  WiFiManager wm;
  wm.resetSettings();
  delay(100);
  esp_restart();
}

static void reset_confirm_no_cb(lv_event_t *e) {
  lv_obj_t *mbox = (lv_obj_t *)lv_event_get_user_data(e);
  lv_obj_del(mbox);
}

static void change_location_event_cb(lv_event_t *e) {
  if (location_win) return;

  create_location_dialog();
}

void create_location_dialog() {
  const LocalizedStrings* strings = get_strings(current_language);
  location_win = lv_win_create(lv_scr_act());
  lv_obj_t *title = lv_win_add_title(location_win, strings->change_location);
  lv_obj_t *header = lv_win_get_header(location_win);
  lv_obj_set_style_height(header, 30, 0);
  lv_obj_set_style_text_font(title, get_font_16(), 0);
  lv_obj_set_style_margin_left(title, 10, 0);
  lv_obj_set_size(location_win, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_pos(location_win, 0, 0); // Position at top-left (full screen implies this)
  lv_obj_clear_flag(location_win, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *cont = lv_win_get_content(location_win);
  lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE); // Prevent content from scrolling

  lv_obj_t *lbl = lv_label_create(cont);
  lv_label_set_text(lbl, strings->city);
  lv_obj_set_style_text_font(lbl, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 5, 10);

  loc_ta = lv_textarea_create(cont);
  lv_textarea_set_one_line(loc_ta, true);
  lv_textarea_set_placeholder_text(loc_ta, strings->city_placeholder);
  lv_obj_set_width(loc_ta, 140);
  lv_obj_align(loc_ta, LV_ALIGN_TOP_RIGHT, 0, 0);

  lv_obj_add_event_cb(loc_ta, ta_event_cb, LV_EVENT_CLICKED, kb);
  lv_obj_add_event_cb(loc_ta, ta_defocus_cb, LV_EVENT_DEFOCUSED, kb);

  lv_obj_t *lbl2 = lv_label_create(cont);
  lv_label_set_text(lbl2, strings->search_results);
  lv_obj_set_style_text_font(lbl2, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl2, LV_ALIGN_TOP_LEFT, 5, 50);

  results_dd = lv_dropdown_create(cont);
  lv_obj_set_width(results_dd, 214);
  lv_obj_align(results_dd, LV_ALIGN_TOP_MID, 0, 70);
  lv_obj_set_style_text_font(results_dd, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(results_dd, get_font_14(), LV_PART_SELECTED | LV_STATE_DEFAULT);

  lv_obj_t *list = lv_dropdown_get_list(results_dd);
  lv_obj_set_style_text_font(list, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);

  lv_dropdown_set_options(results_dd, "");
  lv_obj_clear_flag(results_dd, LV_OBJ_FLAG_CLICKABLE);

  btn_close_loc = lv_btn_create(cont);
  lv_obj_set_size(btn_close_loc, 80, 40);
  lv_obj_align(btn_close_loc, LV_ALIGN_BOTTOM_RIGHT, 0, 0);

  lv_obj_add_event_cb(btn_close_loc, location_save_event_cb, LV_EVENT_CLICKED, &geoResults);
  lv_obj_set_style_bg_color(btn_close_loc, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(btn_close_loc, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(btn_close_loc, lv_palette_darken(LV_PALETTE_GREY, 1), LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_clear_flag(btn_close_loc, LV_OBJ_FLAG_CLICKABLE);

  lv_obj_t *lbl_close = lv_label_create(btn_close_loc);
  lv_label_set_text(lbl_close, strings->save);
  lv_obj_set_style_text_font(lbl_close, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lbl_close);

  lv_obj_t *btn_cancel_loc = lv_btn_create(cont);
  lv_obj_set_size(btn_cancel_loc, 80, 40);
  lv_obj_align_to(btn_cancel_loc, btn_close_loc, LV_ALIGN_OUT_LEFT_MID, -5, 0);
  lv_obj_add_event_cb(btn_cancel_loc, location_cancel_event_cb, LV_EVENT_CLICKED, &geoResults);

  lv_obj_t *lbl_cancel = lv_label_create(btn_cancel_loc);
  lv_label_set_text(lbl_cancel, strings->cancel);
  lv_obj_set_style_text_font(lbl_cancel, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lbl_cancel);
}

void create_settings_window() {
  if (settings_win) return;	// Prevent creating multiple windows

  int vertical_element_spacing = 21;

  const LocalizedStrings* strings = get_strings(current_language);
  settings_win = lv_win_create(lv_scr_act());
  // Set window size to full screen

  lv_obj_set_size(settings_win, SCREEN_WIDTH, SCREEN_HEIGHT);
  lv_obj_set_pos(settings_win, 0, 0); // Position at top-left (full screen implies this)
  
  lv_obj_clear_flag(settings_win, LV_OBJ_FLAG_SCROLLABLE); // Prevent window dragging

  // Set background color for the window itself to ensure full opacity
  lv_obj_set_style_bg_color(settings_win, lv_color_white(), LV_PART_MAIN); 
  lv_obj_set_style_bg_opa(settings_win, LV_OPA_COVER, LV_PART_MAIN);

  lv_obj_t *title_label = lv_win_add_title(settings_win, strings->aura_settings);
  lv_obj_set_style_text_font(title_label, get_font_20(), 0); 
  // Set margin for the label itself to shift text to the right within its parent (the title bar).
  //lv_obj_set_style_margin_left(title_label, 10, 0);
  lv_obj_set_style_text_align(title_label, LV_TEXT_ALIGN_CENTER, 0);
  // Prevent dragging/scrolling of title text (separate calls)
  lv_obj_clear_flag(title_label, LV_OBJ_FLAG_SCROLLABLE); 
  lv_obj_clear_flag(title_label, LV_OBJ_FLAG_PRESS_LOCK);

  // Get the title bar object (it's typically the first child of the window)
  lv_obj_t *title_bar = lv_obj_get_child(settings_win, 0); 
  if (title_bar != NULL) {
    lv_obj_set_height(title_bar, 50); // Set fixed height of the title bar to 50 pixels
    // Adjust vertical padding of the title bar to center the text roughly within the 50px height.
    // For a 20px font, 15px top/bottom padding makes 20 + 15 + 15 = 50px total height.
    lv_obj_set_style_pad_ver(title_bar, 15, LV_PART_MAIN); 
    lv_obj_clear_flag(title_bar, LV_OBJ_FLAG_SCROLLABLE); // Disable dragging of the title bar
  }

  lv_obj_t *cont = lv_win_get_content(settings_win); // Get the content area of the window
  lv_obj_set_width(cont, LV_PCT(100)); // Content area takes 100% width of the window
  lv_obj_set_height(cont, LV_SIZE_CONTENT); // Content area height adjusts to children (will scroll vertically if needed)
  
  // Set padding for the content area (left/right padding for all children)
  //lv_obj_set_style_pad_all(cont, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_left(cont, 10, LV_PART_MAIN); 
  lv_obj_set_style_pad_right(cont, 20, LV_PART_MAIN);

  // Calculate width for elements within the content area (240px screen - 10px left pad - 10px right pad = 220px)
  const int content_element_width = SCREEN_WIDTH - (10 + 20); 

  // Set the main content area to a COLUMN FLEX layout
  lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
  // Align items to start (top) on main axis, and start (left) on cross axis
  lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START); 
  lv_obj_set_style_layout(cont, LV_LAYOUT_FLEX, 0); // Enable Flex layout

  // Buffer for hour options for dropdowns
  static char hours_options[24 * 4 + 1]; // "00\n01\n..." 24 hours * 4 chars each ('HH\n\0') + 1 for final \0
  // Initialize only once to save memory and time
  if (hours_options[0] == '\0') { 
    for (int i = 0; i < 24; ++i) {
      char h_str[4]; 
      sprintf(h_str, "%02d\n", i);
      strcat(hours_options, h_str);
    }
    // Remove the last newline character to avoid an empty option
    if (strlen(hours_options) > 0) {
        hours_options[strlen(hours_options) - 1] = '\0';
    }
  }


  // --- Day Brightness Slider ---
  lv_obj_t *lbl_day_b = lv_label_create(cont);
  lv_label_set_text(lbl_day_b, strings->brightness_day);
  lv_obj_set_style_text_font(lbl_day_b, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_width(lbl_day_b, LV_PCT(100)); // Label takes full width of its flex cell

  lv_obj_t *slider_day = lv_slider_create(cont);
  lv_slider_set_range(slider_day, 5, 255);
  lv_slider_set_value(slider_day, day_brightness, LV_ANIM_OFF);
  lv_obj_set_width(slider_day, content_element_width - 10);
  lv_obj_set_style_margin_left(slider_day, 5, LV_PART_MAIN);

  lv_obj_add_event_cb(slider_day, [](lv_event_t *e){
    lv_obj_t *s = (lv_obj_t*)lv_event_get_target(e);
    day_brightness = lv_slider_get_value(s);
    analogWrite(LCD_BACKLIGHT_PIN, day_brightness); // Immediate preview
    manual_brightness_active = true;
    manual_brightness_counter = 0; // Reset counter
  }, LV_EVENT_VALUE_CHANGED, NULL);
  // Add event for RELEASED to activate manual brightness mode
  lv_obj_add_event_cb(slider_day, [](lv_event_t *e){
    lv_obj_t *s = (lv_obj_t*)lv_event_get_target(e);
    day_brightness = lv_slider_get_value(s); // Ensure final value is captured
    analogWrite(LCD_BACKLIGHT_PIN, day_brightness); // Final apply
    manual_brightness_active = true;
    manual_brightness_counter = 0; // Reset counter
  }, LV_EVENT_RELEASED, NULL);


  // --- Night Brightness Slider ---
  lv_obj_t *lbl_night_b = lv_label_create(cont);
  lv_label_set_text(lbl_night_b, strings->brightness_night);
  lv_obj_set_style_text_font(lbl_night_b, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_width(lbl_night_b, LV_PCT(100));
  lv_obj_set_style_pad_top(lbl_night_b, 10, LV_PART_MAIN); // Gap from previous slider group

  lv_obj_t *slider_night = lv_slider_create(cont);
  lv_slider_set_range(slider_night, 5, 255);
  lv_slider_set_value(slider_night, night_brightness, LV_ANIM_OFF);
  lv_obj_set_width(slider_night, content_element_width - 10);
  lv_obj_set_style_margin_left(slider_night, 5, LV_PART_MAIN);

  lv_obj_add_event_cb(slider_night, [](lv_event_t *e){
    lv_obj_t *s = (lv_obj_t*)lv_event_get_target(e);
    night_brightness = lv_slider_get_value(s);
    analogWrite(LCD_BACKLIGHT_PIN, night_brightness); // Immediate preview
    manual_brightness_active = true;
    manual_brightness_counter = 0; // Reset counter
  }, LV_EVENT_VALUE_CHANGED, NULL);
  // Add event for RELEASED to activate manual brightness mode
  lv_obj_add_event_cb(slider_night, [](lv_event_t *e){
    lv_obj_t *s = (lv_obj_t*)lv_event_get_target(e);
    night_brightness = lv_slider_get_value(s); // Ensure final value is captured
    analogWrite(LCD_BACKLIGHT_PIN, night_brightness); // Final apply
    manual_brightness_active = true;
    manual_brightness_counter = 0; // Reset counter
  }, LV_EVENT_RELEASED, NULL);


  // --- Night Mode Start Hour (Label and Dropdown in a row) ---
  lv_obj_t *cont_start_hour = lv_obj_create(cont); // Intermediate container for row layout
  lv_obj_set_size(cont_start_hour, content_element_width, LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(cont_start_hour, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(cont_start_hour, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_clear_flag(cont_start_hour, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(cont_start_hour, 0, LV_PART_MAIN); // No internal padding for this sub-container
  lv_obj_set_style_bg_opa(cont_start_hour, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(cont_start_hour, 0, 0);

  lv_obj_t *lbl_night_start = lv_label_create(cont_start_hour);
  lv_label_set_text(lbl_night_start, strings->night_start);
  lv_obj_set_style_text_font(lbl_night_start, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  
  lv_obj_t *dd_start_hour = lv_dropdown_create(cont_start_hour); 
  lv_dropdown_set_options_static(dd_start_hour, hours_options);
  lv_dropdown_set_selected(dd_start_hour, night_start_hour); 
  lv_obj_set_width(dd_start_hour, 80); // Fixed width for dropdown
  lv_obj_add_event_cb(dd_start_hour, [](lv_event_t *e){
    lv_obj_t *dd = (lv_obj_t*)lv_event_get_target(e);
    night_start_hour = lv_dropdown_get_selected(dd);
    prefs.putUInt("night_start_hour", night_start_hour); // Save immediately
  }, LV_EVENT_VALUE_CHANGED, NULL);


  // --- Night Mode End Hour (Label and Dropdown in a row) ---
  lv_obj_t *cont_end_hour = lv_obj_create(cont);
  lv_obj_set_size(cont_end_hour, content_element_width, LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(cont_end_hour, LV_FLEX_FLOW_ROW); 
  lv_obj_set_flex_align(cont_end_hour, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_clear_flag(cont_end_hour, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(cont_end_hour, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(cont_end_hour, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(cont_end_hour, 0, 0);

  lv_obj_t *lbl_night_end = lv_label_create(cont_end_hour);
  lv_label_set_text(lbl_night_end, strings->night_stop);
  lv_obj_set_style_text_font(lbl_night_end, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  
  lv_obj_t *dd_end_hour = lv_dropdown_create(cont_end_hour); 
  lv_dropdown_set_options_static(dd_end_hour, hours_options); 
  lv_dropdown_set_selected(dd_end_hour, night_end_hour); 
  lv_obj_set_width(dd_end_hour, 80); 
  lv_obj_add_event_cb(dd_end_hour, [](lv_event_t *e){
    lv_obj_t *dd = (lv_obj_t*)lv_event_get_target(e);
    night_end_hour = lv_dropdown_get_selected(dd);
    prefs.putUInt("night_end_hour", night_end_hour); // Save immediately
  }, LV_EVENT_VALUE_CHANGED, NULL);


  // 'Use F' switch
  lv_obj_t *cont_temp_unit = lv_obj_create(cont);
  lv_obj_set_size(cont_temp_unit, content_element_width, LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(cont_temp_unit, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(cont_temp_unit, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_clear_flag(cont_temp_unit, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(cont_temp_unit, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(cont_temp_unit, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(cont_temp_unit, 0, 0);

  lv_obj_t *lbl_u = lv_label_create(cont_temp_unit);
  lv_label_set_text(lbl_u, strings->fahrenheit);
  lv_obj_set_style_text_font(lbl_u, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);

  unit_switch = lv_switch_create(cont_temp_unit);
  if (use_fahrenheit) {
    lv_obj_add_state(unit_switch, LV_STATE_CHECKED);
  } else {
    lv_obj_remove_state(unit_switch, LV_STATE_CHECKED);
  }
  lv_obj_add_event_cb(unit_switch, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);


  // 24-hr time switch
  lv_obj_t *cont_24hr_clock = lv_obj_create(cont);
  lv_obj_set_size(cont_24hr_clock, content_element_width, LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(cont_24hr_clock, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(cont_24hr_clock, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_clear_flag(cont_24hr_clock, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(cont_24hr_clock, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(cont_24hr_clock, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(cont_24hr_clock, 0, 0);

  lv_obj_t *lbl_24hr = lv_label_create(cont_24hr_clock);
  lv_label_set_text(lbl_24hr, strings->use_24hr);
  lv_obj_set_style_text_font(lbl_24hr, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  
  clock_24hr_switch = lv_switch_create(cont_24hr_clock);
  if (use_24_hour) {
    lv_obj_add_state(clock_24hr_switch, LV_STATE_CHECKED);
  } else {
    lv_obj_clear_state(clock_24hr_switch, LV_STATE_CHECKED);
  }
  lv_obj_add_event_cb(clock_24hr_switch, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);


  // Current Location label
  lv_obj_t *lbl_loc_l = lv_label_create(cont);
  lv_label_set_text(lbl_loc_l, strings->location);
  lv_obj_set_style_text_font(lbl_loc_l, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(lbl_loc_l, lbl_u, LV_ALIGN_OUT_BOTTOM_LEFT, 0, vertical_element_spacing);

  lbl_loc = lv_label_create(cont);
  lv_label_set_text(lbl_loc, location.c_str());
  lv_obj_set_style_text_font(lbl_loc, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(lbl_loc, lbl_loc_l, LV_ALIGN_OUT_RIGHT_MID, 5, 0);


  lv_obj_t *cont_lbl_lang = lv_obj_create(cont);
  lv_obj_set_size(cont_lbl_lang, content_element_width, LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(cont_lbl_lang, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(cont_lbl_lang, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
  lv_obj_clear_flag(cont_lbl_lang, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_style_pad_all(cont_lbl_lang, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(cont_lbl_lang, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(cont_lbl_lang, 0, 0);

  lv_obj_t *lbl_lang = lv_label_create(cont_lbl_lang);
  lv_label_set_text(lbl_lang, strings->language_label);
  lv_obj_set_style_text_font(lbl_lang, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);

  language_dropdown = lv_dropdown_create(cont_lbl_lang);
  lv_dropdown_set_options(language_dropdown, "English\nEspañol\nDeutsch\nFrançais\nTürkçe\nSvenska\nItaliano\nУкраїнська");
  lv_dropdown_set_selected(language_dropdown, current_language);
  lv_obj_set_width(language_dropdown, 120);
  lv_obj_set_style_text_font(language_dropdown, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(language_dropdown, get_font_14(), LV_PART_SELECTED | LV_STATE_DEFAULT);
  lv_obj_t *list = lv_dropdown_get_list(language_dropdown);
  lv_obj_set_style_text_font(list, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(language_dropdown, lbl_lang, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_obj_add_event_cb(language_dropdown, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);


  // Location search button
  lv_obj_t *btn_change_loc = lv_btn_create(cont);
  lv_obj_align_to(btn_change_loc, lbl_lang, LV_ALIGN_OUT_BOTTOM_LEFT, 0, vertical_element_spacing);

  lv_obj_set_size(btn_change_loc, content_element_width, 40);
  lv_obj_add_event_cb(btn_change_loc, change_location_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *lbl_chg = lv_label_create(btn_change_loc);
  lv_label_set_text(lbl_chg, strings->location_btn);
  lv_obj_set_style_text_font(lbl_chg, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lbl_chg);

  // Hidden keyboard object
  if (!kb) {
    kb = lv_keyboard_create(lv_scr_act());
    lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_TEXT_LOWER);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_event_cb(kb, kb_event_cb, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(kb, kb_event_cb, LV_EVENT_CANCEL, NULL);
  }

  // Reset WiFi button
  lv_obj_t *btn_reset = lv_btn_create(cont);
  lv_obj_set_style_bg_color(btn_reset, lv_palette_main(LV_PALETTE_RED), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_color(btn_reset, lv_palette_darken(LV_PALETTE_RED, 1), LV_PART_MAIN | LV_STATE_PRESSED);
  lv_obj_set_style_text_color(btn_reset, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_size(btn_reset, content_element_width, 40);
  lv_obj_align_to(btn_reset, btn_change_loc, LV_ALIGN_OUT_RIGHT_MID, 12, 0);

  lv_obj_add_event_cb(btn_reset, reset_wifi_event_handler, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *lbl_reset = lv_label_create(btn_reset);
  lv_label_set_text(lbl_reset, strings->reset_wifi);
  lv_obj_set_style_text_font(lbl_reset, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lbl_reset);

  // --- Close button (positioned at the very bottom of the *window*) ---
  btn_close_obj = lv_btn_create(settings_win); // Create on the window itself
  lv_obj_set_size(btn_close_obj, SCREEN_WIDTH, 40); // Full screen width (240px), 40px height
  lv_obj_align(btn_close_obj, LV_ALIGN_BOTTOM_LEFT, 0, -10); // Align to bottom-left, 10px from bottom edge
  lv_obj_add_event_cb(btn_close_obj, settings_event_handler, LV_EVENT_CLICKED, NULL);

  lv_obj_t *lbl_btn = lv_label_create(btn_close_obj);
  lv_label_set_text(lbl_btn, strings->close);
  lv_obj_set_style_text_font(lbl_btn, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lbl_btn); // Center label within the button

}

static void settings_event_handler(lv_event_t *e) {
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *tgt = (lv_obj_t *)lv_event_get_target(e);

  if (tgt == unit_switch && code == LV_EVENT_VALUE_CHANGED) {
    use_fahrenheit = lv_obj_has_state(unit_switch, LV_STATE_CHECKED);
  }

  if (tgt == clock_24hr_switch && code == LV_EVENT_VALUE_CHANGED) {
    use_24_hour = lv_obj_has_state(clock_24hr_switch, LV_STATE_CHECKED);
  }

  if (tgt == language_dropdown && code == LV_EVENT_VALUE_CHANGED) {
    current_language = (Language)lv_dropdown_get_selected(language_dropdown);
    // Update the UI immediately to reflect language change
    lv_obj_del(settings_win);
    settings_win = nullptr;
    
    // Save preferences and recreate UI with new language
    prefs.putBool("useFahrenheit", use_fahrenheit);
    prefs.putBool("use24Hour", use_24_hour);
    prefs.putUInt("language", current_language);
    // --- Save new brightness prefs ---
    prefs.putUInt("dayBright", day_brightness);
    prefs.putUInt("nightBright", night_brightness);
    prefs.putInt("nightStart", night_start_hour);
    prefs.putInt("nightEnd", night_end_hour);

    lv_keyboard_set_textarea(kb, nullptr);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    
    // Recreate the main UI with the new language
    lv_obj_clean(lv_scr_act());
    create_ui();
    fetch_and_update_weather();
    return;
  }

  if (tgt == btn_close_obj && code == LV_EVENT_CLICKED) {
    prefs.putBool("useFahrenheit", use_fahrenheit);
    prefs.putBool("use24Hour", use_24_hour);
    prefs.putUInt("language", current_language);
    // --- Save new brightness prefs ---
    prefs.putUInt("dayBright", day_brightness);
    prefs.putUInt("nightBright", night_brightness);
    prefs.putInt("nightStart", night_start_hour);
    prefs.putInt("nightEnd", night_end_hour);

    lv_keyboard_set_textarea(kb, nullptr);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    lv_obj_del(settings_win);
    settings_win = nullptr;

    fetch_and_update_weather();
  }
}

void do_geocode_query(const char *q) {
  geoDoc.clear();
  String url = String("https://geocoding-api.open-meteo.com/v1/search?name=") + urlencode(q) + "&count=15";

  HTTPClient http;
  http.begin(url);
  if (http.GET() == HTTP_CODE_OK) {
    Serial.println("Completed location search at open-meteo: " + url);
    auto err = deserializeJson(geoDoc, http.getString());
    if (!err) {
      geoResults = geoDoc["results"].as<JsonArray>();
      populate_results_dropdown();
    } else {
        Serial.println("Failed to parse search response from open-meteo: " + url);
    }
  } else {
      Serial.println("Failed location search at open-meteo: " + url);
  }
  http.end();
}

void fetch_and_update_weather() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi no longer connected. Attempting to reconnect...");
    WiFi.disconnect();
    WiFiManager wm;  
    wm.autoConnect(DEFAULT_CAPTIVE_SSID);
    delay(1000);  
    if (WiFi.status() != WL_CONNECTED) { 
      Serial.println("WiFi connection still unavailable.");
      return;   
    }
    Serial.println("WiFi connection reestablished.");
  }


  String url = String("http://api.open-meteo.com/v1/forecast?latitude=")
               + latitude + "&longitude=" + longitude
               + "&current=temperature_2m,apparent_temperature,is_day,weather_code"
               + "&daily=temperature_2m_min,temperature_2m_max,weather_code"
               + "&hourly=temperature_2m,precipitation_probability,is_day,weather_code"
               + "&forecast_hours=21"
               + "&timezone=auto";

  HTTPClient http;
  http.begin(url);

  if (http.GET() == HTTP_CODE_OK) {
    Serial.println("Updated weather from open-meteo: " + url);

    String payload = http.getString();
    DynamicJsonDocument doc(96 * 1024);

    if (deserializeJson(doc, payload) == DeserializationError::Ok) {
      float t_now = doc["current"]["temperature_2m"].as<float>();
      float t_ap = doc["current"]["apparent_temperature"].as<float>();
      int code_now = doc["current"]["weather_code"].as<int>();
      int is_day = doc["current"]["is_day"].as<int>();

      if (use_fahrenheit) {
        t_now = t_now * 9.0 / 5.0 + 32.0;
        t_ap = t_ap * 9.0 / 5.0 + 32.0;
      }
      const LocalizedStrings* strings = get_strings(current_language);

      int utc_offset_seconds = doc["utc_offset_seconds"].as<int>();
      configTime(utc_offset_seconds, 0, "pool.ntp.org", "time.nist.gov");
      Serial.print("Updating time from NTP with UTC offset: ");
      Serial.println(utc_offset_seconds);

      char unit = use_fahrenheit ? 'F' : 'C';
      lv_label_set_text_fmt(lbl_today_temp, "%.0f°%c", t_now, unit);
      lv_label_set_text_fmt(lbl_today_feels_like, "%s %.0f°%c", strings->feels_like_temp, t_ap, unit);
      lv_img_set_src(img_today_icon, choose_image(code_now, is_day));

      JsonArray times = doc["daily"]["time"].as<JsonArray>();
      JsonArray tmin = doc["daily"]["temperature_2m_min"].as<JsonArray>();
      JsonArray tmax = doc["daily"]["temperature_2m_max"].as<JsonArray>();
      JsonArray weather_codes = doc["daily"]["weather_code"].as<JsonArray>();

      for (int i = 0; i < 7; i++) {
        const char *date = times[i];
        int year = atoi(date + 0);
        int mon = atoi(date + 5);
        int dayd = atoi(date + 8);
        int dow = day_of_week(year, mon, dayd);
        const char *dayStr = (i == 0 && current_language != LANG_FR) ? strings->today : strings->weekdays[dow];

        float mn = tmin[i].as<float>();
        float mx = tmax[i].as<float>();
        if (use_fahrenheit) {
          mn = mn * 9.0 / 5.0 + 32.0;
          mx = mx * 9.0 / 5.0 + 32.0;
        }

        lv_label_set_text_fmt(lbl_daily_day[i], "%s", dayStr);
        lv_label_set_text_fmt(lbl_daily_high[i], "%.0f°%c", mx, unit);
        lv_label_set_text_fmt(lbl_daily_low[i], "%.0f°%c", mn, unit);
        lv_img_set_src(img_daily[i], choose_icon(weather_codes[i].as<int>(), (i == 0) ? is_day : 1));
      }

      JsonArray hours = doc["hourly"]["time"].as<JsonArray>();
      JsonArray hourly_temps = doc["hourly"]["temperature_2m"].as<JsonArray>();
      JsonArray precipitation_probabilities = doc["hourly"]["precipitation_probability"].as<JsonArray>();
      JsonArray hourly_weather_codes = doc["hourly"]["weather_code"].as<JsonArray>();
      JsonArray hourly_is_day = doc["hourly"]["is_day"].as<JsonArray>();

      for (int i = 0; i < 21; i++) {
        const char *date = hours[i];  // "YYYY-MM-DD"
        int hour = atoi(date + 11);
        int minute = atoi(date + 14);
        String hour_name = hour_of_day(hour);

        float precipitation_probability = precipitation_probabilities[i].as<float>();
        float temp = hourly_temps[i].as<float>();
        if (use_fahrenheit) {
          temp = temp * 9.0 / 5.0 + 32.0;
        }

        if (i == 0 && current_language != LANG_FR) {
          lv_label_set_text(lbl_hourly[i], strings->now);
        } else {
          lv_label_set_text(lbl_hourly[i], hour_name.c_str());
        }
        lv_label_set_text_fmt(lbl_precipitation_probability[i], "%.0f%%", precipitation_probability);
        lv_label_set_text_fmt(lbl_hourly_temp[i], "%.0f°%c", temp, unit);
        lv_img_set_src(img_hourly[i], choose_icon(hourly_weather_codes[i].as<int>(), hourly_is_day[i].as<int>()));
      }


    } else {
      Serial.println("JSON parse failed on result from " + url);
    }
  } else {
    Serial.println("HTTP GET failed at " + url);
  }
  http.end();
}

const lv_img_dsc_t* choose_image(int code, int is_day) {
  switch (code) {
    // Clear sky
    case  0:
      return is_day
        ? &image_sunny
        : &image_clear_night;

    // Mainly clear
    case  1:
      return is_day
        ? &image_mostly_sunny
        : &image_mostly_clear_night;

    // Partly cloudy
    case  2:
      return is_day
        ? &image_partly_cloudy
        : &image_partly_cloudy_night;

    // Overcast
    case  3:
      return &image_cloudy;

    // Fog / mist
    case 45:
    case 48:
      return &image_haze_fog_dust_smoke;

    // Drizzle (light → dense)
    case 51:
    case 53:
    case 55:
      return &image_drizzle;

    // Freezing drizzle
    case 56:
    case 57:
      return &image_sleet_hail;

    // Rain: slight showers
    case 61:
      return is_day
        ? &image_scattered_showers_day
        : &image_scattered_showers_night;

    // Rain: moderate
    case 63:
      return &image_showers_rain;

    // Rain: heavy
    case 65:
      return &image_heavy_rain;

    // Freezing rain
    case 66:
    case 67:
      return &image_wintry_mix_rain_snow;

    // Snow fall (light, moderate, heavy) & snow showers (light)
    case 71:
    case 73:
    case 75:
    case 85:
      return &image_snow_showers_snow;

    // Snow grains
    case 77:
      return &image_flurries;

    // Rain showers (slight → moderate)
    case 80:
    case 81:
      return is_day
        ? &image_scattered_showers_day
        : &image_scattered_showers_night;

    // Rain showers: violent
    case 82:
      return &image_heavy_rain;

    // Heavy snow showers
    case 86:
      return &image_heavy_snow;

    // Thunderstorm (light)
    case 95:
      return is_day
        ? &image_isolated_scattered_tstorms_day
        : &image_isolated_scattered_tstorms_night;

    // Thunderstorm with hail
    case 96:
    case 99:
      return &image_strong_tstorms;

    // Fallback for any other code
    default:
      return is_day
        ? &image_mostly_cloudy_day
        : &image_mostly_cloudy_night;
  }
}

const lv_img_dsc_t* choose_icon(int code, int is_day) {
  switch (code) {
    // Clear sky
    case  0:
      return is_day
        ? &icon_sunny
        : &icon_clear_night;

    // Mainly clear
    case  1:
      return is_day
        ? &icon_mostly_sunny
        : &icon_mostly_clear_night;

    // Partly cloudy
    case  2:
      return is_day
        ? &icon_partly_cloudy
        : &icon_partly_cloudy_night;

    // Overcast
    case  3:
      return &icon_cloudy;

    // Fog / mist
    case 45:
    case 48:
      return &icon_haze_fog_dust_smoke;

    // Drizzle (light → dense)
    case 51:
    case 53:
    case 55:
      return &icon_drizzle;

    // Freezing drizzle
    case 56:
    case 57:
      return &icon_sleet_hail;

    // Rain: slight showers
    case 61:
      return is_day
        ? &icon_scattered_showers_day
        : &icon_scattered_showers_night;

    // Rain: moderate
    case 63:
      return &icon_showers_rain;

    // Rain: heavy
    case 65:
      return &icon_heavy_rain;

    // Freezing rain
    case 66:
    case 67:
      return &icon_wintry_mix_rain_snow;

    // Snow fall (light, moderate, heavy) & snow showers (light)
    case 71:
    case 73:
    case 75:
    case 85:
      return &icon_snow_showers_snow;

    // Snow grains
    case 77:
      return &icon_flurries;

    // Rain showers (slight → moderate)
    case 80:
    case 81:
      return is_day
        ? &icon_scattered_showers_day
        : &icon_scattered_showers_night;

    // Rain showers: violent
    case 82:
      return &icon_heavy_rain;

    // Heavy snow showers
    case 86:
      return &icon_heavy_snow;

    // Thunderstorm (light)
    case 95:
      return is_day
        ? &icon_isolated_scattered_tstorms_day
        : &icon_isolated_scattered_tstorms_night;

    // Thunderstorm with hail
    case 96:
    case 99:
      return &icon_strong_tstorms;

    // Fallback for any other code
    default:
      return is_day
        ? &icon_mostly_cloudy_day
        : &icon_mostly_cloudy_night;
  }
}
