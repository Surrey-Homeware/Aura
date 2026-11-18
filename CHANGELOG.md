# Changelog

## 0.1.0 (2026-11-17)

First release of this fork of [Surrey-Homeware/Aura](https://github.com/Surrey-Homeware/Aura).

### Features

* Grateful Dead "Steal Your Face" startup splash
* Browser-based firmware installer (GitHub Pages) and flashing docs

### Bug Fixes

* Clear stale keyboard/dialog pointers and duplicate screen callback after a language change (crash)
* Make `getLocalTime` non-blocking so the UI doesn't freeze before NTP syncs
* Reconnect Wi-Fi with saved credentials instead of an endless config portal
* Bound the geocode dropdown buffer and handle null result names
* Reset search results and block saving when a location search returns nothing
