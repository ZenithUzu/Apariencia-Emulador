// PC example: the menu running with no console and no emulator. FakeHost is
// what an emulator writes for real (ui/UiHost.h): here it lists folders of
// the PC, makes up a game picture and keeps save states in memory.
// A script of button presses drives the menu; "shot" saves the screen as a
// .ppm picture. Build and run: see the Makefile and README.md.
static int g_hx = -1, g_hy = -1;   // Kinect hand (the "hand x y" command)
#include "UiApp.h"
#include "UiScreens.h"
#include "UiThumb.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <string>
#include <vector>

static std::string g_root;
static int g_rsx = 0, g_rsy = 0;   // fake drives live under here

struct FakeHost : public UiHost
{
  virtual void rightStick(int &x, int &y) { x = g_rsx; y = g_rsy; }
  std::vector<UiCheatInfo> cheats;
  std::vector<UiColor> frame;
  std::string rom;
  std::map<int, bool> states;
  int resets;
  FakeHost() : resets(0)
  {
    frame.resize(240 * 160);
    for (int y = 0; y < 160; y++)
      for (int x = 0; x < 240; x++) {
        int r = 40 + x / 2, g = 90 + y / 2, b = 160;
        if (y > 110) { r = 60; g = 150 + (x % 20); b = 70; }   // grass
        if ((x - 120) * (x - 120) + (y - 70) * (y - 70) < 300) { r = 230; g = 60; b = 60; }
        frame[y * 240 + x] = (r << 16) | (g << 8) | b;
      }
  }
  virtual void listDrives(std::vector<UiDirEntry> &out)
  {
    const char *d[][2] = { { "Disco duro (hdd:)", "hdd:\\" }, { "USB (usb0:)", "usb0:\\" },
                           { "Carpeta del emulador (game:)", "game:\\" } };
    for (int i = 0; i < 3; i++) { UiDirEntry e; e.name = d[i][0]; e.path = d[i][1]; e.isDir = true; e.size = 0; out.push_back(e); }
  }
  static std::string toReal(const std::string &p)
  {
    std::string s = p;
    size_t c = s.find(':');
    std::string r = g_root + "/" + s.substr(0, c) + "/" + s.substr(c + 1);
    for (size_t i = 0; i < r.size(); i++) if (r[i] == '\\') r[i] = '/';
    return r;
  }
  virtual bool listDir(const std::string &path, std::vector<UiDirEntry> &out)
  {
    std::string cmd = "cd '" + toReal(path) + "' 2>/dev/null && ls -1p | LC_ALL=C sort -f";
    FILE *p = popen(cmd.c_str(), "r");
    if (!p) return false;
    char line[512];
    std::vector<UiDirEntry> dirs, files;
    while (fgets(line, sizeof line, p)) {
      std::string n = line; n.erase(n.find_last_not_of("\n") + 1);
      UiDirEntry e; e.isDir = !n.empty() && n[n.size() - 1] == '/';
      if (e.isDir) n.erase(n.size() - 1);
      e.name = n; e.path = uiJoinPath(path, n) + (e.isDir ? "\\" : "");
      e.size = e.isDir ? 0 : 16 * 1024 * 1024 + (unsigned)n.size() * 1000;
      (e.isDir ? dirs : files).push_back(e);
    }
    pclose(p);
    out.insert(out.end(), dirs.begin(), dirs.end());
    out.insert(out.end(), files.begin(), files.end());
    return true;
  }
  virtual bool fileExists(const std::string &path) { FILE *f = fopen(toReal(path).c_str(), "rb"); if (f) fclose(f); return f != 0; }
  virtual std::string emulatorDir() { return "game:\\"; }
  virtual std::string currentRom() { return rom; }
  virtual bool saveState(int slot) { states[slot] = true; return uiSaveThumb(stateThumbPath(slot), &frame[0], 240, 160, 240); }
  virtual bool loadState(int slot) { return states[slot]; }
  virtual bool stateExists(int slot) { return states[slot]; }
  virtual std::string stateThumbPath(int slot) { char b[512]; sprintf(b, "%s/game/states/demo%d.tmb", g_root.c_str(), slot); return b; }
  virtual bool takeScreenshot(std::string &to) { to = "game:\\screenshots\\demo01.png"; return true; }
  virtual void resetGame() { resets++; }
  virtual const UiColor *gameFrame(int &w, int &h, int &p) { if (rom.empty()) return 0; w = 240; h = 160; p = 240; return &frame[0]; }
  virtual void listCheats(const std::string &, std::vector<UiCheatInfo> &out) { out = cheats; }
  virtual void setCheatEnabled(const std::string &, int i, bool e) { cheats[i].enabled = e; }
  virtual bool addCheat(const std::string &, const std::string &code, int, const std::string &desc)
  { if (code.size() < 8) return false; UiCheatInfo c; c.code = code; c.desc = desc; c.enabled = true; cheats.push_back(c); return true; }
  virtual void removeCheat(const std::string &, int i) { cheats.erase(cheats.begin() + i); }
  virtual int importCheatFile(const std::string &, const std::string &) { return 0; }
  std::vector<UiCoverItem> covItems; int covTick; bool covCancel;
  virtual bool startCoverDownload(const std::vector<UiCoverItem> &items, const std::string &server)
  { covItems = items; covTick = 0; covCancel = false; printf("cover job %d items from %s\n", (int)items.size(), server.c_str());
    for (size_t i=0;i<items.size();i++) printf("  %s -> %s [%d %d %d]\n", items[i].romName.c_str(), items[i].coverPath.c_str(), items[i].systems[0], items[i].systems[1], items[i].systems[2]);
    return true; }
  virtual void coverDownloadStatus(UiCoverStatus &s)
  { covTick++; s = UiCoverStatus(); s.total = (int)covItems.size(); int d = covTick / 10; if (d > s.total) d = s.total;
    s.done = d; s.found = d - d / 3; s.missing = d / 3; s.running = d < s.total && !covCancel;
    if (s.running) s.current = covItems[d].romName; if (covCancel) s.error = "cancel"; }
  virtual void cancelCoverDownload() { covCancel = true; }
  // Covers in the sim: PPM data saved with a .png name.
  virtual bool loadImageFit(const std::string &path, int maxW, int maxH, std::vector<UiColor> &out, int &w, int &h)
  { FILE *f = fopen(toReal(path).c_str(), "rb"); if (!f) return false; int sw, sh, mx;
    if (fscanf(f, "P6 %d %d %d", &sw, &sh, &mx) != 3) { fclose(f); return false; } fgetc(f);
    std::vector<unsigned char> d(sw * sh * 3); fread(&d[0], 1, d.size(), f); fclose(f);
    w = maxW; h = sh * maxW / sw; if (h > maxH) { h = maxH; w = sw * maxH / sh; }
    out.resize(w * h); for (int y = 0; y < h; y++) for (int x = 0; x < w; x++) { int i = ((y * sh / h) * sw + x * sw / w) * 3;
      out[y * w + x] = 0xFF000000 | (d[i] << 16) | (d[i + 1] << 8) | d[i + 2]; } return true; }
  virtual bool loadImage(const std::string &path, int w, int h, std::vector<UiColor> &out)
  { int ww, hh; return loadImageFit(path, w, h, out, ww, hh) && ww == w && hh == h; }
  virtual bool handCursor(int &x, int &y) { x = g_hx; y = g_hy; return g_hx >= 0; }
  virtual void quitToDashboard() { printf("quit to dashboard\n"); }
  virtual void applySettings() {}
};

