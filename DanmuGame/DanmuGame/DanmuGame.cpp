#include "DanmuGame.h"

#ifdef __WIN32__
#undef PlaySound
#undef Sleep
#endif // __WIN32

#undef main 
int main(int argc, char** argv)
{
	// for console debug
	system("chcp 65001");

	lua_State*		pLuaVM = nullptr;
	int				iPort;
	std::string		strWindowTitle;
	int				iWindowWidth, iWindowHeight;
	bool			bIsFullScreen;
	int				iMaxFPS;
	bool			bIsLinearFiltering;
	httplib::Server server;
	bool			bHasServerRan = false;
	SDL_Event		event;
	bool			bIsQuit = false;
	Uint32			u32EventTypeEnter, u32EventTypeMessage;
	luaL_Reg		aryRegCFuncs[] = {
		{ "QuitGame",			QuitGame },

		{ "SetTitle",			SetWindowTitle },
		{ "GetTitle",			GetWindowTitle },
		{ "GetWindowSize",		GetWindowSize },

		{ "SetDrawColor",		SetDrawColor },
		{ "GetDrawColor",		GetDrawColor },

		{ "DrawPoint",			DrawPoint },
		{ "DrawLine",			DrawLine },
		{ "DrawRectangle",		DrawRectangle },
		{ "DrawRoundRectangle",	DrawRoundRectangle },
		{ "DrawCircle",			DrawCircle },
		{ "DrawEllipse",		DrawEllipse },
		{ "DrawPie",			DrawPie },
		{ "DrawTriangle",		DrawTriangle },
		{ "DrawPolygon",		DrawPolygon },
		{ "DrawBezier",			DrawBezier },

		{ "LoadSprite",			LoadSprite },
		{ "SetSpriteAlpha",		SetSpriteAlpha },
		{ "GetSpriteSize",		GetSpriteSize },
		{ "RenderSprite",		RenderSprite },
		{ "RenderSpriteEx",		RenderSpriteEx },
		{ "LoadFont",			LoadFont },
		{ "GetFontStyle",		GetFontStyle },
		{ "SetFontStyle",		SetFontStyle },
		{ "GetFontHeight",		GetFontHeight },
		{ "GetTextSize",		GetTextSize },
		{ "CreateTextSprite",	CreateTextSprite },

		{ "Delay",				Delay },
		{ "GetInitTime",		GetInitTime },

		{ "LoadMusic",			LoadMusic },
		{ "PlayMusic",			PlayMusic },
		{ "StopMusic",			StopMusic },
		{ "SetMusicPosition",	SetMusicPosition },
		{ "SetMusicVolume",		SetMusicVolume },
		{ "GetMusicVolume",		GetMusicVolume },
		{ "PauseMusic",			PauseMusic },
		{ "ResumeMusic",		ResumeMusic },
		{ "RewindMusic",		RewindMusic },
		{ "CheckMusicPlaying",	CheckMusicPlaying },
		{ "CheckMusicPaused",	CheckMusicPaused },
		{ "GetMusicFadingType",	GetMusicFadingType },
		{ "LoadSound",			LoadSound },
		{ "PlaySound",			PlaySound },
		{ "SetSoundVolume",		SetSoundVolume },
		{ "GetSoundVolume",		GetSoundVolume },

		{ "LoadJSON",			LoadJSON },
		{ "DumpJSON",			DumpJSON },
	};
	MetaTableData	aryMetaTables[] = {
		{ METANAME_SPRITE,		GC_Sprite },
		{ METANAME_FONT,		GC_Font },
		{ METANAME_MUSIC,		GC_Music },
		{ METANAME_SOUND,		GC_Sound },
	};

	std::unordered_map<Uint32, EventHandlerParam> mapEvent;

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init(); IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP);
	Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

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
	cJSON* pJSONMaxFPS = cJSON_GetObjectItem(pJSONConfig, "max-fps");
	cJSON* pJSONLinearFiltering = cJSON_GetObjectItem(pJSONConfig, "linear-filtering");
	if (!(pJSONPort && pJSONPort->type == cJSON_Number
		&& pJSONWindowTitle && pJSONWindowTitle->type == cJSON_String
		&& pJSONWindowSize && pJSONWindowSize->type == cJSON_Object
		&& pJSONFullScreen && (pJSONFullScreen->type == cJSON_True || pJSONFullScreen->type == cJSON_False)
		&& pJSONMaxFPS && pJSONMaxFPS->type == cJSON_Number
		&& pJSONLinearFiltering && (pJSONLinearFiltering->type == cJSON_True || pJSONLinearFiltering->type == cJSON_False)))
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Config Error",
			std::string("incorrect config properties").c_str(),
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
			std::string("incorrect window size config properties").c_str(),
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
	iMaxFPS = pJSONMaxFPS->valueint;
	bIsLinearFiltering = pJSONLinearFiltering->valueint;

	cJSON_Delete(pJSONConfig);

	u32EventTypeEnter = SDL_RegisterEvents(2);
	u32EventTypeMessage = u32EventTypeEnter + 1;

	mapEvent.insert(std::pair<Uint32, EventHandlerParam>(u32EventTypeEnter, 
		{ "ENTER", [&]() {
			lua_createtable(pLuaVM, 0, 1);
			lua_pushstring(pLuaVM, "username");
			lua_pushstring(pLuaVM, (char*)event.user.data1);
			lua_rawset(pLuaVM, -3); 
		}}));
	mapEvent.insert(std::pair<Uint32, EventHandlerParam>(u32EventTypeMessage, 
		{ "MESSAGE", [&]() {
			lua_createtable(pLuaVM, 0, 2);
			lua_pushstring(pLuaVM, "username");
			lua_pushstring(pLuaVM, (char*)event.user.data1);
			lua_rawset(pLuaVM, -3);
			lua_pushstring(pLuaVM, "message");
			lua_pushstring(pLuaVM, (char*)event.user.data2);
			lua_rawset(pLuaVM, -3);
		}}));

	server.Post("/enter", [&](const httplib::Request& req, httplib::Response& res) {
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
			event.type = u32EventTypeEnter;
			event.user.data1 = (char*)malloc(sizeof(char) * (strlen(pJSONUsername->valuestring) + 1));
			if (event.user.data1)
			{
				strcpy((char*)event.user.data1, pJSONUsername->valuestring);
				SDL_PushEvent(&event);
			}
		}
		cJSON_Delete(pJSON);
	});

	server.Post("/message", [&](const httplib::Request& req, httplib::Response& res) {
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
			event.type = u32EventTypeMessage;
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

	std::thread tServer([&]() { server.listen("127.0.0.1", iPort); bHasServerRan = true; });

	// start engine logic code

	pLuaVM = luaL_newstate();
	luaL_openlibs(pLuaVM);
	lua_gc(pLuaVM, LUA_GCINC, 100);

	for (size_t i = 0; i < sizeof(aryRegCFuncs) / sizeof(luaL_Reg); i++)
	{
		lua_pushcfunction(pLuaVM, aryRegCFuncs[i].func);
		lua_setglobal(pLuaVM, aryRegCFuncs[i].name);
	}

	for (size_t i = 0; i < sizeof(aryMetaTables) / sizeof(MetaTableData); i++)
	{
		luaL_newmetatable(pLuaVM, aryMetaTables[i].name.c_str());

		lua_pushstring(pLuaVM, "__gc");
		lua_pushcfunction(pLuaVM, aryMetaTables[i].gc_func);
		lua_rawset(pLuaVM, -3);
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

	if (bIsLinearFiltering)
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	if (luaL_dofile(pLuaVM, "GameScript.lua"))
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_ERROR,
			"Script Error",
			lua_tostring(pLuaVM, -1),
			g_pWindow
		);
		bIsQuit = true;
	}

	while (!bIsQuit)
	{
		Uint32 u32TimeFrameStart = SDL_GetTicks();

		SDL_SetRenderDrawColor(g_pRenderer, 0, 0, 0, 255);
		SDL_RenderClear(g_pRenderer);

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				bIsQuit = true;
			else
			{
				auto itorEvent = mapEvent.find(event.type);
				if (itorEvent != mapEvent.end())
				{
					lua_getglobal(pLuaVM, "__EventHandler");
					if (!lua_isfunction(pLuaVM, -1))
					{
						SDL_ShowSimpleMessageBox(
							SDL_MESSAGEBOX_ERROR,
							"Script Error",
							"function __EventHandler is not defined",
							g_pWindow
						);
						bIsQuit = true;
						break;
					}

					lua_pushstring(pLuaVM, itorEvent->second.name.c_str());
					itorEvent->second.PushExtraData();

					if (lua_pcall(pLuaVM, 2, 0, 0) != 0)
					{
						SDL_ShowSimpleMessageBox(
							SDL_MESSAGEBOX_ERROR,
							"Script Error",
							lua_tostring(pLuaVM, -1),
							g_pWindow
						);
						bIsQuit = true;
						break;
					}
				}
			}
		}

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

		if (lua_pcall(pLuaVM, 0, 0, 0) != 0)
		{
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_ERROR,
				"Script Error",
				lua_tostring(pLuaVM, -1),
				g_pWindow
			);
			break;
		}

		SDL_RenderPresent(g_pRenderer);

		Uint32 u32TimeFrameEnd = SDL_GetTicks();

		if (u32TimeFrameEnd - u32TimeFrameStart < (Uint32)(1000 / iMaxFPS))
			SDL_Delay(1000 / iMaxFPS - (u32TimeFrameEnd - u32TimeFrameStart));
	}

	lua_close(pLuaVM);

	SDL_DestroyRenderer(g_pRenderer);
	SDL_DestroyWindow(g_pWindow);
	SDL_Quit();

	while (!server.is_running() && !bHasServerRan) SDL_Delay(25);

	server.stop(); tServer.join();

	return 0;
}