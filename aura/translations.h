#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

// Language support
enum Language { LANG_EN = 0, LANG_ES = 1, LANG_DE = 2, LANG_FR = 3, LANG_TR = 4, LANG_SV = 5, LANG_IT = 6 };

struct LocalizedStrings {
  const char* temp_placeholder;
  const char* feels_like_temp;
  const char* seven_day_forecast;
  const char* hourly_forecast;
  const char* today;
  const char* now;
  const char* am;
  const char* pm;
  const char* noon;
  const char* invalid_hour;
  const char* brightness;
  const char* location;
  const char* use_fahrenheit;
  const char* use_24hr;
  const char* save;
  const char* cancel;
  const char* close;
  const char* location_btn;
  const char* reset_wifi;
  const char* reset;
  const char* change_location;
  const char* aura_settings;
  const char* city;
  const char* search_results;
  const char* city_placeholder;
  const char* wifi_config;
  const char* reset_confirmation;
  const char* language_label;
  const char* color_scheme_label;
  const char* color_schemes[8]; // Blue, Red, Yellow, Orange, White, Black, Green, Pink
  const char* weekdays[7];
  const char* use_night_mode;
};

#define DEFAULT_CAPTIVE_SSID "Aura"

static const LocalizedStrings strings_en = {
  "--°C", "Feels Like", "SEVEN DAY FORECAST", "HOURLY FORECAST",
  "Today", "Now", "am", "pm", "Noon", "Invalid hour",
  "Brightness:", "Location:", "Use °F:", "24hr:",
  "Save", "Cancel", "Close", "Location", "Reset Wi-Fi",
  "Reset", "Change Location", "Aura Settings",
  "City:", "Search Results", "e.g. London",
  "Wi-Fi Configuration:\n\n"
  "Please connect your\n"
  "phone or laptop to the\n"
  "temporary Wi-Fi access\n point "
  DEFAULT_CAPTIVE_SSID
  "\n"
  "to configure.\n\n"
  "If you don't see a \n"
  "configuration screen \n"
  "after connecting,\n"
  "visit http://192.168.4.1\n"
  "in your web browser.",
  "Are you sure you want to reset "
  "Wi-Fi credentials?\n\n"
  "You'll need to reconnect to the Wifi SSID " DEFAULT_CAPTIVE_SSID
  " with your phone or browser to "
  "reconfigure Wi-Fi credentials.",
  "Language:",
  "Color scheme:",
  {"Blue", "Red", "Yellow", "Orange", "White", "Black", "Green", "Pink"},
  {"Sun", "Mon", "Tues", "Wed", "Thurs", "Fri", "Sat"},
  "Dim screen at night"
};

static const LocalizedStrings strings_es = {
  "--°C", "Sensación", "PRONÓSTICO 7 DÍAS", "PRONÓSTICO POR HORAS",
  "Hoy", "Ahora", "am", "pm", "Mediodía", "Hora inválida",
  "Brillo:", "Ubicación:", "Usar °F:", "24h:",
  "Guardar", "Cancelar", "Cerrar", "Ubicación", "Wi-Fi",
  "Restablecer", "Cambiar Ubicación", "Configuración Aura",
  "Ciudad:", "Resultados de Búsqueda", "ej. Madrid",
  "Configuración Wi-Fi:\n\n"
  "Conecte su teléfono\n"
  "o portátil al punto de\n"
  "acceso Wi-Fi temporal\n"
  DEFAULT_CAPTIVE_SSID
  "\n"
  "para configurar.\n\n"
  "Si no ve una pantalla\n"
  "de configuración después\n"
  "de conectarse, visite\n"
  "http://192.168.4.1\n"
  "en su navegador.",
  "¿Está seguro de que desea\n"
  "restablecer las credenciales\n"
  "Wi-Fi?\n\n"
  "Deberá reconectarse al SSID " DEFAULT_CAPTIVE_SSID
  " con su teléfono o navegador\n"
  "para reconfigurar las\n"
  "credenciales Wi-Fi.",
  "Idioma:",
  "Esquema de color:",
  {"Azul", "Rojo", "Amarillo", "Naranja", "Blanco", "Negro", "Verde", "Rosa"},
  {"Dom", "Lun", "Mar", "Mié", "Jue", "Vie", "Sáb"},
  "Pantalla noche"
};

