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

// Night mode starts at 10pm and ends at 6am
#define NIGHT_MODE_START_HOUR 22
#define NIGHT_MODE_END_HOUR 6

// International Montserrat fonts with comprehensive UTF-8 support
LV_FONT_DECLARE(lv_font_montserrat_international_12);
LV_FONT_DECLARE(lv_font_montserrat_international_14);
LV_FONT_DECLARE(lv_font_montserrat_international_16);
LV_FONT_DECLARE(lv_font_montserrat_international_20);
LV_FONT_DECLARE(lv_font_montserrat_international_42);

const lv_font_t* get_font_12() {
  return &lv_font_montserrat_international_12;
}

const lv_font_t* get_font_14() {
  return &lv_font_montserrat_international_14;
}

const lv_font_t* get_font_16() {
  return &lv_font_montserrat_international_16;
}

const lv_font_t* get_font_20() {
  return &lv_font_montserrat_international_20;
}

const lv_font_t* get_font_42() {
  return &lv_font_montserrat_international_42;
}

static Language current_language = LANG_EN;

// Color schemes
enum ColorScheme { 
  COLOR_BLUE = 0,    // Default blue scheme
  COLOR_RED = 1, 
  COLOR_YELLOW = 2, 
  COLOR_ORANGE = 3, 
  COLOR_WHITE = 4, 
  COLOR_BLACK = 5, 
  COLOR_GREEN = 6,
  COLOR_PINK = 7 
};

struct ColorSchemeData {
  uint32_t bg_gradient_start;    // Main background gradient start
  uint32_t bg_gradient_end;      // Main background gradient end
  uint32_t box_background;       // Forecast box background
  uint32_t text_primary;         // Main text color (temperature)
  uint32_t text_secondary;       // Secondary text color (feels like, forecast labels)
  uint32_t text_tertiary;        // Tertiary text color (precipitation, low temps)
};

static const ColorSchemeData color_schemes[] = {
  // Blue (default) - preserve existing colors
  { 0x4c8cb9, 0xa6cdec, 0x5e9bc8, 0xFFFFFF, 0xe4ffff, 0xb9ecff },
  // Red (proper red colors)
  { 0xB22222, 0xFF6B6B, 0xDC143C, 0xFFFFFF, 0xffe4e4, 0xffcccc },
  // Yellow (proper bright yellow)
  { 0xFFD700, 0xFFF700, 0xFFE55C, 0x000000, 0x333300, 0x666600 },
  // Orange (proper orange, not brown)
  { 0xFF8C00, 0xFFA500, 0xFF7F50, 0xFFFFFF, 0xfff0e4, 0xffe0cc },
  // White
  { 0xf0f0f0, 0xffffff, 0xe0e0e0, 0x000000, 0x333333, 0x666666 },
  // Black
  { 0x1a1a1a, 0x404040, 0x2a2a2a, 0xFFFFFF, 0xe0e0e0, 0xc0c0c0 },
  // Green
  { 0x4c8b4c, 0xa6eca6, 0x5ec85e, 0xFFFFFF, 0xe4ffe4, 0xb9ffb9 },
  // Pink (the old "red" that was actually pink)
  { 0xFF69B4, 0xFFB6C1, 0xFF1493, 0xFFFFFF, 0xffe4f0, 0xffccdd }
};

static ColorScheme current_color_scheme = COLOR_BLUE;

// Color scheme helper functions
const ColorSchemeData* get_current_colors() {
  return &color_schemes[current_color_scheme];
}

