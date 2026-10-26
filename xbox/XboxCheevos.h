// RetroAchievements on the console: rcheevos (rc_client) plus the network
// (HTTPS, on a thread of its own), the game's memory and the pop-ups shown
// during the game. Softcore only (hardcore is for emulators the site approves).
#ifndef XBOX_CHEEVOS_H
#define XBOX_CHEEVOS_H

#include <string>
#include <vector>

struct UiRaInfo;
struct UiRaAchievement;

// The game's memory as RetroAchievements numbers it for that console (its
// "memory map" on docs.retroachievements.org): one byte, false past the end.
// Example, Game Boy Advance: 0x00000 IWRAM (32 KB), 0x08000 EWRAM (256 KB),
// 0x48000 save RAM (64 KB).
typedef bool (*CheevosReadByte)(unsigned address, unsigned char &out);

// At start-up, and when the option changes: logs in with the saved token.
void cheevosStart();
// Finishes the network replies that came back (login, game data, unlocks).
// Call it often, from the emulator's thread: every menu and game frame.
void cheevosPump();
// The game started: finds it on RetroAchievements (by the ROM's hash).
// consoleId: RC_CONSOLE_* from rcheevos (rc_consoles.h), e.g.
// RC_CONSOLE_GAMEBOY_ADVANCE, RC_CONSOLE_NINTENDO, RC_CONSOLE_MEGA_DRIVE.
void cheevosGameStarted(unsigned consoleId, const unsigned char *rom, unsigned size, CheevosReadByte read);
// After every emulated frame: checks the achievements, moves the pop-ups.
void cheevosFrame();
// While the game is paused (in-game menu).
void cheevosIdle();
void cheevosGameExited();
// The game was reset or a saved state loaded: its memory changed at once, so
// achievements wait for fresh conditions instead of firing by mistake.
void cheevosReset();

void cheevosLogin(const std::string &user, const std::string &password);
void cheevosLogout();
void cheevosInfo(UiRaInfo &out);
void cheevosList(std::vector<UiRaAchievement> &out);

#endif
