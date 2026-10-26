#include <Arduino.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>
#include <ArduinoJson.h>
#include <time.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <Preferences.h>
#include <LittleFS.h>
#include <PNGdec.h>
#include <new>
#include "esp_system.h"
#include "translations.h"
#include "root_ca.h"

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
#define CONFIG_PORTAL_TIMEOUT_S 300  // reboot and retry saved Wi-Fi if nobody configures the portal
#define WIFI_RECONNECT_TIMEOUT_MS 15000
#define RETRY_INTERVAL 60000UL  // after a failed fetch
#define STALE_AFTER_MS (25 * 60 * 1000UL)  // two missed updates
#define ALERT_INTERVAL 300000UL  // 5 minutes
#define NWS_USER_AGENT "Aura weather display (github.com/Kzamd2014/Aura)"

// Radar page: RainViewer smoothed radar over IEM county/state lines, ~270x220 km
#define RADAR_W 220
#define RADAR_H 180
#define RADAR_ZOOM 7
#define RADAR_INTERVAL 600000UL  // refresh while the radar page is showing
#define RADAR_PAGE 3
#define RADAR_FILE "/radar.bin"
#define RADAR_TMP "/radar.tmp"
#define BASE_FILE "/base.raw"
#define BASE_KEY_FILE "/base.key"
#define DOWNLOAD_FILE "/download.png"

enum AlertLevel { ALERT_NONE, ALERT_MINOR, ALERT_MODERATE, ALERT_SEVERE, ALERT_EXTREME };
struct RadarWindow { int32_t left, top; };  // top-left in global pixels at RADAR_ZOOM

// Night mode defaults: 10pm to 6am, backlight off
#define NIGHT_MODE_START_HOUR 22
#define NIGHT_MODE_END_HOUR 6
#define DEFAULT_BRIGHTNESS 255

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
static bool use_night_mode = false;
static uint32_t brightness_setting = DEFAULT_BRIGHTNESS;
static uint8_t night_start_hour = NIGHT_MODE_START_HOUR;
static uint8_t night_end_hour = NIGHT_MODE_END_HOUR;
static uint32_t night_brightness = 0;  // 0 = backlight off
static lv_obj_t *night_win;
static lv_obj_t *night_start_dd;
static lv_obj_t *night_end_dd;
static lv_obj_t *night_bright_slider;
static char latitude[16] = LATITUDE_DEFAULT;
static char longitude[16] = LONGITUDE_DEFAULT;
static String location = String(LOCATION_DEFAULT);
static char dd_opts[512];
static JsonDocument geoDoc;
static JsonArray geoResults;
static char ap_password[9];

// Screen dimming variables
static bool night_mode_active = false;
static bool temp_screen_wakeup_active = false;
static lv_timer_t *temp_screen_wakeup_timer = nullptr;

// UI components
static lv_obj_t *lbl_today_temp;
static lv_obj_t *lbl_today_feels_like;
static lv_obj_t *img_today_icon;
#define PAGE_COUNT 4  // 7-day, hourly, current conditions, radar
static lv_obj_t *forecast_tv;
static lv_obj_t *page_dots[PAGE_COUNT];
static lv_obj_t *radar_img;
static lv_obj_t *lbl_radar_title;
static uint32_t last_radar_ms = 0;  // 0 = refresh on next view
static lv_obj_t *lbl_updated;
static uint32_t last_weather_success_ms = 0;  // 0 = never
static uint8_t weather_fail_count = 0;

// Active National Weather Service alerts for the current location (US only)
static AlertLevel alert_level = ALERT_NONE;
static String alert_event;
static String alert_details;
static int alert_count = 0;
static lv_obj_t *alert_banner;
static lv_obj_t *lbl_alert;
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
#define NOW_ROWS 8
#define NOW_ROW_AQI 7
static lv_obj_t *box_now;
static lv_obj_t *lbl_now_name[NOW_ROWS];
static lv_obj_t *lbl_now_value[NOW_ROWS];
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
void update_freshness_label();
void fetch_alerts();
void update_alert_banner();
bool fetch_weather();
void create_settings_window();
static void screen_event_cb(lv_event_t *e);
static void alert_banner_cb(lv_event_t *e);
void next_page_cb(lv_event_t *e);
void create_night_dialog();
void update_radar();
void update_page_dots();
static void page_changed_cb(lv_event_t *e);
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
  if (m < 1 || m > 12) return 0;
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