static void savePPM(const UiCanvas &c, const std::string &path)
{
  FILE *f = fopen(path.c_str(), "wb");
  fprintf(f, "P6 %d %d 255\n", c.width(), c.height());
  for (int i = 0; i < c.width() * c.height(); i++) {
    UiColor p = c.pixels()[i];
    unsigned char rgb[3] = { (unsigned char)(p >> 16), (unsigned char)(p >> 8), (unsigned char)p };
    fwrite(rgb, 1, 3, f);
  }
  fclose(f);
}

// Usage: simulador <root> <script> <out dir>
// <root>: folder standing for the console's drives (<root>/game, <root>/hdd...).
// Script lines: "<button> [times]" (up down left right a b x y lb rb lt rt
// start back ls rs), "shot <name>" (settle and save), "snap <name>" (save now),
// "game <rom>" (in-game menu), "home", "wait <frames>", "rs <x> <y>" (right
// stick), "hand <x> <y>" (Kinect hand, -1 none), "bench <frames>" (speed).
int main(int argc, char **argv)
{
  g_root = argv[1];
  FakeHost host;
  UiConfig cfg; if (getenv("THEME")) cfg.menuTheme = atoi(getenv("THEME")); if (getenv("STYLE")) cfg.optionStyle = atoi(getenv("STYLE")); if (getenv("LIST")) cfg.listView = atoi(getenv("LIST")); if (getenv("FX")) cfg.bgEffect = atoi(getenv("FX")); if (getenv("LIB")) cfg.libraryDir = getenv("LIB");
  if (getenv("CFG")) cfg.load(getenv("CFG"));
  UiApp app(host, cfg);
  app.loadLists();
  app.openHome();
  UiCanvas canvas;
  canvas.create(1280, 720);
  static const char *names[] = { "up","down","left","right","a","b","x","y","lb","rb","lt","rt","start","back","ls","rs" };
  char line[256];
  FILE *script = fopen(argv[2], "r");
  while (fgets(line, sizeof line, script)) {
    char cmd[64] = "", arg[200] = "";
    if (sscanf(line, "%63s %199[^\n]", cmd, arg) < 1 || cmd[0] == '#') continue;
    std::string c = cmd;
    if (c == "snap") { app.update(0); app.draw(canvas); savePPM(canvas, std::string(argv[3]) + "/" + arg + ".ppm"); continue; }
    if (c == "shot") {
      for (int i = 0; i < 12; i++) { app.update(0); app.draw(canvas); }
      savePPM(canvas, std::string(argv[3]) + "/" + arg + ".ppm");
      continue;
    }
    if (c == "game") { host.rom = arg; app.openInGameMenu(); continue; }
    if (c == "home") { host.rom = ""; app.openHome(); continue; }
    if (c == "rs") { sscanf(arg, "%d %d", &g_rsx, &g_rsy); app.update(0); continue; }
    if (c == "bench") { clock_t t0 = clock(); int n = atoi(arg); for (int i = 0; i < n; i++) { app.update(0); app.draw(canvas); }
      printf("%s: %.2f ms/draw\n", line, (clock() - t0) * 1000.0 / CLOCKS_PER_SEC / n); continue; }
    if (c == "hand") { sscanf(arg, "%d %d", &g_hx, &g_hy); continue; }
    if (c == "wait") { for (int i = 0; i < atoi(arg); i++) app.update(0); continue; }
    int times = arg[0] ? atoi(arg) : 1;
    for (int b = 0; b < 16; b++) if (c == names[b]) {
      for (int t = 0; t < times; t++) { app.update(1u << b); app.update(0); }
    }
    std::string rom;
    UiRequest r = app.takeRequest(rom);
    if (r) printf("request %d %s\n", r, rom.c_str());
  }
  return 0;
}
