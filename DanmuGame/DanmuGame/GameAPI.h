#ifndef _GAMEAPI_H_
#define _GAMEAPI_H_

#include "Macro.h"
#include "Util.h"

#include <lua.hpp>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL2_gfxPrimitives.h>

#include <string>
#include <unordered_map>

extern SDL_Window* g_pWindow;
extern SDL_Renderer* g_pRenderer;

GAME_API int QuitGame(lua_State* pLuaVM);

GAME_API int SetWindowTitle(lua_State* pLuaVM);
GAME_API int GetWindowTitle(lua_State* pLuaVM);
GAME_API int GetWindowSize(lua_State* pLuaVM);

GAME_API int SetDrawColor(lua_State* pLuaVM);
GAME_API int GetDrawColor(lua_State* pLuaVM);

GAME_API int DrawPoint(lua_State* pLuaVM);
GAME_API int DrawLine(lua_State* pLuaVM);
GAME_API int DrawRectangle(lua_State* pLuaVM);
GAME_API int DrawRoundRectangle(lua_State* pLuaVM);
GAME_API int DrawCircle(lua_State* pLuaVM);
GAME_API int DrawEllipse(lua_State* pLuaVM);
GAME_API int DrawPie(lua_State* pLuaVM);
GAME_API int DrawTriangle(lua_State* pLuaVM);
GAME_API int DrawPolygon(lua_State* pLuaVM);
GAME_API int DrawBezier(lua_State* pLuaVM);

GAME_API int LoadSprite(lua_State* pLuaVM);
GAME_API int GC_Sprite(lua_State* pLuaVM);
GAME_API int SetSpriteAlpha(lua_State* pLuaVM);
GAME_API int GetSpriteSize(lua_State* pLuaVM);
GAME_API int RenderSprite(lua_State* pLuaVM);
GAME_API int RenderSpriteEx(lua_State* pLuaVM);
GAME_API int LoadFont(lua_State* pLuaVM);
GAME_API int GC_Font(lua_State* pLuaVM);
GAME_API int GetFontStyle(lua_State* pLuaVM);
GAME_API int SetFontStyle(lua_State* pLuaVM);
GAME_API int GetFontHeight(lua_State* pLuaVM);
GAME_API int GetTextSize(lua_State* pLuaVM);
GAME_API int CreateTextSprite(lua_State* pLuaVM);

GAME_API int Delay(lua_State* pLuaVM);
GAME_API int GetInitTime(lua_State* pLuaVM);

GAME_API int LoadMusic(lua_State* pLuaVM);
GAME_API int GC_Music(lua_State* pLuaVM);
GAME_API int PlayMusic(lua_State* pLuaVM);
GAME_API int StopMusic(lua_State* pLuaVM);
GAME_API int SetMusicPosition(lua_State* pLuaVM);
GAME_API int SetMusicVolume(lua_State* pLuaVM);
GAME_API int GetMusicVolume(lua_State* pLuaVM);
GAME_API int PauseMusic(lua_State* pLuaVM);
GAME_API int ResumeMusic(lua_State* pLuaVM);
GAME_API int RewindMusic(lua_State* pLuaVM);
GAME_API int CheckMusicPlaying(lua_State* pLuaVM);
GAME_API int CheckMusicPaused(lua_State* pLuaVM);
GAME_API int GetMusicFadingType(lua_State* pLuaVM);
GAME_API int LoadSound(lua_State* pLuaVM);
GAME_API int GC_Sound(lua_State* pLuaVM);
GAME_API int PlaySound(lua_State* pLuaVM);
GAME_API int SetSoundVolume(lua_State* pLuaVM);
GAME_API int GetSoundVolume(lua_State* pLuaVM);

GAME_API int LoadJSON(lua_State* pLuaVM);
GAME_API int DumpJSON(lua_State* pLuaVM);

#endif // !_GAMEAPI_H_
