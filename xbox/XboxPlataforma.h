// The Xbox 360 side of the interface: what this folder gives the emulator and
// what the emulator has to give it. Everything here was made for AdvanXe360
// (VBA-M + Gambatte on libSDLx360); another emulator implements the second
// half of this file and calls the first. See README.md, "Adaptarlo".
#ifndef XBOX_PLATAFORMA_H
#define XBOX_PLATAFORMA_H

#include <string>
#include <vector>

class UiCanvas;
class UiHost;
struct UiConfig;
typedef unsigned int UiColor;   // as in ui/UiCanvas.h

// ===== Given by this folder ======================================================

// --- XboxUiRenderer.cpp: the menu on screen, images, the picture around the game
bool xboxUiRendererInit();
// Shows the menu canvas (1280x720). changed = false: the last picture again.
void xboxUiPresent(const UiCanvas &canvas, bool changed = true);
// Decodes an image (PNG, JPG, BMP, TGA, DDS) scaled to w x h. smooth: box
// filter (better when shrinking a lot, slower); else linear.
bool xboxLoadImagePixels(const char *path, int w, int h, std::vector<UiColor> &out, bool smooth = false);
// The same keeping its proportions, at most maxW x maxH (covers).
bool xboxLoadImageFit(const char *path, int maxW, int maxH, std::vector<UiColor> &out, int &w, int &h);
// Background around the game picture: black plus the image at path (NULL or
// "" = black), fitted as UiBorderFit says.
bool xboxSetBorderImage(const char *path, int fit);
void xboxSetBorderFit(int fit);
// A picture over the game (achievement pop-ups): its pixels (0 = hide), then
// where it goes each frame and how many of its columns show (it unfolds).
void xboxOverlaySet(const UiCanvas *canvas);
void xboxOverlayPlace(int x, int y, int shownWidth);

// --- XboxMusic.cpp: menu music and short sounds
// A short sound: a 16-bit .wav, or a little chime when path is "" or unreadable.
void xboxPlaySound(const std::string &path);

// --- XboxCovers.h (covers, cheats, HTTP), XboxTls.h (HTTPS), XboxCheevos.h
// (RetroAchievements) and XboxLog.h have their own headers.

// ===== Given by the emulator =====================================================

// The settings (ui/UiConfig.h), loaded at start and saved by the menu.
extern UiConfig g_cfg;
// The emulator's UiHost (ui/UiHost.h): what the menu asks of the emulator.
UiHost &xboxHost();
// The emulator's folder, "game:\" usually.
std::string xboxEmulatorDir();
// Puts back the game picture's texture filter after the overlay was drawn.
void xboxApplyGameSampler();
// After a frame that was slow on purpose (a pop-up being made): auto frame
// skip should measure afresh instead of skipping. Empty if there is none.
void xboxResetFrameTiming();

// From libSDLx360 (or your own video/audio code doing the same):
//   IDirect3DDevice9 *pDevice;                 the Direct3D device
//   void XBOX_RestoreVideoState(void);         the game's render state back
//   void (*XBOX_OverlayHook)(void);            called before each Present
//   void (*XBOX_BackgroundHook)(void);         called before the game picture
//   IXAudio2 *XboxSDL_AudioEngine(void);       the XAudio2 engine
// (declared where they are used, as they need the XDK headers).

#endif