void apply_background_colors(lv_obj_t* obj) {
  const ColorSchemeData* colors = get_current_colors();
  lv_obj_set_style_bg_color(obj, lv_color_hex(colors->bg_gradient_start), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_color(obj, lv_color_hex(colors->bg_gradient_end), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_grad_dir(obj, LV_GRAD_DIR_VER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
}

void apply_box_colors(lv_obj_t* obj) {
  const ColorSchemeData* colors = get_current_colors();
  lv_obj_set_style_bg_color(obj, lv_color_hex(colors->box_background), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
}

// Helper function to build localized color scheme options string
String get_localized_color_options(const LocalizedStrings* strings) {
  String options = "";
  for (int i = 0; i < 8; i++) {
    if (i > 0) options += "\n";
    options += strings->color_schemes[i];
  }
  return options;
}

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
uint32_t draw_buf[DRAW_BUF_SIZE / 4];
int x, y, z;

// Preferences
static Preferences prefs;
static bool use_fahrenheit = false;
static bool use_24_hour = false; 
static bool use_night_mode = false;
static char latitude[16] = LATITUDE_DEFAULT;
static char longitude[16] = LONGITUDE_DEFAULT;
static String location = String(LOCATION_DEFAULT);
static char dd_opts[512];
static DynamicJsonDocument geoDoc(8 * 1024);
static JsonArray geoResults;

// Screen dimming variables
static bool night_mode_active = false;
static bool temp_screen_wakeup_active = false;
static lv_timer_t *temp_screen_wakeup_timer = nullptr;

// UI components
static lv_obj_t *lbl_today_temp;
static lv_obj_t *lbl_today_feels_like;
static lv_obj_t *img_today_icon;
static lv_obj_t *lbl_forecast;
static lv_obj_t *box_daily;
static lv_obj_t *box_hourly;
static lv_obj_t *lbl_daily_day[7];
static lv_obj_t *lbl_daily_high[7];
static lv_obj_t *lbl_daily_low[7];
static lv_obj_t *img_daily[7];
static lv_obj_t *lbl_hourly[7];
static lv_obj_t *lbl_precipitation_probability[7];
static lv_obj_t *lbl_hourly_temp[7];
static lv_obj_t *img_hourly[7];
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
static lv_obj_t *night_mode_switch;
static lv_obj_t *language_dropdown;
static lv_obj_t *color_scheme_dropdown;
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
const ColorSchemeData* get_current_colors();
void apply_background_colors(lv_obj_t* obj);
void apply_box_colors(lv_obj_t* obj);
static void screen_event_cb(lv_event_t *e);
static void settings_event_handler(lv_event_t *e);
const lv_img_dsc_t *choose_image(int wmo_code, int is_day);
const lv_img_dsc_t *choose_icon(int wmo_code, int is_day);

// Screen dimming functions
bool night_mode_should_be_active();
void activate_night_mode();
void deactivate_night_mode();
void check_for_night_mode();
void handle_temp_screen_wakeup_timeout(lv_timer_t *timer);


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
      return String("0") + String(hour);
    else
      return String(hour);
  } else {
    if(hour == 0)   return String("12") + strings->am;
    if(hour == 12)  return String(strings->noon);

    bool isMorning = (hour < 12);
    String suffix = isMorning ? strings->am : strings->pm;

    int displayHour = hour % 12;

    return String(displayHour) + suffix;
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

  check_for_night_mode();

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

    // Handle touch during dimmed screen
    if (night_mode_active) {
      // Temporarily wake the screen for 15 seconds
      analogWrite(LCD_BACKLIGHT_PIN, prefs.getUInt("brightness", 128));
    
      if (temp_screen_wakeup_timer) {
        lv_timer_del(temp_screen_wakeup_timer);
      }
      temp_screen_wakeup_timer = lv_timer_create(handle_temp_screen_wakeup_timeout, 15000, NULL);
      lv_timer_set_repeat_count(temp_screen_wakeup_timer, 1); // Run only once
      Serial.println("Woke up screen. Setting timer to turn of screen after 15 seconds of inactivity.");

      if (!temp_screen_wakeup_active) {
          // If this is the wake-up tap, don't pass this touch to the UI - just undim the screen
          temp_screen_wakeup_active = true;
          data->state = LV_INDEV_STATE_RELEASED;
          return;
      }

      temp_screen_wakeup_active = true;
    }

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
  use_night_mode = prefs.getBool("useNightMode", false);
  uint32_t brightness = prefs.getUInt("brightness", 255);
  use_24_hour = prefs.getBool("use24Hour", false);
  current_language = (Language)prefs.getUInt("language", LANG_EN);
  current_color_scheme = (ColorScheme)prefs.getUInt("colorScheme", COLOR_BLUE);
  analogWrite(LCD_BACKLIGHT_PIN, brightness);

  // Check for Wi-Fi config and request it if not available
  WiFiManager wm;
  wm.setAPCallback(apModeCallback);
  wm.autoConnect(DEFAULT_CAPTIVE_SSID);

  lv_timer_create(update_clock, 1000, NULL);

  lv_obj_clean(lv_scr_act());
  create_ui();
  fetch_and_update_weather();
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

void wifi_splash_screen() {
  lv_obj_t *scr = lv_scr_act();
  lv_obj_clean(scr);
  apply_background_colors(scr);

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
  apply_background_colors(scr);

  // Trigger settings screen on touch
  lv_obj_add_event_cb(scr, screen_event_cb, LV_EVENT_CLICKED, NULL);

  img_today_icon = lv_img_create(scr);
  lv_img_set_src(img_today_icon, &image_partly_cloudy);
  lv_obj_align(img_today_icon, LV_ALIGN_TOP_MID, -64, 4);

  const ColorSchemeData* colors = get_current_colors();
  static lv_style_t default_label_style;
  lv_style_init(&default_label_style);
  lv_style_set_text_color(&default_label_style, lv_color_hex(colors->text_primary));
  lv_style_set_text_opa(&default_label_style, LV_OPA_COVER);

  const LocalizedStrings* strings = get_strings(current_language);

  lbl_today_temp = lv_label_create(scr);
  lv_label_set_text(lbl_today_temp, strings->temp_placeholder);
  lv_obj_set_style_text_font(lbl_today_temp, get_font_42(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_today_temp, LV_ALIGN_TOP_MID, 45, 25);
  lv_obj_add_style(lbl_today_temp, &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);

  lbl_today_feels_like = lv_label_create(scr);
  lv_label_set_text(lbl_today_feels_like, strings->feels_like_temp);
  lv_obj_set_style_text_font(lbl_today_feels_like, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_today_feels_like, lv_color_hex(colors->text_secondary), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_today_feels_like, LV_ALIGN_TOP_MID, 45, 75);

  lbl_forecast = lv_label_create(scr);
  lv_label_set_text(lbl_forecast, strings->seven_day_forecast);
  lv_obj_set_style_text_font(lbl_forecast, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_forecast, lv_color_hex(colors->text_secondary), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_forecast, LV_ALIGN_TOP_LEFT, 20, 110);

  box_daily = lv_obj_create(scr);
  lv_obj_set_size(box_daily, 220, 180);
  lv_obj_align(box_daily, LV_ALIGN_TOP_LEFT, 10, 135);
  apply_box_colors(box_daily);
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
    lv_obj_set_style_text_color(lbl_daily_low[i], lv_color_hex(colors->text_tertiary), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_daily_low[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_daily_low[i], LV_ALIGN_TOP_RIGHT, -50, i * 24);

    lv_img_set_src(img_daily[i], &icon_partly_cloudy);
    lv_obj_align(img_daily[i], LV_ALIGN_TOP_LEFT, 72, i * 24);
  }

  box_hourly = lv_obj_create(scr);
  lv_obj_set_size(box_hourly, 220, 180);
  lv_obj_align(box_hourly, LV_ALIGN_TOP_LEFT, 10, 135);
  apply_box_colors(box_hourly);
  lv_obj_set_style_radius(box_hourly, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_hourly, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_hourly, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_hourly, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_hourly, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_hourly, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_hourly, hourly_cb, LV_EVENT_CLICKED, NULL);

  for (int i = 0; i < 7; i++) {
    lbl_hourly[i] = lv_label_create(box_hourly);
    lbl_precipitation_probability[i] = lv_label_create(box_hourly);
    lbl_hourly_temp[i] = lv_label_create(box_hourly);
    img_hourly[i] = lv_img_create(box_hourly);

    lv_obj_add_style(lbl_hourly[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly[i], LV_ALIGN_TOP_LEFT, 2, i * 24);

    lv_obj_add_style(lbl_hourly_temp[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_hourly_temp[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_hourly_temp[i], LV_ALIGN_TOP_RIGHT, 0, i * 24);

    lv_label_set_text(lbl_precipitation_probability[i], "");
    lv_obj_set_style_text_color(lbl_precipitation_probability[i], lv_color_hex(colors->text_tertiary), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_precipitation_probability[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_precipitation_probability[i], LV_ALIGN_TOP_RIGHT, -55, i * 24);

    lv_img_set_src(img_hourly[i], &icon_partly_cloudy);
    lv_obj_align(img_hourly[i], LV_ALIGN_TOP_LEFT, 72, i * 24);
  }

  lv_obj_add_flag(box_hourly, LV_OBJ_FLAG_HIDDEN);

  // Create clock label in the top-right corner
  lbl_clock = lv_label_create(scr);
  lv_obj_set_style_text_font(lbl_clock, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_clock, lv_color_hex(colors->text_tertiary), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(lbl_clock, "");
  lv_obj_align(lbl_clock, LV_ALIGN_TOP_RIGHT, -10, 2);
}

void populate_results_dropdown() {
  dd_opts[0] = '\0';
  for (JsonObject item : geoResults) {
    strcat(dd_opts, item["name"].as<const char *>());
    if (item["admin1"]) {
      strcat(dd_opts, ", ");
      strcat(dd_opts, item["admin1"].as<const char *>());
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

  String opts;
  const char *name = obj["name"];
  const char *admin = obj["admin1"];
  const char *country = obj["country_code"];
  opts += name;
  if (admin) {
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
  lv_obj_clear_flag(box_hourly, LV_OBJ_FLAG_HIDDEN);
}

void hourly_cb(lv_event_t *e) {
  const LocalizedStrings* strings = get_strings(current_language);
  lv_obj_add_flag(box_hourly, LV_OBJ_FLAG_HIDDEN);
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
  lv_obj_set_size(location_win, 240, 320);
  lv_obj_center(location_win);

  lv_obj_t *cont = lv_win_get_content(location_win);

  lv_obj_t *lbl = lv_label_create(cont);
  lv_label_set_text(lbl, strings->city);
  lv_obj_set_style_text_font(lbl, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl, LV_ALIGN_TOP_LEFT, 5, 10);

  loc_ta = lv_textarea_create(cont);
  lv_textarea_set_one_line(loc_ta, true);
  lv_textarea_set_placeholder_text(loc_ta, strings->city_placeholder);
  lv_obj_set_width(loc_ta, 170);
  lv_obj_align_to(loc_ta, lbl, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  lv_obj_add_event_cb(loc_ta, ta_event_cb, LV_EVENT_CLICKED, kb);
  lv_obj_add_event_cb(loc_ta, ta_defocus_cb, LV_EVENT_DEFOCUSED, kb);

  lv_obj_t *lbl2 = lv_label_create(cont);
  lv_label_set_text(lbl2, strings->search_results);
  lv_obj_set_style_text_font(lbl2, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl2, LV_ALIGN_TOP_LEFT, 5, 50);

  results_dd = lv_dropdown_create(cont);
  lv_obj_set_width(results_dd, 200);
  lv_obj_align(results_dd, LV_ALIGN_TOP_LEFT, 5, 70);
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
  if (settings_win) return;

  int vertical_element_spacing = 21;

  const LocalizedStrings* strings = get_strings(current_language);
  settings_win = lv_win_create(lv_scr_act());

  lv_obj_t *header = lv_win_get_header(settings_win);
  lv_obj_set_style_height(header, 30, 0);

  lv_obj_t *title = lv_win_add_title(settings_win, strings->aura_settings);
  lv_obj_set_style_text_font(title, get_font_16(), 0);
  lv_obj_set_style_margin_left(title, 10, 0);

  lv_obj_center(settings_win);
  lv_obj_set_width(settings_win, 240);

  lv_obj_t *cont = lv_win_get_content(settings_win);
  
  // Make content scrollable
  lv_obj_set_scroll_dir(cont, LV_DIR_VER);
  lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_AUTO);

  // Brightness
  lv_obj_t *lbl_b = lv_label_create(cont);
  lv_label_set_text(lbl_b, strings->brightness);
  lv_obj_set_style_text_font(lbl_b, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_b, LV_ALIGN_TOP_LEFT, 0, 5);
  lv_obj_t *slider = lv_slider_create(cont);
  lv_slider_set_range(slider, 1, 255);
  uint32_t saved_b = prefs.getUInt("brightness", 128);
  lv_slider_set_value(slider, saved_b, LV_ANIM_OFF);
  lv_obj_set_width(slider, 100);
  lv_obj_align_to(slider, lbl_b, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

  lv_obj_add_event_cb(slider, [](lv_event_t *e){
    lv_obj_t *s = (lv_obj_t*)lv_event_get_target(e);
    uint32_t v = lv_slider_get_value(s);
    analogWrite(LCD_BACKLIGHT_PIN, v);
    prefs.putUInt("brightness", v);
  }, LV_EVENT_VALUE_CHANGED, NULL);

  // 'Night mode' switch
  lv_obj_t *lbl_night_mode = lv_label_create(cont);
  lv_label_set_text(lbl_night_mode, strings->use_night_mode);
  lv_obj_set_style_text_font(lbl_night_mode, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(lbl_night_mode, lbl_b, LV_ALIGN_OUT_BOTTOM_LEFT, 0, vertical_element_spacing);

  night_mode_switch = lv_switch_create(cont);
  lv_obj_align_to(night_mode_switch, lbl_night_mode, LV_ALIGN_OUT_RIGHT_MID, 6, 0);
  if (use_night_mode) {
    lv_obj_add_state(night_mode_switch, LV_STATE_CHECKED);
  } else {
    lv_obj_remove_state(night_mode_switch, LV_STATE_CHECKED);
  }
  lv_obj_add_event_cb(night_mode_switch, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  // 'Use F' switch
  lv_obj_t *lbl_u = lv_label_create(cont);
  lv_label_set_text(lbl_u, strings->use_fahrenheit);
  lv_obj_set_style_text_font(lbl_u, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(lbl_u, lbl_night_mode, LV_ALIGN_OUT_BOTTOM_LEFT, 0, vertical_element_spacing);

  unit_switch = lv_switch_create(cont);
  lv_obj_align_to(unit_switch, lbl_u, LV_ALIGN_OUT_RIGHT_MID, 6, 0);
  if (use_fahrenheit) {
    lv_obj_add_state(unit_switch, LV_STATE_CHECKED);
  } else {
    lv_obj_remove_state(unit_switch, LV_STATE_CHECKED);
  }
  lv_obj_add_event_cb(unit_switch, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  // 24-hr time switch
  lv_obj_t *lbl_24hr = lv_label_create(cont);
  lv_label_set_text(lbl_24hr, strings->use_24hr);
  lv_obj_set_style_text_font(lbl_24hr, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(lbl_24hr, unit_switch, LV_ALIGN_OUT_RIGHT_MID, 6, 0);

  clock_24hr_switch = lv_switch_create(cont);
  lv_obj_align_to(clock_24hr_switch, lbl_24hr, LV_ALIGN_OUT_RIGHT_MID, 6, 0);
  if (use_24_hour) {
    lv_obj_add_state(clock_24hr_switch, LV_STATE_CHECKED);
  } else {
    lv_obj_clear_state(clock_24hr_switch, LV_STATE_CHECKED);
  }
  lv_obj_add_event_cb(clock_24hr_switch, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  // Current Location label
  lv_obj_t *lbl_loc_l = lv_label_create(cont);
  lv_label_set_text(lbl_loc_l, strings->location);
  lv_obj_set_style_text_font(lbl_loc_l, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(lbl_loc_l, lbl_u, LV_ALIGN_OUT_BOTTOM_LEFT, 0, vertical_element_spacing);

  lbl_loc = lv_label_create(cont);
  lv_label_set_text(lbl_loc, location.c_str());
  lv_obj_set_style_text_font(lbl_loc, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(lbl_loc, lbl_loc_l, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  // Language selection
  lv_obj_t *lbl_lang = lv_label_create(cont);
  lv_label_set_text(lbl_lang, strings->language_label);
  lv_obj_set_style_text_font(lbl_lang, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(lbl_lang, lbl_loc_l, LV_ALIGN_OUT_BOTTOM_LEFT, 0, vertical_element_spacing);

  language_dropdown = lv_dropdown_create(cont);
  lv_dropdown_set_options(language_dropdown, "English\nEspañol\nDeutsch\nFrançais\nTürkçe\nSvenska\nItaliano");
  lv_dropdown_set_selected(language_dropdown, current_language);
  lv_obj_set_width(language_dropdown, 120);
  lv_obj_set_style_text_font(language_dropdown, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(language_dropdown, get_font_12(), LV_PART_SELECTED | LV_STATE_DEFAULT);
  lv_obj_t *list = lv_dropdown_get_list(language_dropdown);
  lv_obj_set_style_text_font(list, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(language_dropdown, lbl_lang, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_obj_add_event_cb(language_dropdown, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  // Color scheme selection
  lv_obj_t *lbl_color = lv_label_create(cont);
  lv_label_set_text(lbl_color, strings->color_scheme_label);
  lv_obj_set_style_text_font(lbl_color, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(lbl_color, lbl_lang, LV_ALIGN_OUT_BOTTOM_LEFT, 0, vertical_element_spacing);

  color_scheme_dropdown = lv_dropdown_create(cont);
  String color_options = get_localized_color_options(strings);
  lv_dropdown_set_options(color_scheme_dropdown, color_options.c_str());
  lv_dropdown_set_selected(color_scheme_dropdown, current_color_scheme);
  lv_obj_set_width(color_scheme_dropdown, 120);
  lv_obj_set_style_text_font(color_scheme_dropdown, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(color_scheme_dropdown, get_font_12(), LV_PART_SELECTED | LV_STATE_DEFAULT);
  lv_obj_t *color_list = lv_dropdown_get_list(color_scheme_dropdown);
  lv_obj_set_style_text_font(color_list, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(color_scheme_dropdown, lbl_color, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_obj_add_event_cb(color_scheme_dropdown, settings_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

  // Location search button
  lv_obj_t *btn_change_loc = lv_btn_create(cont);
  lv_obj_align_to(btn_change_loc, lbl_color, LV_ALIGN_OUT_BOTTOM_LEFT, 0, vertical_element_spacing);

  lv_obj_set_size(btn_change_loc, 100, 40);
  lv_obj_add_event_cb(btn_change_loc, change_location_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t *lbl_chg = lv_label_create(btn_change_loc);
  lv_label_set_text(lbl_chg, strings->location_btn);
  lv_obj_set_style_text_font(lbl_chg, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
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
  lv_obj_set_size(btn_reset, 100, 40);
  lv_obj_align_to(btn_reset, btn_change_loc, LV_ALIGN_OUT_RIGHT_MID, 12, 0);

  lv_obj_add_event_cb(btn_reset, reset_wifi_event_handler, LV_EVENT_CLICKED, nullptr);

  lv_obj_t *lbl_reset = lv_label_create(btn_reset);
  lv_label_set_text(lbl_reset, strings->reset_wifi);
  lv_obj_set_style_text_font(lbl_reset, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lbl_reset);

  // Add bottom padding to content so close button doesn't overlap
  lv_obj_set_style_pad_bottom(cont, 60, LV_PART_MAIN);

  // Close Settings button - make it a full-width bottom bar
  btn_close_obj = lv_btn_create(settings_win);
  lv_obj_set_size(btn_close_obj, 240, 45);
  lv_obj_align(btn_close_obj, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_add_event_cb(btn_close_obj, settings_event_handler, LV_EVENT_CLICKED, NULL);
  
  // Style the close button as a bottom bar
  lv_obj_set_style_radius(btn_close_obj, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(btn_close_obj, 1, LV_PART_MAIN);
  lv_obj_set_style_border_color(btn_close_obj, lv_color_hex(0x888888), LV_PART_MAIN);
  lv_obj_set_style_border_side(btn_close_obj, LV_BORDER_SIDE_TOP, LV_PART_MAIN);

  // Cancel button
  lv_obj_t *lbl_btn = lv_label_create(btn_close_obj);
  lv_label_set_text(lbl_btn, strings->close);
  lv_obj_set_style_text_font(lbl_btn, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lbl_btn);
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

  if (tgt == night_mode_switch && code == LV_EVENT_VALUE_CHANGED) {
    use_night_mode = lv_obj_has_state(night_mode_switch, LV_STATE_CHECKED);
  }

  if (tgt == language_dropdown && code == LV_EVENT_VALUE_CHANGED) {
    current_language = (Language)lv_dropdown_get_selected(language_dropdown);
    // Update the UI immediately to reflect language change
    lv_obj_del(settings_win);
    settings_win = nullptr;
    
    // Save preferences and recreate UI with new language
    prefs.putBool("useFahrenheit", use_fahrenheit);
    prefs.putBool("use24Hour", use_24_hour);
    prefs.putBool("useNightMode", use_night_mode);
    prefs.putUInt("language", current_language);
    prefs.putUInt("colorScheme", current_color_scheme);

    lv_keyboard_set_textarea(kb, nullptr);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    
    // Recreate the main UI with the new language
    lv_obj_clean(lv_scr_act());
    create_ui();
    fetch_and_update_weather();
    return;
  }

  if (tgt == color_scheme_dropdown && code == LV_EVENT_VALUE_CHANGED) {
    // Prevent rapid consecutive changes that could cause freezing
    static unsigned long last_color_change = 0;
    unsigned long now = millis();
    if (now - last_color_change < 1000) { // Debounce for 1 second
      return;
    }
    last_color_change = now;
    
    current_color_scheme = (ColorScheme)lv_dropdown_get_selected(color_scheme_dropdown);
    
    // Show loading spinner
    lv_obj_t *spinner = lv_spinner_create(lv_scr_act());
    lv_obj_set_size(spinner, 40, 40);
    lv_obj_center(spinner);
    
    // Force a display update to show the spinner
    lv_refr_now(lv_disp_get_default());
    
    // Small delay to ensure spinner is visible
    delay(100);
    
    // Save preferences first before UI changes
    prefs.putBool("useFahrenheit", use_fahrenheit);
    prefs.putBool("use24Hour", use_24_hour);
    prefs.putBool("useNightMode", use_night_mode);
    prefs.putUInt("language", current_language);
    prefs.putUInt("colorScheme", current_color_scheme);

    // Clean up keyboard first
    lv_keyboard_set_textarea(kb, nullptr);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    
    // Delete settings window
    if (settings_win != nullptr) {
      lv_obj_del(settings_win);
      settings_win = nullptr;
    }
    
    // Give time for cleanup before recreating UI
    delay(50);
    
    // Recreate the main UI with the new color scheme
    lv_obj_clean(lv_scr_act());
    create_ui();
    fetch_and_update_weather();
    return;
  }

  if (tgt == btn_close_obj && code == LV_EVENT_CLICKED) {
    prefs.putBool("useFahrenheit", use_fahrenheit);
    prefs.putBool("use24Hour", use_24_hour);
    prefs.putBool("useNightMode", use_night_mode);
    prefs.putUInt("language", current_language);
    prefs.putUInt("colorScheme", current_color_scheme);

    lv_keyboard_set_textarea(kb, nullptr);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    lv_obj_del(settings_win);
    settings_win = nullptr;

    fetch_and_update_weather();
  }
}

// Screen dimming functions implementation
bool night_mode_should_be_active() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return false;

  if (!use_night_mode) return false;
  
  int hour = timeinfo.tm_hour;
  return (hour >= NIGHT_MODE_START_HOUR || hour < NIGHT_MODE_END_HOUR);
}

void activate_night_mode() {
  analogWrite(LCD_BACKLIGHT_PIN, 0);
  night_mode_active = true;
}

void deactivate_night_mode() {
  analogWrite(LCD_BACKLIGHT_PIN, prefs.getUInt("brightness", 128));
  night_mode_active = false;
}

void check_for_night_mode() {
  bool night_mode_time = night_mode_should_be_active();

  if (night_mode_time && !night_mode_active && !temp_screen_wakeup_active) {
    activate_night_mode();
  } else if (!night_mode_time && night_mode_active) {
    deactivate_night_mode();
  }
}

void handle_temp_screen_wakeup_timeout(lv_timer_t *timer) {
  if (temp_screen_wakeup_active) {
    temp_screen_wakeup_active = false;

    if (night_mode_should_be_active()) {
      activate_night_mode();
    }
  }
  
  if (temp_screen_wakeup_timer) {
    lv_timer_del(temp_screen_wakeup_timer);
    temp_screen_wakeup_timer = nullptr;
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
               + "&forecast_hours=7"
               + "&timezone=auto";

  HTTPClient http;
  http.begin(url);

  if (http.GET() == HTTP_CODE_OK) {
    Serial.println("Updated weather from open-meteo: " + url);

    String payload = http.getString();
    DynamicJsonDocument doc(32 * 1024);

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

      for (int i = 0; i < 7; i++) {
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