static const LocalizedStrings strings_de = {
  "--°C", "Gefühlt", "7-TAGE VORHERSAGE", "STÜNDLICHE VORHERSAGE",
  "Heute", "Jetzt", "", "", "Mittag", "Ungültige Stunde",
  "Helligkeit:", "Standort:", "°F:", "24h:",
  "Speichern", "Abbrechen", "Schließen", "Standort", "Wi-Fi",
  "Zurücksetzen", "Standort ändern", "Aura Einstellungen",
  "Stadt:", "Suchergebnisse", "z.B. Berlin",
  "Wi-Fi Konfiguration:\n\n"
  "Verbinden Sie Ihr Telefon\n"
  "oder Laptop mit dem\n"
  "temporären Wi-Fi\n"
  "Zugangspunkt "
  DEFAULT_CAPTIVE_SSID
  "\n"
  "zum Konfigurieren.\n\n"
  "Wenn Sie keinen\n"
  "Konfigurationsbildschirm\n"
  "sehen, besuchen Sie\n"
  "http://192.168.4.1\n"
  "in Ihrem Browser.",
  "Sind Sie sicher, dass Sie\n"
  "die Wi-Fi Zugangsdaten\n"
  "zurücksetzen möchten?\n\n"
  "Sie müssen sich erneut mit\n"
  "der SSID " DEFAULT_CAPTIVE_SSID
  " verbinden, um die\n"
  "Wi-Fi Zugangsdaten\n"
  "neu zu konfigurieren.",
  "Sprache:",
  "Farbschema:",
  {"Blau", "Rot", "Gelb", "Orange", "Weiß", "Schwarz", "Grün", "Rosa"},
  {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"},
  "Nacht-Dimmen"
};

static const LocalizedStrings strings_fr = {
  "--°C", "Ressenti", "PRÉVISIONS 7 JOURS", "PRÉVISIONS HORAIRES",
  "Aujourd'hui", "Maintenant", "h", "h", "Midi", "Heure invalide",
  "Luminosité:", "Lieu:", "Utiliser °F:", "24h:",
  "Sauvegarder", "Annuler", "Fermer", "Lieu", "Wi-Fi",
  "Réinitialiser", "Changer de lieu", "Paramètres Aura",
  "Ville:", "Résultats de recherche", "ex. Paris",
  "Configuration Wi-Fi:\n\n"
  "Connectez votre téléphone\n"
  "ou ordinateur portable au\n"
  "point d'accès Wi-Fi\n"
  "temporaire "
  DEFAULT_CAPTIVE_SSID
  "\n"
  "pour configurer.\n\n"
  "Si vous ne voyez pas\n"
  "d'écran de configuration\n"
  "après connexion, visitez\n"
  "http://192.168.4.1\n"
  "dans votre navigateur.",
  "Êtes-vous sûr de vouloir\n"
  "réinitialiser les\n"
  "identifiants Wi-Fi?\n\n"
  "Vous devrez vous reconnecter\n"
  "au SSID " DEFAULT_CAPTIVE_SSID
  " avec votre téléphone ou\n"
  "navigateur pour reconfigurer\n"
  "les identifiants Wi-Fi.",
  "Langue:",
  "Schéma de couleur:",
  {"Bleu", "Rouge", "Jaune", "Orange", "Blanc", "Noir", "Vert", "Rose"},
  {"Dim", "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam"},
  "Nuit écran"
};

static const LocalizedStrings strings_tr = {
  "--°C", "Hissedilen", "YEDI GÜNLÜK TAHMIN", "SAATLIK TAHMIN",
  "Bugün", "Simdi", "öö", "ös", "Öğle", "Geçersiz saat",
  "Parlaklik:", "Konum:", "°F Kullan:", "24 Saat:",
  "Kaydet", "İptal", "Kapat", "Konum", "Wi-Fi Sifirla",
  "Sifirla", "Konumu Değiştir", "Aura Ayarlari",
  "Şehir:", "Arama Sonuçları", "örn. Londra",
  "Wi-Fi Yapilandirmasi:\n\n"
  "Lütfen telefonunuzu veya\n"
  "bilgisayarinizi geçici Wi-Fi\n"
  "erişim noktasina bağlayin "
  DEFAULT_CAPTIVE_SSID
  "\n"
  "yapilandirmak için.\n\n"
  "Bağlandiktan sonra bir\n"
  "yapilandirma ekrani görmezseniz,\n"
  "web tarayicinizda\n"
  "http://192.168.4.1 adresine gidin.",
  "Wi-Fi kimlik bilgilerini sifirlamak\n"
  "istediğinizden emin misiniz?\n\n"
  "Wi-Fi kimlik bilgilerini yeniden\n"
  "yapilandirmak için telefonunuz veya\n"
  "tarayiciniz ile " DEFAULT_CAPTIVE_SSID
  " SSID'sine tekrar bağlanmaniz\n"
  "gerekecek.",
  "Dil:",
  "Renk şeması:",
  {"Mavi", "Kırmızı", "Sarı", "Turuncu", "Beyaz", "Siyah", "Yeşil", "Pembe"},
  {"Paz", "Pzt", "Sal", "Çar", "Per", "Cum", "Cmt"},
  "Gece kısık"
};

static const LocalizedStrings strings_sv = {
  "--°C", "Känns som", "7-DAGARS PROGNOS", "TIMPROGNOS",
  "Idag", "Nu", "fm", "em", "Middag", "Ogiltig timme",
  "Ljusstyrka:", "Plats:", "Använd °F:", "24h:",
  "Spara", "Avbryt", "Stäng", "Plats", "Återställ Wi-Fi",
  "Återställ", "Ändra plats", "Aura-inställningar",
  "Stad:", "Sökresultat", "t.ex. Stockholm",
  "Wi-Fi-konfiguration:\n\n"
  "Anslut din telefon\n"
  "eller laptop till den\n"
  "tillfälliga Wi-Fi-\n"
  "åtkomstpunkten "
  DEFAULT_CAPTIVE_SSID
  "\n"
  "för att konfigurera.\n\n"
  "Om du inte ser en\n"
  "konfigurationsskärm\n"
  "efter anslutning, besök\n"
  "http://192.168.4.1\n"
  "i din webbläsare.",
  "Är du säker på att du vill\n"
  "återställa Wi-Fi-\n"
  "autentiseringsuppgifter?\n\n"
  "Du måste återansluta till\n"
  "SSID " DEFAULT_CAPTIVE_SSID
  " med din telefon eller\n"
  "webbläsare för att\n"
  "omkonfigurera Wi-Fi-\n"
  "autentiseringsuppgifter.",
  "Språk:",
  "Färgschema:",
  {"Blå", "Röd", "Gul", "Orange", "Vit", "Svart", "Grön", "Rosa"},
  {"Sön", "Mån", "Tis", "Ons", "Tor", "Fre", "Lör"},
  "Dimma nattetid"
};

static const LocalizedStrings strings_it = {
  "--°C", "Percepita", "PREVISIONI A 7 GIORNI", "PREVISIONI ORARIE",
  "Oggi", "Ora", "am", "pm", "Mezzog.", "Ora non valida",
  "Luminosità:", "Posizione:", "Utilizzo °F:", "24hr:",
  "Salva", "Cancellare", "Close", "Posizione", "Resetta Wi-Fi",
  "Reset", "Cambia posizione", "Impostazioni aura",
  "Città:", "Risultati di ricerca", "e.s. Londra",
  "Configurazione Wi-Fi:\n\n"
  "Per favore collega il tuo\n"
  "smartphone o laptop\n"
  "al Wi-Fi temporaneo\n "
  DEFAULT_CAPTIVE_SSID
  "\n"
  "per configurare la rete.\n\n"
  "Se non vedi la \n"
  "Schermata di configurazione \n"
  "dopo il collegamento,\n"
  "visita http://192.168.4.1\n"
  "sul tuo web browser.",
  "Sei sicuro di voler ripristinare "
  "le credenzili Wi-Fi ?\n\n"
  "Dovrai riconnetterti al WiFi con SSID " DEFAULT_CAPTIVE_SSID
  "con il tuo telefono o browser a "
  "riconfigurare le credenziali Wi-Fi.",
  "Lingua:",
  "Schema colore:",
  {"Blu", "Rosso", "Giallo", "Arancione", "Bianco", "Nero", "Verde", "Rosa"},
  {"Dom", "Lun", "Mar", "Mer", "Gio", "Ven", "Sab"},
  "Schermo notte"
};

static const LocalizedStrings* get_strings(Language current_language) {
  switch (current_language) {
    case LANG_ES: return &strings_es;
    case LANG_DE: return &strings_de;
    case LANG_FR: return &strings_fr;
    case LANG_TR: return &strings_tr;
    case LANG_SV: return &strings_sv;
    case LANG_IT: return &strings_it;
    default: return &strings_en;
  }
}

#endif // TRANSLATIONS_H
