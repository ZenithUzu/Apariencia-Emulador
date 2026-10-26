// Your emulator's name and files: change these (and UiMarca.cpp, its About
// page and credits) to use this interface for another emulator. As it comes,
// it is AdvanXe360's.
#ifndef UI_MARCA_H
#define UI_MARCA_H

// The name the menus show, and the version (compared with the latest GitHub
// release, "v0.5.0", by "Check for updates").
#define UI_APP_NAME "AdvanXe360"
#define UI_APP_VERSION "0.5.0"
// After "Name version" in About ("" for nothing).
#define UI_APP_SUBTITLE_ES " (antes VBA360 enhanced-ai)"
#define UI_APP_SUBTITLE_EN " (formerly VBA360 enhanced-ai)"
// Under the name in the side panel ("" for nothing).
#define UI_APP_BASED_ON_ES "basado en VBA360"
#define UI_APP_BASED_ON_EN "based on VBA360"
// Where new versions are (said when one is found).
#define UI_APP_RELEASES "github.com/ZenithUzu/AdvanXe360/releases"
// Settings file and log, in the emulator's folder.
#define UI_APP_INI "vba360.ini"
#define UI_APP_LOG "vba360.log"

// The About page (UiMarca.cpp): lines under the title (es, en); what it
// emulates and what it doesn't: what (es, en), short answer (es, en),
// details (es, en); credits: role (es, en), who, details (es, en).
extern const char *const uiAboutInfo[][2];
extern const int uiAboutInfoCount;
extern const char *const uiAboutYes[][6];
extern const int uiAboutYesCount;
extern const char *const uiAboutNo[][6];
extern const int uiAboutNoCount;
extern const char *const uiAboutCredits[][5];
extern const int uiAboutCreditCount;

#endif