// Shows when the weather was last fetched; red once it's stale so old data
// doesn't pass for current.
void update_freshness_label() {
  if (!lbl_updated) return;
  const LocalizedStrings *strings = get_strings(current_language);
  bool never = last_weather_success_ms == 0;
  bool stale = never || millis() - last_weather_success_ms > STALE_AFTER_MS;

  char when[12] = "--:--";
  struct tm tm;
  if (!never && getLocalTime(&tm, 0)) {
    time_t t = mktime(&tm) - (millis() - last_weather_success_ms) / 1000;
    localtime_r(&t, &tm);
    if (use_24_hour) {
      snprintf(when, sizeof(when), "%02d:%02d", tm.tm_hour, tm.tm_min);
    } else {
      int h = tm.tm_hour % 12;
      snprintf(when, sizeof(when), "%d:%02d%s", h ? h : 12, tm.tm_min, tm.tm_hour < 12 ? strings->am : strings->pm);
    }
  }

  if (never && weather_fail_count == 0) {
    lv_label_set_text(lbl_updated, "");  // first fetch still in progress
  } else {
    lv_label_set_text_fmt(lbl_updated, "%s %s", strings->updated, when);
  }
  lv_obj_set_style_text_color(lbl_updated, lv_color_hex(stale ? 0xff6b6b : 0xe4ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void update_clock(lv_timer_t *timer) {
  struct tm timeinfo;

  check_for_night_mode();
  update_freshness_label();

  if (!getLocalTime(&timeinfo, 0)) return;

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
      analogWrite(LCD_BACKLIGHT_PIN, brightness_setting);
    
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

  if (!LittleFS.begin(true)) Serial.println("LittleFS mount failed; radar disabled");
  lv_init();
  lv_tick_set_cb([]() -> uint32_t { return millis(); });

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
  brightness_setting = prefs.getUInt("brightness", DEFAULT_BRIGHTNESS);
  night_start_hour = prefs.getUChar("nightStart", NIGHT_MODE_START_HOUR);
  night_end_hour = prefs.getUChar("nightEnd", NIGHT_MODE_END_HOUR);
  night_brightness = prefs.getUInt("nightBright", 0);
  use_24_hour = prefs.getBool("use24Hour", false);
  current_language = (Language)prefs.getUInt("language", LANG_EN);
  analogWrite(LCD_BACKLIGHT_PIN, brightness_setting);

  // Check for Wi-Fi config and request it if not available
  generate_ap_password();
  WiFiManager wm;
  wm.setAPCallback(apModeCallback);
  wm.setConfigPortalTimeout(CONFIG_PORTAL_TIMEOUT_S);
  // Registered before WiFiManager's own routes, so these take precedence and
  // nobody on the open setup network can push firmware through the portal.
  wm.setWebServerCallback([&wm]() {
    wm.server->on("/update", [&wm]() { wm.server->send(403, "text/plain", "Firmware upload disabled"); });
    wm.server->on("/u", [&wm]() { wm.server->send(403, "text/plain", "Firmware upload disabled"); });
  });
  std::vector<const char *> menu = {"wifi", "info", "sep", "restart", "exit"};
  wm.setMenu(menu);
  if (!wm.autoConnect(DEFAULT_CAPTIVE_SSID, ap_password)) {
    Serial.println("Config portal timed out without Wi-Fi; rebooting to retry.");
    esp_restart();
  }

  lv_timer_create(update_clock, 1000, NULL);

  lv_obj_clean(lv_scr_act());
  create_ui();
  fetch_and_update_weather();
  fetch_alerts();
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

  if (millis() - last >= (weather_fail_count ? RETRY_INTERVAL : UPDATE_INTERVAL)) {
    fetch_and_update_weather();
    last = millis();
  }

  if (forecast_tv && current_page() == RADAR_PAGE &&
      (last_radar_ms == 0 || millis() - last_radar_ms >= RADAR_INTERVAL)) {
    update_radar();
  }

  static uint32_t last_alerts = millis();
  if (millis() - last_alerts >= ALERT_INTERVAL) {
    fetch_alerts();
    last_alerts = millis();
  }

  delay(5);
}

void generate_ap_password() {
  // No 0/o/1/l so it's easy to read off the screen
  static const char charset[] = "abcdefghijkmnpqrstuvwxyz23456789";
  for (int i = 0; i < 8; i++) {
    ap_password[i] = charset[esp_random() % (sizeof(charset) - 1)];
  }
  ap_password[8] = '\0';
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

  lv_obj_t *lbl_pw = lv_label_create(scr);
  lv_label_set_text_fmt(lbl_pw, "Password: %s", ap_password);
  lv_obj_set_style_text_font(lbl_pw, get_font_20(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_pw, LV_ALIGN_BOTTOM_MID, 0, -20);
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
  lv_obj_align(lbl_today_temp, LV_ALIGN_TOP_MID, 45, 25);
  lv_obj_add_style(lbl_today_temp, &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);

  lbl_today_feels_like = lv_label_create(scr);
  lv_label_set_text(lbl_today_feels_like, strings->feels_like_temp);
  lv_obj_set_style_text_font(lbl_today_feels_like, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_today_feels_like, lv_color_hex(0xe4ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_today_feels_like, LV_ALIGN_TOP_MID, 45, 75);

  // Swipeable forecast pages below the header; each page has its own heading
  forecast_tv = lv_tileview_create(scr);
  lv_obj_set_size(forecast_tv, SCREEN_WIDTH, SCREEN_HEIGHT - 105);
  lv_obj_align(forecast_tv, LV_ALIGN_TOP_LEFT, 0, 105);
  lv_obj_set_style_bg_opa(forecast_tv, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_scrollbar_mode(forecast_tv, LV_SCROLLBAR_MODE_OFF);
  lv_obj_add_event_cb(forecast_tv, page_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);

  const char *page_titles[PAGE_COUNT] = {
    strings->seven_day_forecast, strings->hourly_forecast, strings->current_conditions, "RADAR"
  };
  lv_obj_t *pages[PAGE_COUNT];
  for (int i = 0; i < PAGE_COUNT; i++) {
    pages[i] = lv_tileview_add_tile(forecast_tv, i, 0, (lv_dir_t)((i > 0 ? LV_DIR_LEFT : 0) | (i < PAGE_COUNT - 1 ? LV_DIR_RIGHT : 0)));
    lv_obj_set_style_bg_opa(pages[i], LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_t *title = lv_label_create(pages[i]);
    lv_label_set_text(title, page_titles[i]);
    lv_obj_set_style_text_font(title, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(title, lv_color_hex(0xe4ffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 20, 5);
    if (i == RADAR_PAGE) lbl_radar_title = title;

    // Page indicator, fixed on the screen at the right of the heading row
    page_dots[i] = lv_obj_create(scr);
    lv_obj_set_size(page_dots[i], 6, 6);
    lv_obj_set_style_radius(page_dots[i], LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_border_width(page_dots[i], 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(page_dots[i], lv_color_hex(0xe4ffff), LV_PART_MAIN);
    lv_obj_clear_flag(page_dots[i], LV_OBJ_FLAG_CLICKABLE);
    lv_obj_align(page_dots[i], LV_ALIGN_TOP_RIGHT, -12 - (PAGE_COUNT - 1 - i) * 9, 114);
  }
  update_page_dots();

  lbl_updated = lv_label_create(scr);
  lv_label_set_text(lbl_updated, "");
  lv_obj_set_style_text_font(lbl_updated, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_updated, LV_ALIGN_TOP_RIGHT, -10, 95);
  update_freshness_label();

  box_daily = lv_obj_create(pages[0]);
  lv_obj_set_size(box_daily, 220, 180);
  lv_obj_align(box_daily, LV_ALIGN_TOP_LEFT, 10, 30);
  lv_obj_set_style_bg_color(box_daily, lv_color_hex(0x5e9bc8), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(box_daily, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(box_daily, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_daily, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_daily, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_daily, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_daily, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_daily, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_daily, next_page_cb, LV_EVENT_CLICKED, NULL);

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
    lv_obj_align(img_daily[i], LV_ALIGN_TOP_LEFT, 72, i * 24);
  }

  box_hourly = lv_obj_create(pages[1]);
  lv_obj_set_size(box_hourly, 220, 180);
  lv_obj_align(box_hourly, LV_ALIGN_TOP_LEFT, 10, 30);
  lv_obj_set_style_bg_color(box_hourly, lv_color_hex(0x5e9bc8), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(box_hourly, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(box_hourly, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_hourly, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_hourly, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_hourly, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_hourly, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_hourly, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_hourly, next_page_cb, LV_EVENT_CLICKED, NULL);

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
    lv_obj_set_style_text_color(lbl_precipitation_probability[i], lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_precipitation_probability[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_precipitation_probability[i], LV_ALIGN_TOP_RIGHT, -55, i * 24);

    lv_img_set_src(img_hourly[i], &icon_partly_cloudy);
    lv_obj_align(img_hourly[i], LV_ALIGN_TOP_LEFT, 72, i * 24);
  }


  box_now = lv_obj_create(pages[2]);
  lv_obj_set_size(box_now, 220, 180);
  lv_obj_align(box_now, LV_ALIGN_TOP_LEFT, 10, 30);
  lv_obj_set_style_bg_color(box_now, lv_color_hex(0x5e9bc8), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_bg_opa(box_now, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_radius(box_now, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_border_width(box_now, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(box_now, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_scrollbar_mode(box_now, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_pad_all(box_now, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_gap(box_now, 0, LV_PART_MAIN);
  lv_obj_add_event_cb(box_now, next_page_cb, LV_EVENT_CLICKED, NULL);

  // Radar: drawn by LVGL straight from LittleFS, so no frame buffer in RAM
  radar_img = lv_image_create(pages[RADAR_PAGE]);
  lv_obj_set_size(radar_img, RADAR_W, RADAR_H);
  lv_obj_align(radar_img, LV_ALIGN_TOP_LEFT, 10, 30);
  lv_obj_set_style_bg_color(radar_img, lv_color_hex(0x262626), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(radar_img, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_add_flag(radar_img, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_add_event_cb(radar_img, next_page_cb, LV_EVENT_CLICKED, NULL);
  if (LittleFS.exists(RADAR_FILE)) lv_image_set_src(radar_img, "L:" RADAR_FILE);

  lv_obj_t *marker = lv_obj_create(radar_img);
  lv_obj_set_size(marker, 8, 8);
  lv_obj_set_style_radius(marker, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_bg_color(marker, lv_color_white(), LV_PART_MAIN);
  lv_obj_set_style_border_color(marker, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_border_width(marker, 1, LV_PART_MAIN);
  lv_obj_clear_flag(marker, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_center(marker);

  // RainViewer's free API asks for attribution
  lv_obj_t *credit = lv_label_create(radar_img);
  lv_label_set_text(credit, "RainViewer");
  lv_obj_set_style_text_font(credit, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(credit, lv_color_hex(0xb0b0b0), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(credit, LV_ALIGN_BOTTOM_RIGHT, -3, -1);

  const char *now_names[NOW_ROWS] = {
    strings->wind, strings->gusts, strings->humidity, strings->uv_index,
    strings->precip_today, strings->sunrise, strings->sunset, strings->air_quality
  };
  for (int i = 0; i < NOW_ROWS; i++) {
    lbl_now_name[i] = lv_label_create(box_now);
    lv_label_set_text(lbl_now_name[i], now_names[i]);
    lv_obj_set_style_text_color(lbl_now_name[i], lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_now_name[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_now_name[i], LV_ALIGN_TOP_LEFT, 2, i * 20);

    lbl_now_value[i] = lv_label_create(box_now);
    lv_label_set_text(lbl_now_value[i], "--");
    lv_obj_add_style(lbl_now_value[i], &default_label_style, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(lbl_now_value[i], get_font_16(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(lbl_now_value[i], LV_ALIGN_TOP_RIGHT, 0, i * 20);
  }


  // Create clock label in the top-right corner
  lbl_clock = lv_label_create(scr);
  lv_obj_set_style_text_font(lbl_clock, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_color(lbl_clock, lv_color_hex(0xb9ecff), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_label_set_text(lbl_clock, "");
  lv_obj_align(lbl_clock, LV_ALIGN_TOP_RIGHT, -10, 2);

  // Alert banner across the top; covers the clock while an alert is active
  alert_banner = lv_obj_create(scr);
  lv_obj_set_size(alert_banner, SCREEN_WIDTH, 24);
  lv_obj_align(alert_banner, LV_ALIGN_TOP_MID, 0, 0);
  lv_obj_set_style_radius(alert_banner, 0, LV_PART_MAIN);
  lv_obj_set_style_border_width(alert_banner, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(alert_banner, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(alert_banner, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_clear_flag(alert_banner, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_add_event_cb(alert_banner, alert_banner_cb, LV_EVENT_CLICKED, NULL);

  lbl_alert = lv_label_create(alert_banner);
  lv_obj_set_width(lbl_alert, SCREEN_WIDTH - 12);
  lv_label_set_long_mode(lbl_alert, LV_LABEL_LONG_DOT);
  lv_obj_set_style_text_font(lbl_alert, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_align(lbl_alert, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(lbl_alert);

  update_alert_banner();
}

void update_alert_banner() {
  if (!alert_banner) return;
  if (alert_level == ALERT_NONE) {
    lv_obj_add_flag(alert_banner, LV_OBJ_FLAG_HIDDEN);
    return;
  }

  uint32_t bg = 0xd32f2f;  // severe/extreme: red
  uint32_t fg = 0xffffff;
  if (alert_level == ALERT_MODERATE) bg = 0xef6c00;  // orange
  if (alert_level == ALERT_MINOR) { bg = 0xfdd835; fg = 0x202020; }  // yellow
  lv_obj_set_style_bg_color(alert_banner, lv_color_hex(bg), LV_PART_MAIN);
  lv_obj_set_style_text_color(lbl_alert, lv_color_hex(fg), LV_PART_MAIN);

  if (alert_count > 1) {
    lv_label_set_text_fmt(lbl_alert, "%s (+%d)", alert_event.c_str(), alert_count - 1);
  } else {
    lv_label_set_text(lbl_alert, alert_event.c_str());
  }
  lv_obj_clear_flag(alert_banner, LV_OBJ_FLAG_HIDDEN);
}

static void alert_banner_cb(lv_event_t *e) {
  const LocalizedStrings *strings = get_strings(current_language);
  lv_obj_t *mbox = lv_msgbox_create(lv_scr_act());
  lv_obj_t *title = lv_msgbox_add_title(mbox, alert_event.c_str());
  lv_obj_set_style_margin_left(title, 10, 0);
  lv_obj_set_style_text_font(title, get_font_14(), 0);
  lv_obj_t *text = lv_msgbox_add_text(mbox, alert_details.c_str());
  lv_obj_set_style_text_font(text, get_font_12(), 0);
  lv_msgbox_add_close_button(mbox);

  lv_obj_set_width(mbox, 230);
  lv_obj_set_style_max_height(mbox, 300, 0);
  lv_obj_center(mbox);
  lv_obj_set_style_border_width(mbox, 2, LV_PART_MAIN);
  lv_obj_set_style_border_color(mbox, lv_color_black(), LV_PART_MAIN);
  lv_obj_set_style_radius(mbox, 4, LV_PART_MAIN);
}

void populate_results_dropdown() {
  dd_opts[0] = '\0';
  for (JsonObject item : geoResults) {
    const char *name = item["name"];
    if (!name) name = "?";
    strlcat(dd_opts, name, sizeof(dd_opts));
    if (item["admin1"]) {
      strlcat(dd_opts, ", ", sizeof(dd_opts));
      strlcat(dd_opts, item["admin1"].as<const char *>(), sizeof(dd_opts));
    }

    strlcat(dd_opts, "\n", sizeof(dd_opts));
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
  if (pres->isNull() || idx >= pres->size()) return;

  JsonObject obj = (*pres)[idx];
  if (!obj["latitude"].is<double>() || !obj["longitude"].is<double>()) return;
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
  fetch_alerts();
  last_radar_ms = 0;

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

static int current_page() {
  lv_obj_t *tile = lv_tileview_get_tile_active(forecast_tv);
  return tile ? lv_obj_get_x(tile) / lv_obj_get_width(forecast_tv) : 0;
}

void update_page_dots() {
  int active = current_page();
  for (int i = 0; i < PAGE_COUNT; i++) {
    lv_obj_set_style_bg_opa(page_dots[i], i == active ? LV_OPA_COVER : LV_OPA_40, LV_PART_MAIN);
  }
}

static void page_changed_cb(lv_event_t *e) {
  update_page_dots();
}

// Tapping a forecast box still advances to the next page, wrapping around
void next_page_cb(lv_event_t *e) {
  int next = (current_page() + 1) % PAGE_COUNT;
  lv_tileview_set_tile_by_index(forecast_tv, next, 0, LV_ANIM_ON);
  update_page_dots();
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

static lv_obj_t *add_hour_dropdown(lv_obj_t *cont, lv_obj_t *label, uint8_t selected) {
  String opts;
  for (int h = 0; h < 24; h++) {
    if (h) opts += "\n";
    opts += hour_of_day(h);
  }
  lv_obj_t *dd = lv_dropdown_create(cont);
  lv_dropdown_set_options(dd, opts.c_str());
  lv_dropdown_set_selected(dd, selected);
  lv_obj_set_width(dd, 100);
  lv_obj_set_style_text_font(dd, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_set_style_text_font(lv_dropdown_get_list(dd), get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align_to(dd, label, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  return dd;
}

// Night mode hours and how bright the screen stays during them
void create_night_dialog() {
  if (night_win) return;
  const LocalizedStrings *strings = get_strings(current_language);
  night_win = lv_win_create(lv_scr_act());
  lv_obj_t *title = lv_win_add_title(night_win, strings->use_night_mode);
  lv_obj_set_style_height(lv_win_get_header(night_win), 30, 0);
  lv_obj_set_style_text_font(title, get_font_16(), 0);
  lv_obj_set_style_margin_left(title, 10, 0);
  lv_obj_set_size(night_win, 240, 320);
  lv_obj_center(night_win);

  lv_obj_t *cont = lv_win_get_content(night_win);

  lv_obj_t *lbl_start = lv_label_create(cont);
  lv_label_set_text(lbl_start, strings->night_start);
  lv_obj_set_style_text_font(lbl_start, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_start, LV_ALIGN_TOP_LEFT, 5, 15);
  night_start_dd = add_hour_dropdown(cont, lbl_start, night_start_hour);

  lv_obj_t *lbl_end = lv_label_create(cont);
  lv_label_set_text(lbl_end, strings->night_end);
  lv_obj_set_style_text_font(lbl_end, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_end, LV_ALIGN_TOP_LEFT, 5, 65);
  night_end_dd = add_hour_dropdown(cont, lbl_end, night_end_hour);
  // Line the two dropdowns up whichever label is longer
  lv_obj_align(night_start_dd, LV_ALIGN_TOP_LEFT, 90, 5);
  lv_obj_align(night_end_dd, LV_ALIGN_TOP_LEFT, 90, 55);

  lv_obj_t *lbl_b = lv_label_create(cont);
  lv_label_set_text(lbl_b, strings->brightness);
  lv_obj_set_style_text_font(lbl_b, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_b, LV_ALIGN_TOP_LEFT, 5, 115);

  // Leftmost position turns the backlight off, as before
  night_bright_slider = lv_slider_create(cont);
  lv_slider_set_range(night_bright_slider, 0, 128);
  lv_slider_set_value(night_bright_slider, night_brightness, LV_ANIM_OFF);
  lv_obj_set_width(night_bright_slider, 180);
  lv_obj_align(night_bright_slider, LV_ALIGN_TOP_LEFT, 10, 145);

  lv_obj_t *btn_close = lv_btn_create(cont);
  lv_obj_set_size(btn_close, 80, 40);
  lv_obj_align(btn_close, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  lv_obj_add_event_cb(btn_close, [](lv_event_t *e) {
    night_start_hour = lv_dropdown_get_selected(night_start_dd);
    night_end_hour = lv_dropdown_get_selected(night_end_dd);
    night_brightness = lv_slider_get_value(night_bright_slider);
    prefs.putUChar("nightStart", night_start_hour);
    prefs.putUChar("nightEnd", night_end_hour);
    prefs.putUInt("nightBright", night_brightness);
    if (night_mode_active) activate_night_mode();  // apply a new night level immediately
    lv_obj_del(night_win);
    night_win = nullptr;
  }, LV_EVENT_CLICKED, NULL);
  lv_obj_t *lbl_close = lv_label_create(btn_close);
  lv_label_set_text(lbl_close, strings->close);
  lv_obj_set_style_text_font(lbl_close, get_font_14(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lbl_close);
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

  // Brightness
  lv_obj_t *lbl_b = lv_label_create(cont);
  lv_label_set_text(lbl_b, strings->brightness);
  lv_obj_set_style_text_font(lbl_b, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_align(lbl_b, LV_ALIGN_TOP_LEFT, 0, 5);
  lv_obj_t *slider = lv_slider_create(cont);
  lv_slider_set_range(slider, 1, 255);
  lv_slider_set_value(slider, brightness_setting, LV_ANIM_OFF);
  lv_obj_set_width(slider, 100);
  lv_obj_align_to(slider, lbl_b, LV_ALIGN_OUT_RIGHT_MID, 10, 0);

  lv_obj_add_event_cb(slider, [](lv_event_t *e){
    lv_obj_t *s = (lv_obj_t*)lv_event_get_target(e);
    brightness_setting = lv_slider_get_value(s);
    analogWrite(LCD_BACKLIGHT_PIN, brightness_setting);
  }, LV_EVENT_VALUE_CHANGED, NULL);
  // Save once when released rather than on every step of the drag
  lv_obj_add_event_cb(slider, [](lv_event_t *e){
    prefs.putUInt("brightness", brightness_setting);
  }, LV_EVENT_RELEASED, NULL);

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

  lv_obj_t *btn_night = lv_btn_create(cont);
  lv_obj_set_size(btn_night, 30, 25);
  lv_obj_align_to(btn_night, night_mode_switch, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
  lv_obj_add_event_cb(btn_night, [](lv_event_t *e) { create_night_dialog(); }, LV_EVENT_CLICKED, NULL);
  lv_obj_t *lbl_gear = lv_label_create(btn_night);
  lv_label_set_text(lbl_gear, LV_SYMBOL_SETTINGS);
  lv_obj_set_style_text_font(lbl_gear, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_center(lbl_gear);

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

  // Location search button
  lv_obj_t *btn_change_loc = lv_btn_create(cont);
  lv_obj_align_to(btn_change_loc, lbl_lang, LV_ALIGN_OUT_BOTTOM_LEFT, 0, vertical_element_spacing);

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

  // Close Settings button
  btn_close_obj = lv_btn_create(cont);
  lv_obj_set_size(btn_close_obj, 80, 40);
  lv_obj_align(btn_close_obj, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  lv_obj_add_event_cb(btn_close_obj, settings_event_handler, LV_EVENT_CLICKED, NULL);

  // Cancel button
  lv_obj_t *lbl_btn = lv_label_create(btn_close_obj);
  lv_label_set_text(lbl_btn, strings->close);
  lv_obj_set_style_text_font(lbl_btn, get_font_12(), LV_PART_MAIN | LV_STATE_DEFAULT);
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

    lv_keyboard_set_textarea(kb, nullptr);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    
    // Recreate the main UI with the new language
    lv_obj_clean(lv_scr_act());
    kb = nullptr;  // was a child of the screen, so lv_obj_clean deleted it
    night_win = nullptr;
    create_ui();
    fetch_and_update_weather();
    return;
  }

  if (tgt == btn_close_obj && code == LV_EVENT_CLICKED) {
    prefs.putBool("useFahrenheit", use_fahrenheit);
    prefs.putBool("use24Hour", use_24_hour);
    prefs.putBool("useNightMode", use_night_mode);
    prefs.putUInt("language", current_language);

    lv_keyboard_set_textarea(kb, nullptr);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

    lv_obj_del(settings_win);
    settings_win = nullptr;

    fetch_and_update_weather();
  }
}

// Screen dimming functions implementation
bool night_mode_should_be_active() {
  if (!use_night_mode) return false;
  if (alert_level >= ALERT_SEVERE) return false;  // keep the screen lit for warnings

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 0)) return false;
  
  int hour = timeinfo.tm_hour;
  if (night_start_hour == night_end_hour) return false;
  if (night_start_hour < night_end_hour) {
    return hour >= night_start_hour && hour < night_end_hour;
  }
  return hour >= night_start_hour || hour < night_end_hour;  // wraps past midnight
}

void activate_night_mode() {
  analogWrite(LCD_BACKLIGHT_PIN, night_brightness);
  night_mode_active = true;
}

void deactivate_night_mode() {
  analogWrite(LCD_BACKLIGHT_PIN, brightness_setting);
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

void log_heap(const char *where) {
  Serial.printf("[heap] %s: free=%u largest_block=%u min_ever=%u\n", where,
                ESP.getFreeHeap(), ESP.getMaxAllocHeap(), ESP.getMinFreeHeap());
}

// Reconnects using the saved credentials. Never opens the config portal: that
// would block the UI indefinitely and expose an access point on every router blip.
bool ensure_wifi() {
  if (WiFi.status() == WL_CONNECTED) return true;

  Serial.println("WiFi no longer connected. Attempting to reconnect...");
  WiFi.reconnect();
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WIFI_RECONNECT_TIMEOUT_MS) {
    lv_timer_handler();
    delay(50);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection still unavailable.");
    return false;
  }
  Serial.println("WiFi connection reestablished.");
  return true;
}

void do_geocode_query(const char *q) {
  // Drop any previous results first so Save can't act on a stale or freed array
  geoResults = JsonArray();
  geoDoc.clear();
  lv_dropdown_set_options(results_dd, "");
  lv_obj_clear_flag(results_dd, LV_OBJ_FLAG_CLICKABLE);
  lv_obj_set_style_bg_color(btn_close_loc, lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN | LV_STATE_DEFAULT);
  lv_obj_clear_flag(btn_close_loc, LV_OBJ_FLAG_CLICKABLE);

  if (!ensure_wifi()) return;

  String url = String("https://geocoding-api.open-meteo.com/v1/search?name=") + urlencode(q) + "&count=10";

  JsonDocument filter;
  JsonObject f = filter["results"].add<JsonObject>();
  f["name"] = true;
  f["admin1"] = true;
  f["country_code"] = true;
  f["latitude"] = true;
  f["longitude"] = true;

  log_heap("before geocode");
  NetworkClientSecure client;
  client.setCACert(ISRG_ROOT_CAS);
  HTTPClient http;
  http.useHTTP10(true);  // no chunked encoding, so the stream can be parsed directly
  http.begin(client, url);
  if (http.GET() == HTTP_CODE_OK) {
    Serial.println("Completed location search at open-meteo: " + url);
    auto err = deserializeJson(geoDoc, http.getStream(), DeserializationOption::Filter(filter));
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

// "2026-10-21T07:34" -> local clock text in the user's 12/24h format
String format_iso_time(const char *iso, const LocalizedStrings *strings) {
  if (!iso || strlen(iso) < 16) return "--";
  int h = atoi(iso + 11), m = atoi(iso + 14);
  char buf[12];
  if (use_24_hour) {
    snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
  } else {
    snprintf(buf, sizeof(buf), "%d:%02d%s", h % 12 ? h % 12 : 12, m, h < 12 ? strings->am : strings->pm);
  }
  return String(buf);
}

void update_now_values(JsonDocument &doc, const LocalizedStrings *strings) {
  JsonObject cur = doc["current"];
  JsonObject daily = doc["daily"];
  const char *speed = use_fahrenheit ? "mph" : "km/h";

  lv_label_set_text_fmt(lbl_now_value[0], "%.0f %s", cur["wind_speed_10m"] | 0.0f, speed);
  lv_label_set_text_fmt(lbl_now_value[1], "%.0f %s", cur["wind_gusts_10m"] | 0.0f, speed);
  lv_label_set_text_fmt(lbl_now_value[2], "%d%%", cur["relative_humidity_2m"] | 0);
  lv_label_set_text_fmt(lbl_now_value[3], "%.0f", cur["uv_index"] | 0.0f);
  if (use_fahrenheit) {
    lv_label_set_text_fmt(lbl_now_value[4], "%.2f in", daily["precipitation_sum"][0] | 0.0f);
  } else {
    lv_label_set_text_fmt(lbl_now_value[4], "%.1f mm", daily["precipitation_sum"][0] | 0.0f);
  }
  lv_label_set_text(lbl_now_value[5], format_iso_time(daily["sunrise"][0], strings).c_str());
  lv_label_set_text(lbl_now_value[6], format_iso_time(daily["sunset"][0], strings).c_str());
}

// ---- Radar ----

// The RADAR_W x RADAR_H window of the Web Mercator world map centred on the location
static RadarWindow radar_window() {
  double world = 256.0 * (1 << RADAR_ZOOM);
  double lat = atof(latitude), lon = atof(longitude);
  double px = (lon + 180.0) / 360.0 * world;
  double s = sin(lat * M_PI / 180.0);
  double py = (0.5 - log((1 + s) / (1 - s)) / (4 * M_PI)) * world;
  return { (int32_t)px - RADAR_W / 2, (int32_t)py - RADAR_H / 2 };
}

// Uses the caller's client so a run of tiles from one host shares a single
// keep-alive connection: each TLS handshake costs ~2.5 s on the ESP32.
static bool download_to_file(HTTPClient &http, NetworkClientSecure &client, const char *url, const char *path) {
  http.begin(client, url);
  int code = http.GET();
  bool ok = false;
  if (code == HTTP_CODE_OK) {
    File f = LittleFS.open(path, FILE_WRITE);
    if (f) {
      int written = http.writeToStream(&f);
      ok = written > 0;
      if (!ok) Serial.printf("Download write failed (%d): %s\n", written, url);
      f.close();
    }
  } else {
    Serial.printf("Download failed (%d): %s\n", code, url);
  }
  http.end();
  return ok;
}

static File png_file;
static void *png_open(const char *name, int32_t *size) {
  png_file = LittleFS.open(name, FILE_READ);
  if (!png_file) return nullptr;
  *size = png_file.size();
  return &png_file;
}
static void png_close(void *handle) { png_file.close(); }
static int32_t png_read(PNGFILE *pf, uint8_t *buf, int32_t len) { return ((File *)pf->fHandle)->read(buf, len); }
static int32_t png_seek(PNGFILE *pf, int32_t pos) { return ((File *)pf->fHandle)->seek(pos) ? pos : -1; }

// Decodes the in-window rectangle of one 256x256 map tile into a piece file,
// one row after another: 1 byte/pixel (alpha only) for base map line layers,
// or 4 bytes/pixel (RGBA) for radar. Writes are strictly sequential:
// LittleFS rewrites a whole block for every write into the middle of a file,
// which made stitching in place take over 70 s.
struct PieceCtx {
  File *out;
  bool rgba;
  int32_t row0, row1, col0, col1;  // in-window part of the tile
  uint8_t buf[256 * 4];
};

static void png_pixel(PNGDRAW *d, int x, uint8_t *rgba) {
  const uint8_t *p = d->pPixels;
  switch (d->iPixelType) {
    case PNG_PIXEL_TRUECOLOR_ALPHA:
      memcpy(rgba, &p[x * 4], 4);
      return;
    case PNG_PIXEL_TRUECOLOR:
      memcpy(rgba, &p[x * 3], 3);
      rgba[3] = 255;
      return;
    case PNG_PIXEL_INDEXED: {
      int per_byte = 8 / d->iBpp;
      uint8_t idx = (p[x / per_byte] >> ((per_byte - 1 - x % per_byte) * d->iBpp)) & ((1 << d->iBpp) - 1);
      memcpy(rgba, &d->pPalette[idx * 3], 3);
      rgba[3] = d->iHasAlpha ? d->pPalette[768 + idx] : 255;
      return;
    }
    default:  // grayscale tiles aren't used by these services
      memset(rgba, 0, 4);
  }
}

static int piece_draw(PNGDRAW *d) {
  PieceCtx *c = (PieceCtx *)d->pUser;
  if (d->y < c->row0 || d->y >= c->row1) return 1;
  if (d->iBpp != 8 && d->iPixelType != PNG_PIXEL_INDEXED) return 0;  // 16-bit channels unsupported
  uint8_t px[4];
  int n = 0;
  for (int x = c->col0; x < c->col1; x++) {
    png_pixel(d, x, px);
    if (c->rgba) {
      memcpy(&c->buf[n], px, 4);
      n += 4;
    } else {
      c->buf[n++] = px[3];
    }
  }
  c->out->write(c->buf, n);
  return 1;
}

static void piece_path(char *buf, size_t len, const char *kind, int tx, int ty) {
  snprintf(buf, len, "/%s%d%d.raw", kind, tx, ty);
}

// Downloads each tile covering the window from url_fmt (printf with z, x, y)
// and decodes it into a piece file. Indices in piece names are relative to the
// window's first tile, so there are at most 2x2.
// keep_alive reuses one TLS connection for all tiles. Otherwise requests go
// out as HTTP/1.0: IEM answers HTTP/1.1 with chunked bodies, which
// HTTPClient's chunked reader intermittently fails on over TLS.
static bool fetch_pieces(RadarWindow win, const char *url_fmt, const char *ca, const char *kind, bool rgba, bool keep_alive) {
  PNG *png = new (std::nothrow) PNG();
  PieceCtx *ctx = new (std::nothrow) PieceCtx();
  bool ok = png && ctx;
  NetworkClientSecure client;
  client.setCACert(ca);
  HTTPClient http;
  http.setReuse(keep_alive);
  http.useHTTP10(!keep_alive);
  http.setTimeout(15000);
  int32_t tx0 = win.left / 256, ty0 = win.top / 256;
  for (int32_t ty = ty0; ok && ty <= (win.top + RADAR_H - 1) / 256; ty++) {
    for (int32_t tx = tx0; ok && tx <= (win.left + RADAR_W - 1) / 256; tx++) {
      char url[200], path[24];
      snprintf(url, sizeof(url), url_fmt, RADAR_ZOOM, (long)tx, (long)ty);
      piece_path(path, sizeof(path), kind, tx - tx0, ty - ty0);
      uint32_t t0 = millis();
      ok = download_to_file(http, client, url, DOWNLOAD_FILE) ||
           download_to_file(http, client, url, DOWNLOAD_FILE);  // one retry
      uint32_t t1 = millis();
      ok = ok && png->open(DOWNLOAD_FILE, png_open, png_close, png_read, png_seek, piece_draw) == PNG_SUCCESS;
      if (!ok) break;
      File out = LittleFS.open(path, FILE_WRITE);
      ctx->out = &out;
      ctx->rgba = rgba;
      ctx->row0 = max(0L, (long)(win.top - ty * 256));
      ctx->row1 = min(256L, (long)(win.top + RADAR_H - ty * 256));
      ctx->col0 = max(0L, (long)(win.left - tx * 256));
      ctx->col1 = min(256L, (long)(win.left + RADAR_W - tx * 256));
      ok = out && png->decode(ctx, 0) == PNG_SUCCESS;
      png->close();
      out.close();
      Serial.printf("  %s tile: download %lu ms, decode %lu ms\n", kind, t1 - t0, millis() - t1);
    }
  }
  delete ctx;
  delete png;
  if (!ok) Serial.printf("Fetching %s tiles failed\n", kind);
  return ok;
}

// Reads pieces of one kind back as whole window rows, in order
struct PieceReader {
  RadarWindow win;
  const char *kind;
  int bpp;
  File files[2];
  int32_t open_ty = -1;

  bool read_row(int32_t r, uint8_t *row) {
    int32_t tx0 = win.left / 256, tx1 = (win.left + RADAR_W - 1) / 256;
    int32_t ty = (win.top + r) / 256;
    if (ty != open_ty) {  // moved down into the next row of tiles
      for (int32_t tx = tx0; tx <= tx1; tx++) {
        char path[24];
        piece_path(path, sizeof(path), kind, tx - tx0, ty - win.top / 256);
        if (files[tx - tx0]) files[tx - tx0].close();
        files[tx - tx0] = LittleFS.open(path, FILE_READ);
        if (!files[tx - tx0]) return false;
      }
      open_ty = ty;
    }
    int32_t x = 0;
    for (int32_t tx = tx0; tx <= tx1; tx++) {
      int32_t w = min(win.left + RADAR_W, (tx + 1) * 256) - max(win.left, tx * 256);
      if (files[tx - tx0].read(&row[x * bpp], w * bpp) != (size_t)(w * bpp)) return false;
      x += w;
    }
    return true;
  }

  void close() {
    for (File &f : files) if (f) f.close();
  }
};

static void remove_pieces(const char *kind) {
  for (int ty = 0; ty < 2; ty++) {
    for (int tx = 0; tx < 2; tx++) {
      char path[24];
      piece_path(path, sizeof(path), kind, tx, ty);
      LittleFS.remove(path);
    }
  }
}

static inline uint16_t rgb565(int r, int g, int b) {
  return ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
}

static inline uint16_t blend565(uint16_t bg, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  int br = (bg >> 8) & 0xf8, bgg = (bg >> 3) & 0xfc, bb = (bg << 3) & 0xf8;
  return rgb565(br + (r - br) * a / 255, bgg + (g - bgg) * a / 255, bb + (b - bb) * a / 255);
}

// Builds the window's base map: county and state lines from IEM (US public
// data, no API key) drawn over a dark background. Only needed per location.
static bool build_base_map(RadarWindow win) {
  uint32_t start = millis();
  static const char *IEM_TILES = "https://mesonet.agron.iastate.edu/c/tile.py/1.0.0/%s/%%d/%%ld/%%ld.png";
  char counties_fmt[96], states_fmt[96];
  snprintf(counties_fmt, sizeof(counties_fmt), IEM_TILES, "uscounties");
  snprintf(states_fmt, sizeof(states_fmt), IEM_TILES, "usstates");
  bool ok = fetch_pieces(win, counties_fmt, USERTRUST_ECC_ROOT_CA, "cty", false, false) &&
            fetch_pieces(win, states_fmt, USERTRUST_ECC_ROOT_CA, "st", false, false);

  if (ok) {
    PieceReader counties{win, "cty", 1}, states{win, "st", 1};
    uint8_t *ca = new (std::nothrow) uint8_t[RADAR_W];
    uint8_t *sa = new (std::nothrow) uint8_t[RADAR_W];
    uint16_t *row = new (std::nothrow) uint16_t[RADAR_W];
    File out = LittleFS.open(BASE_FILE, FILE_WRITE);
    ok = ca && sa && row && out;
    for (int32_t r = 0; ok && r < RADAR_H; r++) {
      ok = counties.read_row(r, ca) && states.read_row(r, sa);
      for (int x = 0; ok && x < RADAR_W; x++) {
        uint16_t px = rgb565(0x1c, 0x24, 0x2e);
        if (ca[x]) px = blend565(px, 0x3a, 0x46, 0x54, ca[x]);
        if (sa[x]) px = blend565(px, 0x8a, 0x96, 0xa4, sa[x]);
        row[x] = px;
      }
      if (ok) out.write((uint8_t *)row, RADAR_W * 2);
    }
    counties.close();
    states.close();
    if (out) out.close();
    delete[] ca;
    delete[] sa;
    delete[] row;
  }
  remove_pieces("cty");
  remove_pieces("st");
  Serial.printf("Radar base map %s in %lu ms\n", ok ? "built" : "failed", millis() - start);
  return ok;
}

static time_t radar_frame_time = 0;

void update_radar_title() {
  if (!lbl_radar_title) return;
  if (!radar_frame_time || !LittleFS.exists(RADAR_FILE)) {
    lv_label_set_text(lbl_radar_title, "RADAR");
    return;
  }
  const LocalizedStrings *strings = get_strings(current_language);
  struct tm tm;
  localtime_r(&radar_frame_time, &tm);
  if (use_24_hour) {
    lv_label_set_text_fmt(lbl_radar_title, "RADAR  %02d:%02d", tm.tm_hour, tm.tm_min);
  } else {
    int h = tm.tm_hour % 12;
    lv_label_set_text_fmt(lbl_radar_title, "RADAR  %d:%02d%s", h ? h : 12, tm.tm_min, tm.tm_hour < 12 ? strings->am : strings->pm);
  }
}

// Looks up the newest RainViewer radar frame and returns its tile URL format
static bool latest_rainviewer_frame(char *url_fmt, size_t len, time_t *frame_time) {
  JsonDocument filter;
  filter["host"] = true;
  JsonObject f = filter["radar"]["past"].add<JsonObject>();
  f["time"] = true;
  f["path"] = true;

  NetworkClientSecure client;
  client.setCACert(ISRG_ROOT_CAS);
  HTTPClient http;
  http.useHTTP10(true);
  http.begin(client, "https://api.rainviewer.com/public/weather-maps.json");
  bool ok = false;
  if (http.GET() == HTTP_CODE_OK) {
    JsonDocument doc;
    if (!deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter))) {
      JsonArray past = doc["radar"]["past"];
      const char *host = doc["host"];
      if (host && past.size()) {
        JsonObject last = past[past.size() - 1];
        const char *path = last["path"];
        if (path) {
          // 256px tiles, colour scheme 2, smoothed, snow shown
          snprintf(url_fmt, len, "%s%s/256/%%d/%%ld/%%ld/2/1_1.png", host, path);
          *frame_time = last["time"] | 0;
          ok = true;
        }
      }
    }
  }
  http.end();
  if (!ok) Serial.println("RainViewer frame lookup failed");
  return ok;
}

// Fetches the newest smoothed RainViewer radar tiles for the window and
// composites them over the base map into an LVGL image file.
void update_radar() {
  uint32_t start = millis();
  last_radar_ms = millis();
  if (!last_radar_ms) last_radar_ms = 1;
  if (!ensure_wifi()) return;

  // The update blocks the UI for several seconds; say so before it starts
  if (lbl_radar_title) {
    lv_label_set_text(lbl_radar_title, "RADAR  ...");
    lv_refr_now(NULL);
  }

  RadarWindow win = radar_window();
  char key[48];
  snprintf(key, sizeof(key), "iem:%s,%s,%d", latitude, longitude, RADAR_ZOOM);
  File kf = LittleFS.open(BASE_KEY_FILE, FILE_READ);
  bool have_base = kf && kf.readString() == key && LittleFS.exists(BASE_FILE);
  if (kf) kf.close();
  if (!have_base) {
    if (!build_base_map(win)) {
      update_radar_title();
      return;
    }
    kf = LittleFS.open(BASE_KEY_FILE, FILE_WRITE);
    kf.print(key);
    kf.close();
  }

  log_heap("before radar fetch");
  char url_fmt[160];
  time_t frame_time;
  if (!latest_rainviewer_frame(url_fmt, sizeof(url_fmt), &frame_time) ||
      !fetch_pieces(win, url_fmt, ISRG_ROOT_CAS, "rad", true, true)) {
    remove_pieces("rad");
    update_radar_title();
    return;
  }

  PieceReader radar{win, "rad", 4};
  uint8_t *rgba = new (std::nothrow) uint8_t[RADAR_W * 4];
  uint16_t *row = new (std::nothrow) uint16_t[RADAR_W];
  File base = LittleFS.open(BASE_FILE, FILE_READ);
  File out = LittleFS.open(RADAR_TMP, FILE_WRITE);
  bool ok = rgba && row && base && out;
  if (ok) {
    lv_image_header_t header = {};
    header.magic = LV_IMAGE_HEADER_MAGIC;
    header.cf = LV_COLOR_FORMAT_RGB565;
    header.w = RADAR_W;
    header.h = RADAR_H;
    header.stride = RADAR_W * 2;
    out.write((uint8_t *)&header, sizeof(header));
  }
  for (int32_t r = 0; ok && r < RADAR_H; r++) {
    ok = base.read((uint8_t *)row, RADAR_W * 2) == RADAR_W * 2 && radar.read_row(r, rgba);
    for (int x = 0; ok && x < RADAR_W; x++) {
      const uint8_t *p = &rgba[x * 4];
      if (p[3]) row[x] = blend565(row[x], p[0], p[1], p[2], p[3]);
    }
    if (ok) out.write((uint8_t *)row, RADAR_W * 2);
  }
  radar.close();
  if (base) base.close();
  if (out) out.close();
  delete[] rgba;
  delete[] row;
  remove_pieces("rad");

  if (ok) {
    LittleFS.remove(RADAR_FILE);
    LittleFS.rename(RADAR_TMP, RADAR_FILE);
    lv_image_cache_drop("L:" RADAR_FILE);
    lv_image_set_src(radar_img, "L:" RADAR_FILE);  // same path: LVGL re-reads the file
    lv_obj_invalidate(radar_img);
    radar_frame_time = frame_time;
  }
  update_radar_title();
  Serial.printf("Radar update %s in %lu ms\n", ok ? "ok" : "failed", millis() - start);
}

// US AQI from open-meteo's air quality API, coloured by EPA category
void fetch_air_quality() {
  if (WiFi.status() != WL_CONNECTED) return;
  char url[160];
  snprintf(url, sizeof(url), "https://air-quality-api.open-meteo.com/v1/air-quality?latitude=%s&longitude=%s&current=us_aqi",
           latitude, longitude);

  NetworkClientSecure client;
  client.setCACert(ISRG_ROOT_CAS);
  HTTPClient http;
  http.useHTTP10(true);
  http.begin(client, url);
  if (http.GET() == HTTP_CODE_OK) {
    JsonDocument doc;
    if (!deserializeJson(doc, http.getStream()) && doc["current"]["us_aqi"].is<int>()) {
      int aqi = doc["current"]["us_aqi"];
      uint32_t color = aqi <= 50 ? 0x7cfc00 : aqi <= 100 ? 0xffe14d : aqi <= 150 ? 0xffa040
                     : aqi <= 200 ? 0xff5a5a : aqi <= 300 ? 0xd08cff : 0xff8080;
      lv_label_set_text_fmt(lbl_now_value[NOW_ROW_AQI], "%d", aqi);
      lv_obj_set_style_text_color(lbl_now_value[NOW_ROW_AQI], lv_color_hex(color), LV_PART_MAIN | LV_STATE_DEFAULT);
      Serial.printf("Air quality: US AQI %d\n", aqi);
    }
  } else {
    Serial.println("Air quality fetch failed");
  }
  http.end();
}

void fetch_and_update_weather() {
  if (fetch_weather()) {
    last_weather_success_ms = millis();
    if (last_weather_success_ms == 0) last_weather_success_ms = 1;
    weather_fail_count = 0;
    fetch_air_quality();
  } else if (weather_fail_count < 255) {
    weather_fail_count++;
  }
  update_freshness_label();
}

static AlertLevel parse_severity(const char *s) {
  if (!s) return ALERT_MINOR;
  if (!strcmp(s, "Extreme")) return ALERT_EXTREME;
  if (!strcmp(s, "Severe")) return ALERT_SEVERE;
  if (!strcmp(s, "Moderate")) return ALERT_MODERATE;
  return ALERT_MINOR;
}

// Polls api.weather.gov for alerts at the saved location. Outside the US the
// API returns an error, which simply clears the banner.
void fetch_alerts() {
  if (!ensure_wifi()) return;

  // NWS redirects points with more than 4 decimal places
  char url[160];
  snprintf(url, sizeof(url), "https://api.weather.gov/alerts/active?status=actual&point=%.4f,%.4f",
           atof(latitude), atof(longitude));

  JsonDocument filter;
  JsonObject f = filter["features"].add<JsonObject>()["properties"].to<JsonObject>();
  f["event"] = true;
  f["severity"] = true;
  f["headline"] = true;

  log_heap("before alerts fetch");
  NetworkClientSecure client;
  client.setCACert(ISRG_ROOT_CAS);
  HTTPClient http;
  http.useHTTP10(true);
  http.setUserAgent(NWS_USER_AGENT);  // required by api.weather.gov
  http.begin(client, url);
  int code = http.GET();

  if (code == HTTP_CODE_OK) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
    if (!err) {
      AlertLevel level = ALERT_NONE;
      String event, details;
      int count = 0;
      for (JsonObject feat : doc["features"].as<JsonArray>()) {
        JsonObject props = feat["properties"];
        AlertLevel l = parse_severity(props["severity"]);
        const char *ev = props["event"] | "Weather alert";
        if (l > level) {
          level = l;
          event = ev;
        }
        if (count < 3) {
          if (details.length()) details += "\n\n";
          details += props["headline"] | ev;
        }
        count++;
      }
      if (count > 3) details += "\n\n+" + String(count - 3) + " more";

      alert_level = level;
      alert_event = event;
      alert_details = details;
      alert_count = count;
      Serial.printf("Alerts from NWS: %d active%s%s\n", count, count ? ", top: " : "", event.c_str());
    } else {
      Serial.printf("Alerts parse failed: %s\n", err.c_str());
    }
  } else if (code >= 400 && code < 500) {
    // Location outside NWS coverage
    alert_level = ALERT_NONE;
    alert_count = 0;
    Serial.printf("Alerts unavailable for this location (HTTP %d)\n", code);
  } else {
    Serial.printf("Alerts fetch failed (%d); keeping previous state\n", code);
  }
  http.end();
  update_alert_banner();
}

bool fetch_weather() {
  if (!ensure_wifi()) return false;
  bool ok = false;


  String url = String("https://api.open-meteo.com/v1/forecast?latitude=")
               + latitude + "&longitude=" + longitude
               + "&current=temperature_2m,apparent_temperature,is_day,weather_code"
               + ",relative_humidity_2m,wind_speed_10m,wind_gusts_10m,uv_index"
               + "&daily=temperature_2m_min,temperature_2m_max,weather_code"
               + ",sunrise,sunset,precipitation_sum"
               + (use_fahrenheit ? "&wind_speed_unit=mph&precipitation_unit=inch" : "")
               + "&hourly=temperature_2m,precipitation_probability,is_day,weather_code"
               + "&forecast_hours=7"
               + "&timezone=auto";

  log_heap("before weather fetch");
  NetworkClientSecure client;
  client.setCACert(ISRG_ROOT_CAS);
  HTTPClient http;
  http.useHTTP10(true);  // no chunked encoding, so the stream can be parsed directly
  http.begin(client, url);

  if (http.GET() == HTTP_CODE_OK) {
    Serial.println("Updated weather from open-meteo: " + url);

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, http.getStream());
    log_heap("after weather parse");

    JsonArray times = doc["daily"]["time"].as<JsonArray>();
    JsonArray hours = doc["hourly"]["time"].as<JsonArray>();
    bool complete = times.size() >= 7 && hours.size() >= 7;
    for (int i = 0; complete && i < 7; i++) {
      const char *d = times[i];
      const char *h = hours[i];
      complete = d && strlen(d) >= 10 && h && strlen(h) >= 16;
    }

    if (err == DeserializationError::Ok && complete) {
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
      update_now_values(doc, strings);
      ok = true;


    } else {
      Serial.printf("Unusable weather response (%s) from %s\n", err.c_str(), url.c_str());
    }
  } else {
    Serial.println("HTTP GET failed at " + url);
  }
  http.end();
  return ok;
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
