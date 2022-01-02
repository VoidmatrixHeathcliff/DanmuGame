#include <httplib.h>
#include <cJSON.h>
#include <SDL.h>
#include <lua.hpp>

#include <thread>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

#define GAME_API extern "C"

Uint32 g_u32EventTypeEnter, g_u32EventTypeMessage;

SDL_Window* g_pWindow = nullptr;
SDL_Renderer* g_pRenderer = nullptr;

SDL_Event g_event;
bool g_bIsQuit = false;

std::unordered_map<std::string, Uint32> mapEvent;

static const char* ERRMSG_INVALIDMEMBER = "invalid member";

inline void CheckColor(lua_State* pLuaVM, int idx, SDL_Color& color)
{
	luaL_argexpected(pLuaVM, lua_istable(pLuaVM, idx), idx, LUA_TABLIBNAME);

	lua_pushstring(pLuaVM, "r"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : r").c_str());
	color.r = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);

	lua_pushstring(pLuaVM, "g"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : g").c_str());
	color.g = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);

	lua_pushstring(pLuaVM, "b"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : b").c_str());
	color.b = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);

	lua_pushstring(pLuaVM, "a"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : a").c_str());
	color.a = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);
}

GAME_API int SetDrawColor(lua_State* pLuaVM)
{
	SDL_Color _color; CheckColor(pLuaVM, 1, _color);

	if (_color.a != 255) SDL_SetRenderDrawBlendMode(g_pRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(g_pRenderer, _color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int ClearScreen(lua_State* pLuaVM)
{
	SDL_RenderClear(g_pRenderer);

	return 0;
}

GAME_API int UpdateScreen(lua_State* pLuaVM)
{
	SDL_RenderPresent(g_pRenderer);

	return 0;
}

GAME_API int UpdateEvent(lua_State* pLuaVM)
{
	lua_pushboolean(pLuaVM, SDL_PollEvent(&g_event));
	g_bIsQuit = g_event.type == SDL_QUIT;

	return 1;
}

GAME_API int CheckEvent(lua_State* pLuaVM)
{
	auto itorEvent = mapEvent.find(luaL_checkstring(pLuaVM, 1));

	lua_pushboolean(pLuaVM, itorEvent != mapEvent.end() && itorEvent->second == g_event.type);

	return 1;
}

GAME_API int GetDanmuUserName(lua_State* pLuaVM)
{
	lua_pushstring(pLuaVM, (char*)g_event.user.data1);

	return 1;
}

GAME_API int GetDanmuContent(lua_State* pLuaVM)
{
	lua_pushstring(pLuaVM, (char*)g_event.user.data2);

	return 1;
}

#undef main
int main(int argc, char** argv)
{
	// for console debug
	system("chcp 65001");

	int iPort;
	std::string strWindowTitle;
	int iWindowWidth, iWindowHeight;
	bool bIsFullScreen;
	httplib::Server server;
	bool bHasServerRan = false;
	luaL_Reg aryRegCFuncs[] = { 
		{ "SetDrawColor",		SetDrawColor },
		{ "ClearScreen",		ClearScreen },
		{ "UpdateScreen",		UpdateScreen },
		{ "UpdateEvent",		UpdateEvent },
		{ "CheckEvent",			CheckEvent },
		{ "GetDanmuUserName",	GetDanmuUserName },
		{ "GetDanmuContent",	GetDanmuContent },
	};

	SDL_Init(SDL_INIT_EVERYTHING);

	// start process config file

	std::ifstream fConfig("config.json");
	if (!fConfig.good())
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Config Error",
			"failed to open config.json",
			g_pWindow
		);
		return -1;
	}

	std::stringstream ssConfigContent;
	ssConfigContent << fConfig.rdbuf();
	fConfig.close(); fConfig.clear();
	cJSON* pJSONConfig = cJSON_Parse(ssConfigContent.str().c_str());
	if (!pJSONConfig)
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Config Error",
			std::string("json parse error before: ")
				.append(cJSON_GetErrorPtr()).c_str(),
			g_pWindow
		);
		ssConfigContent.clear();
		return -1;
	}
	ssConfigContent.clear();

	cJSON* pJSONPort = cJSON_GetObjectItem(pJSONConfig, "port");
	cJSON* pJSONWindowTitle = cJSON_GetObjectItem(pJSONConfig, "window-title");
	cJSON* pJSONWindowSize = cJSON_GetObjectItem(pJSONConfig, "window-size");
	cJSON* pJSONFullScreen = cJSON_GetObjectItem(pJSONConfig, "full-screen");
	if (!(pJSONPort && pJSONPort->type == cJSON_Number 
		&& pJSONWindowTitle && pJSONWindowTitle->type == cJSON_String
		&& pJSONWindowSize && pJSONWindowSize->type == cJSON_Object
		&& pJSONFullScreen && (pJSONFullScreen->type == cJSON_True || pJSONFullScreen->type == cJSON_False)))
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Config Error",
			std::string("incomplete config properties").c_str(),
			g_pWindow
		);
		cJSON_Delete(pJSONConfig);
		return -1;
	}

	cJSON* pJSONWindowWidth = cJSON_GetObjectItem(pJSONWindowSize, "width");
	cJSON* pJSONWindowHeight = cJSON_GetObjectItem(pJSONWindowSize, "height");
	if (!(pJSONWindowWidth && pJSONWindowWidth->type == cJSON_Number
		&& pJSONWindowHeight && pJSONWindowHeight->type == cJSON_Number))
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Config Error",
			std::string("incomplete window size config properties").c_str(),
			g_pWindow
		);
		cJSON_Delete(pJSONConfig);
		return -1;
	}

	iPort = pJSONPort->valueint;
	strWindowTitle = pJSONWindowTitle->valuestring;
	iWindowWidth = pJSONWindowWidth->valueint;
	iWindowHeight = pJSONWindowHeight->valueint;
	bIsFullScreen = pJSONFullScreen->valueint;

	cJSON_Delete(pJSONConfig);

	g_u32EventTypeEnter = SDL_RegisterEvents(2);
	g_u32EventTypeMessage = g_u32EventTypeEnter + 1;

	mapEvent.insert(std::make_pair("ENTER", g_u32EventTypeEnter));
	mapEvent.insert(std::make_pair("MESSAGE", g_u32EventTypeMessage));

	server.Post("/enter", [](const httplib::Request& req, httplib::Response& res) {
		// req.body like this: "{\"username\":\"Voidmatrix\"}"
		std::string strJSONCopy = req.body;
		strJSONCopy.replace(0, 1, "").replace(strJSONCopy.size() - 1, 1, "");
		std::string::size_type szPos = 0;
		while ((szPos = strJSONCopy.find('\\')) != std::string::npos)
			strJSONCopy.replace(szPos, 1, "");
		// current strJSONCopy like this: {"username":"Voidmatrix"}
		cJSON* pJSON = cJSON_Parse(strJSONCopy.c_str());
		cJSON* pJSONUsername = cJSON_GetObjectItem(pJSON, "username");
		if (pJSONUsername)
		{
			SDL_Event event; SDL_zero(event);
			event.type = g_u32EventTypeEnter;
			event.user.data1 = (char*)malloc(sizeof(char) * (strlen(pJSONUsername->valuestring) + 1));
			if (event.user.data1)
			{
				strcpy((char*)event.user.data1, pJSONUsername->valuestring);
				SDL_PushEvent(&event);
			}
		}
		cJSON_Delete(pJSON);
	});

	server.Post("/message", [](const httplib::Request& req, httplib::Response& res) {
		// process raw json string like before
		std::string strJSONCopy = req.body;
		strJSONCopy.replace(0, 1, "").replace(strJSONCopy.size() - 1, 1, "");
		std::string::size_type szPos = 0;
		while ((szPos = strJSONCopy.find('\\')) != std::string::npos)
			strJSONCopy.replace(szPos, 1, "");
		cJSON* pJSON = cJSON_Parse(strJSONCopy.c_str());
		cJSON* pJSONUsername = cJSON_GetObjectItem(pJSON, "username");
		cJSON* pJSONMessage = cJSON_GetObjectItem(pJSON, "message");
		if (pJSONUsername && pJSONMessage)
		{
			SDL_Event event; SDL_zero(event);
			event.type = g_u32EventTypeMessage;
			event.user.data1 = (char*)malloc(sizeof(char) * (strlen(pJSONUsername->valuestring) + 1));
			event.user.data2 = (char*)malloc(sizeof(char) * (strlen(pJSONMessage->valuestring) + 1));
			if (event.user.data1 && event.user.data2)
			{
				strcpy((char*)event.user.data1, pJSONUsername->valuestring);
				strcpy((char*)event.user.data2, pJSONMessage->valuestring);
				SDL_PushEvent(&event);
			}
			else
			{
				if (event.user.data1) free(event.user.data1);
				if (event.user.data2) free(event.user.data2);
			}
		}
		cJSON_Delete(pJSON);
	});

	std::thread tServer([&]() { server.listen("127.0.0.1", 25566); bHasServerRan = true; });

	// start engine logic code

	lua_State* pLuaVM = luaL_newstate();
	luaL_openlibs(pLuaVM);
	lua_gc(pLuaVM, LUA_GCINC, 100);

	for (size_t i = 0; i < sizeof(aryRegCFuncs) / sizeof(luaL_Reg); i++)
	{
		lua_pushcfunction(pLuaVM, aryRegCFuncs[i].func);
		lua_setglobal(pLuaVM, aryRegCFuncs[i].name);
	}

	g_pWindow = SDL_CreateWindow(
		strWindowTitle.c_str(),
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		iWindowWidth, iWindowHeight,
		bIsFullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_SHOWN
	);

	g_pRenderer = SDL_CreateRenderer(
		g_pWindow, -1, 
		SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED
	);

	if (luaL_dofile(pLuaVM, "GameScript.lua"))
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Script Error",
			lua_tostring(pLuaVM, -1),
			g_pWindow
		);
		g_bIsQuit = true;
	}

	while (!g_bIsQuit)
	{
		lua_getglobal(pLuaVM, "__MainUpdate");
		if (!lua_isfunction(pLuaVM, -1))
		{
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_ERROR,
				"Script Error",
				"function __MainUpdate is not defined",
				g_pWindow
			);
			break;
		}

		if (lua_pcall(pLuaVM, 0, 1, 0) != 0)
		{
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_ERROR,
				"Script Error",
				lua_tostring(pLuaVM, -1),
				g_pWindow
			);
			break;
		}

		g_bIsQuit = g_bIsQuit || !lua_toboolean(pLuaVM, -1);

		lua_pop(pLuaVM, 1);
	}

	while (!bHasServerRan);

	server.stop(); tServer.join();

	return 0;
}