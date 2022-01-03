#ifndef _DANMUGAME_H_
#define _DANMUGAME_H_

#include "GameAPI.h"

#include <lua.hpp>
#include <httplib.h>
#include <cJSON.h>
#include <SDL.h>

#include <thread>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>
#include <unordered_map>

SDL_Window* g_pWindow = nullptr;
SDL_Renderer* g_pRenderer = nullptr;

struct EventHandlerParam
{
	std::string name;
	std::function<void()> PushExtraData;
};

struct MetaTableData
{
	std::string name;
	lua_CFunction gc_func;
};

#endif // !_DANMUGAME_H_
