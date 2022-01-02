#include <httplib.h>
#include <cJSON.h>
#include <SDL.h>
#include <lua.hpp>

#include <thread>
#include <string>
#include <fstream>
#include <sstream>

Uint32 g_u32EventTypeEnter, g_u32EventTypeMessage;

SDL_Window* g_pWindow = nullptr;
SDL_Renderer* g_pRenderer = nullptr;

SDL_Event g_event;

#undef main
int main(int argc, char** argv)
{
	// for console debug
	system("chcp 65001");

	SDL_Init(SDL_INIT_EVERYTHING);

	int iPort;
	std::string strWindowTitle;
	int iWindowWidth, iWindowHeight;
	bool bIsFullScreen;

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

	httplib::Server server;

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

	bool bHasServerRan = false;

	std::thread tServer([&]() { server.listen("127.0.0.1", 25566); bHasServerRan = true; });

	// start engine logic code

	lua_State* pLuaVM = luaL_newstate();
	luaL_openlibs(pLuaVM);
	lua_gc(pLuaVM, LUA_GCINC, 100);

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
		return -1;
	}

	bool bIsQuit = false;

	while (!bIsQuit)
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
			return -1;
		}

		if (lua_pcall(pLuaVM, 0, 1, 0) != 0)
		{
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_ERROR,
				"Script Error",
				lua_tostring(pLuaVM, -1),
				g_pWindow
			);
			return -1;
		}

		bIsQuit = !lua_toboolean(pLuaVM, -1);

		lua_pop(pLuaVM, 1);

		/*while (SDL_PollEvent(&g_event))
		{
			if (event.type == g_u32EventTypeEnter)
			{
				std::cout << "Enter: " << (char*)event.user.data1 << std::endl;
			}
			else if (event.type == g_u32EventTypeMessage)
			{
				std::cout << (char*)event.user.data1 << " say: " << (char*)event.user.data2 << std::endl;
			}
		}*/
	}

	while (!bHasServerRan);

	server.stop(); tServer.join();

	return 0;
}