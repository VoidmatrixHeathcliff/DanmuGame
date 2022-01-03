#include "GameAPI.h"

// most of the following API-implementations are copied from EtherEngineRefactor project
// GitHub Link: https://github.com/VoidmatrixHeathcliff/EtherEngineRefactor

GAME_API int QuitGame(lua_State* pLuaVM)
{
	SDL_Event event; SDL_zero(event);
	event.type = SDL_QUIT;
	SDL_PushEvent(&event);

	return 0;
}

GAME_API int SetWindowTitle(lua_State* pLuaVM)
{
	SDL_SetWindowTitle(g_pWindow, luaL_checkstring(pLuaVM, 1));

	return 0;
}

GAME_API int GetWindowTitle(lua_State* pLuaVM)
{
	lua_pushstring(pLuaVM, SDL_GetWindowTitle(g_pWindow));

	return 1;
}

GAME_API int GetWindowSize(lua_State* pLuaVM)
{
	int _width = 0, _height = 0;
	SDL_GetRendererOutputSize(g_pRenderer, &_width, &_height);
	lua_pushinteger(pLuaVM, _width); lua_pushinteger(pLuaVM, _height);

	return 2;
}

GAME_API int SetDrawColor(lua_State* pLuaVM)
{
	SDL_Color _color; CheckColor(pLuaVM, 1, _color);

	if (_color.a != 255) SDL_SetRenderDrawBlendMode(g_pRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(g_pRenderer, _color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int GetDrawColor(lua_State* pLuaVM)
{
	SDL_Color _color;
	SDL_GetRenderDrawColor(g_pRenderer, &(_color.r), &(_color.g), &(_color.b), &(_color.a));

	lua_createtable(pLuaVM, 0, 4);
	lua_pushstring(pLuaVM, "r"); lua_pushinteger(pLuaVM, _color.r); lua_rawset(pLuaVM, -3);
	lua_pushstring(pLuaVM, "g"); lua_pushinteger(pLuaVM, _color.g); lua_rawset(pLuaVM, -3);
	lua_pushstring(pLuaVM, "b"); lua_pushinteger(pLuaVM, _color.b); lua_rawset(pLuaVM, -3);
	lua_pushstring(pLuaVM, "a"); lua_pushinteger(pLuaVM, _color.a); lua_rawset(pLuaVM, -3);

	return 1;
}

GAME_API int DrawPoint(lua_State* pLuaVM)
{
	SDL_Point _point; CheckPoint(pLuaVM, 1, _point);

	SDL_RenderDrawPoint(g_pRenderer, _point.x, _point.y);

	return 0;
}

GAME_API int DrawLine(lua_State* pLuaVM)
{
	SDL_Point _point_1, _point_2;
	CheckPoint(pLuaVM, 1, _point_1);
	CheckPoint(pLuaVM, 2, _point_2);

	SDL_Color _color;
	SDL_GetRenderDrawColor(g_pRenderer, &(_color.r), &(_color.g), &(_color.b), &(_color.a));
	if (lua_gettop(pLuaVM) < 3)
		aalineRGBA(g_pRenderer, _point_1.x, _point_1.y, _point_2.x, _point_2.y,
			_color.r, _color.g, _color.b, _color.a);
	else
		thickLineRGBA(g_pRenderer, _point_1.x, _point_1.y, _point_2.x, _point_2.y,
			(Uint8)luaL_checknumber(pLuaVM, 3), _color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int DrawRectangle(lua_State* pLuaVM)
{
	SDL_Rect _rect; CheckRect(pLuaVM, 1, _rect);

	if (!lua_toboolean(pLuaVM, 2))
		SDL_RenderDrawRect(g_pRenderer, &_rect);
	else
		SDL_RenderFillRect(g_pRenderer, &_rect);

	return 0;
}

GAME_API int DrawRoundRectangle(lua_State* pLuaVM)
{
	SDL_Rect _rect; CheckRect(pLuaVM, 1, _rect);

	SDL_Color _color;
	SDL_GetRenderDrawColor(g_pRenderer, &(_color.r), &(_color.g), &(_color.b), &(_color.a));
	if (!lua_toboolean(pLuaVM, 3))
		roundedRectangleRGBA(g_pRenderer, _rect.x, _rect.y, _rect.x + _rect.w, _rect.y + _rect.h,
			(Sint16)luaL_checknumber(pLuaVM, 2), _color.r, _color.g, _color.b, _color.a);
	else
		roundedBoxRGBA(g_pRenderer, _rect.x, _rect.y, _rect.x + _rect.w, _rect.y + _rect.h,
			(Sint16)luaL_checknumber(pLuaVM, 2), _color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int DrawCircle(lua_State* pLuaVM)
{
	SDL_Point _point; CheckPoint(pLuaVM, 1, _point);

	SDL_Color _color;
	SDL_GetRenderDrawColor(g_pRenderer, &(_color.r), &(_color.g), &(_color.b), &(_color.a));
	if (!lua_toboolean(pLuaVM, 3))
		aacircleRGBA(g_pRenderer, _point.x, _point.y, (Sint16)luaL_checknumber(pLuaVM, 2),
			_color.r, _color.g, _color.b, _color.a);
	else
		filledCircleRGBA(g_pRenderer, _point.x, _point.y, (Sint16)luaL_checknumber(pLuaVM, 2),
			_color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int DrawEllipse(lua_State* pLuaVM)
{
	SDL_Point _point; CheckPoint(pLuaVM, 1, _point);

	SDL_Color _color;
	SDL_GetRenderDrawColor(g_pRenderer, &(_color.r), &(_color.g), &(_color.b), &(_color.a));
	if (!lua_toboolean(pLuaVM, 4))
		aaellipseRGBA(g_pRenderer, _point.x, _point.y, (Sint16)luaL_checknumber(pLuaVM, 2),
			(Sint16)luaL_checknumber(pLuaVM, 3), _color.r, _color.g, _color.b, _color.a);
	else
		filledEllipseRGBA(g_pRenderer, _point.x, _point.y, (Sint16)luaL_checknumber(pLuaVM, 2),
			(Sint16)luaL_checknumber(pLuaVM, 3), _color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int DrawPie(lua_State* pLuaVM)
{
	SDL_Point _point; CheckPoint(pLuaVM, 1, _point);

	SDL_Color _color;
	SDL_GetRenderDrawColor(g_pRenderer, &(_color.r), &(_color.g), &(_color.b), &(_color.a));
	if (!lua_toboolean(pLuaVM, 5))
		pieRGBA(g_pRenderer, _point.x, _point.y, (Sint16)luaL_checknumber(pLuaVM, 2),
			(Sint16)luaL_checknumber(pLuaVM, 3), (Sint16)luaL_checknumber(pLuaVM, 4), _color.r, _color.g, _color.b, _color.a);
	else
		filledPieRGBA(g_pRenderer, _point.x, _point.y, (Sint16)luaL_checknumber(pLuaVM, 2),
			(Sint16)luaL_checknumber(pLuaVM, 3), (Sint16)luaL_checknumber(pLuaVM, 4), _color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int DrawTriangle(lua_State* pLuaVM)
{
	SDL_Point _point_1, _point_2, _point_3;
	CheckPoint(pLuaVM, 1, _point_1); CheckPoint(pLuaVM, 2, _point_2); CheckPoint(pLuaVM, 3, _point_3);

	SDL_Color _color;
	SDL_GetRenderDrawColor(g_pRenderer, &(_color.r), &(_color.g), &(_color.b), &(_color.a));
	if (!lua_toboolean(pLuaVM, 4))
		aatrigonRGBA(g_pRenderer, _point_1.x, _point_1.y, _point_2.x, _point_2.y, _point_3.x, _point_3.y,
			_color.r, _color.g, _color.b, _color.a);
	else
		filledTrigonRGBA(g_pRenderer, _point_1.x, _point_1.y, _point_2.x, _point_2.y, _point_3.x, _point_3.y,
			_color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int DrawPolygon(lua_State* pLuaVM)
{
	std::vector<Sint16> _vecx, _vecy;
	luaL_argexpected(pLuaVM, lua_istable(pLuaVM, 1), 1, LUA_TABLIBNAME);
	TraverseTable(
		pLuaVM, 1,
		[&]() -> bool
		{
			SDL_Point _point;

			luaL_argcheck(pLuaVM, lua_istable(pLuaVM, -1), 1, "elements expected table");

			lua_pushstring(pLuaVM, "x"); lua_rawget(pLuaVM, -2);
			luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1), 1, std::string("point at #")
				.append(std::to_string(lua_tonumber(pLuaVM, -2))).append(" ").append(ERRMSG_INVALIDMEMBER).append(" : x").c_str());
			_point.x = (int)lua_tonumber(pLuaVM, -1);
			lua_pop(pLuaVM, 1);

			lua_pushstring(pLuaVM, "y"); lua_rawget(pLuaVM, -2);
			luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1), 1, std::string("point at #")
				.append(std::to_string(lua_tonumber(pLuaVM, -2))).append(" ").append(ERRMSG_INVALIDMEMBER).append(" : y").c_str());
			_point.y = (int)lua_tonumber(pLuaVM, -1);
			lua_pop(pLuaVM, 1);

			_vecx.push_back(_point.x); _vecy.push_back(_point.y);

			return true;
		}
	);

	SDL_Color _color;
	SDL_GetRenderDrawColor(g_pRenderer, &(_color.r), &(_color.g), &(_color.b), &(_color.a));
	if (!lua_toboolean(pLuaVM, 2))
		aapolygonRGBA(g_pRenderer, &_vecx[0], &_vecy[0], (int)_vecx.size(),
			_color.r, _color.g, _color.b, _color.a);
	else
		filledPolygonRGBA(g_pRenderer, &_vecx[0], &_vecy[0], (int)_vecx.size(),
			_color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int DrawBezier(lua_State* pLuaVM)
{
	std::vector<Sint16> _vecx, _vecy;
	luaL_argexpected(pLuaVM, lua_istable(pLuaVM, 1), 1, LUA_TABLIBNAME);
	TraverseTable(
		pLuaVM, 1,
		[&]() -> bool
		{
			SDL_Point _point;

			luaL_argcheck(pLuaVM, lua_istable(pLuaVM, -1), 1, "elements expected table");

			lua_pushstring(pLuaVM, "x"); lua_rawget(pLuaVM, -2);
			luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1), 1, std::string("point at #")
				.append(std::to_string(lua_tonumber(pLuaVM, -2))).append(" ").append(ERRMSG_INVALIDMEMBER).append(" : x").c_str());
			_point.x = (int)lua_tonumber(pLuaVM, -1);
			lua_pop(pLuaVM, 1);

			lua_pushstring(pLuaVM, "y"); lua_rawget(pLuaVM, -2);
			luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1), 1, std::string("point at #")
				.append(std::to_string(lua_tonumber(pLuaVM, -2))).append(" ").append(ERRMSG_INVALIDMEMBER).append(" : y").c_str());
			_point.y = (int)lua_tonumber(pLuaVM, -1);
			lua_pop(pLuaVM, 1);

			_vecx.push_back(_point.x); _vecy.push_back(_point.y);

			return true;
		}
	);

	SDL_Color _color;
	SDL_GetRenderDrawColor(g_pRenderer, &(_color.r), &(_color.g), &(_color.b), &(_color.a));
	bezierRGBA(g_pRenderer, &_vecx[0], &_vecy[0], (int)_vecx.size(), (int)lua_tonumber(pLuaVM, 2),
		_color.r, _color.g, _color.b, _color.a);

	return 0;
}

GAME_API int LoadSprite(lua_State* pLuaVM)
{
	SDL_Surface* _pSurface = nullptr;

	if (_pSurface = IMG_Load(luaL_checkstring(pLuaVM, 1)))
	{
		SDL_Texture* _pTexture = nullptr;
		if (_pTexture = SDL_CreateTextureFromSurface(g_pRenderer, _pSurface))
			PushUserdata<SDL_Texture>(pLuaVM, _pTexture, METANAME_SPRITE);
		else
			lua_pushnil(pLuaVM);
		SDL_FreeSurface(_pSurface);
	}
	else
		lua_pushnil(pLuaVM);

	return 1;
}

GAME_API int GC_Sprite(lua_State* pLuaVM)
{
	SDL_Texture* _pTexture = ToUserdata<SDL_Texture>(pLuaVM, 1, METANAME_SPRITE);

	SDL_DestroyTexture(_pTexture); _pTexture = nullptr;

	return 0;
}

GAME_API int SetSpriteAlpha(lua_State* pLuaVM)
{
	SDL_Texture* _pTexture = ToUserdata<SDL_Texture>(pLuaVM, 1, METANAME_SPRITE);

	SDL_SetTextureBlendMode(_pTexture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(_pTexture, (Uint8)luaL_checknumber(pLuaVM, 2));

	return 0;
}

GAME_API int GetSpriteSize(lua_State* pLuaVM)
{
	SDL_Texture* _pTexture = ToUserdata<SDL_Texture>(pLuaVM, 1, METANAME_SPRITE);

	int _width = 0, _height = 0;
	SDL_QueryTexture(_pTexture, nullptr, nullptr, &_width, &_height);

	lua_pushinteger(pLuaVM, _width);
	lua_pushinteger(pLuaVM, _height);

	return 2;
}

GAME_API int RenderSprite(lua_State* pLuaVM)
{
	SDL_Texture* _pTexture = ToUserdata<SDL_Texture>(pLuaVM, 1, METANAME_SPRITE);
	SDL_Rect _rect_dst; CheckRect(pLuaVM, 2, _rect_dst);

	if (lua_gettop(pLuaVM) < 3)
		SDL_RenderCopy(g_pRenderer, _pTexture, nullptr, &_rect_dst);
	else
	{
		SDL_Rect _rect_src; CheckRect(pLuaVM, 3, _rect_src);
		SDL_RenderCopy(g_pRenderer, _pTexture, &_rect_src, &_rect_dst);
	}

	return 0;
}

GAME_API int RenderSpriteEx(lua_State* pLuaVM)
{
	SDL_Texture* _pTexture = ToUserdata<SDL_Texture>(pLuaVM, 1, METANAME_SPRITE);
	SDL_Rect _rect_dst; CheckRect(pLuaVM, 2, _rect_dst);

	if (lua_gettop(pLuaVM) < 3)
		SDL_RenderCopy(g_pRenderer, _pTexture, nullptr, &_rect_dst);
	else if (lua_gettop(pLuaVM) < 4)
	{
		SDL_Rect _rect_src; CheckRect(pLuaVM, 3, _rect_src);
		SDL_RenderCopy(g_pRenderer, _pTexture, &_rect_src, &_rect_dst);
	}
	else if (lua_gettop(pLuaVM) < 6)
	{
		SDL_Point _point_center; CheckPoint(pLuaVM, 4, _point_center);
		if (lua_isnil(pLuaVM, 3))
			SDL_RenderCopyEx(g_pRenderer, _pTexture, nullptr, &_rect_dst,
				luaL_checknumber(pLuaVM, 5), &_point_center, SDL_FLIP_NONE);
		else
		{
			SDL_Rect _rect_src; CheckRect(pLuaVM, 3, _rect_src);
			SDL_RenderCopyEx(g_pRenderer, _pTexture, &_rect_src, &_rect_dst,
				luaL_checknumber(pLuaVM, 5), &_point_center, SDL_FLIP_NONE);
		}
	}
	else
	{
		SDL_Point _point_center; CheckPoint(pLuaVM, 4, _point_center);
		luaL_argexpected(pLuaVM, lua_istable(pLuaVM, 6), 6, LUA_TABLIBNAME);
		SDL_RendererFlip _flags = SDL_FLIP_NONE; TraverseTable(
			pLuaVM, 6,
			[&]() -> bool
			{
				std::string strParam = lua_tostring(pLuaVM, -1);
				if (strParam == "H")
					_flags = (SDL_RendererFlip)(_flags | SDL_FLIP_HORIZONTAL);
				else if (strParam == "V")
					_flags = (SDL_RendererFlip)(_flags | SDL_FLIP_VERTICAL);
				else if (strParam == "N")
					_flags = (SDL_RendererFlip)(_flags | SDL_FLIP_NONE);
				else
					luaL_argerror(pLuaVM, 6, ERRMSG_INVALIDENUM);
				return true;
			}
		);
		if (lua_isnil(pLuaVM, 3))
			SDL_RenderCopyEx(g_pRenderer, _pTexture, nullptr, &_rect_dst,
				luaL_checknumber(pLuaVM, 5), &_point_center, _flags);
		else
		{
			SDL_Rect _rect_src; CheckRect(pLuaVM, 3, _rect_src);
			SDL_RenderCopyEx(g_pRenderer, _pTexture, &_rect_src, &_rect_dst,
				luaL_checknumber(pLuaVM, 5), &_point_center, _flags);
		}
	}

	return 0;
}

GAME_API int LoadFont(lua_State* pLuaVM)
{
	TTF_Font* _pFont = nullptr;

	if (_pFont = TTF_OpenFont(luaL_checkstring(pLuaVM, 1), (int)luaL_checknumber(pLuaVM, 2)))
		PushUserdata<TTF_Font>(pLuaVM, _pFont, METANAME_FONT);
	else
		lua_pushnil(pLuaVM);

	return 1;
}

GAME_API int GC_Font(lua_State* pLuaVM)
{
	TTF_Font* _pFont = ToUserdata<TTF_Font>(pLuaVM, 1, METANAME_FONT);

	TTF_CloseFont(_pFont); _pFont = nullptr;

	return 0;
}

GAME_API int GetFontStyle(lua_State* pLuaVM)
{
	TTF_Font* _pFont = ToUserdata<TTF_Font>(pLuaVM, 1, METANAME_FONT);

	lua_createtable(pLuaVM, 4, 0);
	int _style = TTF_GetFontStyle(_pFont);

	if (_style == TTF_STYLE_NORMAL)
	{
		lua_pushstring(pLuaVM, "N");
		lua_rawseti(pLuaVM, -2, 1);

		return 1;
	}

	int _index = 1;
	if (_style & TTF_STYLE_BOLD)
	{
		lua_pushstring(pLuaVM, "B");
		lua_rawseti(pLuaVM, -2, _index); _index++;
	}
	if (_style & TTF_STYLE_ITALIC)
	{
		lua_pushstring(pLuaVM, "I");
		lua_rawseti(pLuaVM, -2, _index); _index++;
	}
	if (_style & TTF_STYLE_UNDERLINE)
	{
		lua_pushstring(pLuaVM, "U");
		lua_rawseti(pLuaVM, -2, _index); _index++;
	}
	if (_style & TTF_STYLE_STRIKETHROUGH)
	{
		lua_pushstring(pLuaVM, "S");
		lua_rawseti(pLuaVM, -2, _index); _index++;
	}

	return 1;
}

GAME_API int SetFontStyle(lua_State* pLuaVM)
{
	TTF_Font* _pFont = ToUserdata<TTF_Font>(pLuaVM, 1, METANAME_FONT);

	int _style = TTF_STYLE_NORMAL;

	luaL_argexpected(pLuaVM, lua_istable(pLuaVM, 2), 2, LUA_TABLIBNAME);
	TraverseTable(
		pLuaVM, 2,
		[&]() -> bool
		{
			std::string strParam = lua_tostring(pLuaVM, -1);
			if (strParam == "B") _style |= TTF_STYLE_BOLD;
			else if (strParam == "I") _style |= TTF_STYLE_ITALIC;
			else if (strParam == "U") _style |= TTF_STYLE_UNDERLINE;
			else if (strParam == "S") _style |= TTF_STYLE_STRIKETHROUGH;
			else if (strParam == "N") _style |= TTF_STYLE_NORMAL;
			else luaL_argerror(pLuaVM, 2, ERRMSG_INVALIDENUM);
			return true;
		}
	);

	TTF_SetFontStyle(_pFont, _style);

	return 0;
}

GAME_API int GetFontHeight(lua_State* pLuaVM)
{
	TTF_Font* _pFont = ToUserdata<TTF_Font>(pLuaVM, 1, METANAME_FONT);

	lua_pushinteger(pLuaVM, TTF_FontHeight(_pFont));

	return 1;
}

GAME_API int GetTextSize(lua_State* pLuaVM)
{
	TTF_Font* _pFont = ToUserdata<TTF_Font>(pLuaVM, 1, METANAME_FONT);

	int _width, _height;
	TTF_SizeUTF8(_pFont, luaL_checkstring(pLuaVM, 2), &_width, &_height);
	lua_pushinteger(pLuaVM, _width); lua_pushinteger(pLuaVM, _height);

	return 2;
}

GAME_API int CreateTextSprite(lua_State* pLuaVM)
{
	TTF_Font* _pFont = ToUserdata<TTF_Font>(pLuaVM, 1, METANAME_FONT);
	SDL_Color _color; CheckColor(pLuaVM, 3, _color);

	SDL_Surface* _pSurface = nullptr;
	if (_pSurface = TTF_RenderUTF8_Blended(_pFont, luaL_checkstring(pLuaVM, 2), _color))
	{
		SDL_Texture* _pTexture = nullptr;
		if (_pTexture = SDL_CreateTextureFromSurface(g_pRenderer, _pSurface))
			PushUserdata<SDL_Texture>(pLuaVM, _pTexture, METANAME_SPRITE);
		else
			lua_pushnil(pLuaVM);
		SDL_FreeSurface(_pSurface);
	}
	else
		lua_pushnil(pLuaVM);

	return 1;
}

GAME_API int Delay(lua_State* pLuaVM)
{
	SDL_Delay((Uint32)luaL_checknumber(pLuaVM, 1));

	return 0;
}

GAME_API int GetInitTime(lua_State* pLuaVM)
{
	lua_pushinteger(pLuaVM, SDL_GetTicks());

	return 1;
}

GAME_API int LoadMusic(lua_State* pLuaVM)
{
	Mix_Music* _pMusic = nullptr;

	if (_pMusic = Mix_LoadMUS(luaL_checkstring(pLuaVM, 1)))
		PushUserdata<Mix_Music>(pLuaVM, _pMusic, METANAME_MUSIC);
	else
		lua_pushnil(pLuaVM);

	return 1;
}

GAME_API int GC_Music(lua_State* pLuaVM)
{
	Mix_Music* _pMusic = ToUserdata<Mix_Music>(pLuaVM, 1, METANAME_MUSIC);

	Mix_FreeMusic(_pMusic); _pMusic = nullptr;

	return 0;
}

GAME_API int PlayMusic(lua_State* pLuaVM)
{
	Mix_Music* _pMusic = ToUserdata<Mix_Music>(pLuaVM, 1, METANAME_MUSIC);

	if (lua_gettop(pLuaVM) < 3)
		Mix_PlayMusic(_pMusic, (int)luaL_checknumber(pLuaVM, 2));
	else
		Mix_FadeInMusic(_pMusic, (int)luaL_checknumber(pLuaVM, 2),
			(int)luaL_checknumber(pLuaVM, 3));

	return 0;
}

GAME_API int StopMusic(lua_State* pLuaVM)
{
	if (lua_gettop(pLuaVM) < 1)
		Mix_HaltMusic();
	else
		Mix_FadeOutMusic((int)luaL_checknumber(pLuaVM, 1));

	return 0;
}

GAME_API int SetMusicPosition(lua_State* pLuaVM)
{
	Mix_SetMusicPosition(luaL_checknumber(pLuaVM, 1));

	return 0;
}

GAME_API int SetMusicVolume(lua_State* pLuaVM)
{
	Mix_VolumeMusic((int)luaL_checknumber(pLuaVM, 1));

	return 0;
}

GAME_API int GetMusicVolume(lua_State* pLuaVM)
{
	lua_pushinteger(pLuaVM, Mix_VolumeMusic(-1));

	return 1;
}

GAME_API int PauseMusic(lua_State* pLuaVM)
{
	Mix_PauseMusic();

	return 0;
}

GAME_API int ResumeMusic(lua_State* pLuaVM)
{
	Mix_ResumeMusic();

	return 0;
}

GAME_API int RewindMusic(lua_State* pLuaVM)
{
	Mix_RewindMusic();

	return 0;
}

GAME_API int CheckMusicPlaying(lua_State* pLuaVM)
{
	lua_pushboolean(pLuaVM, Mix_PlayingMusic());

	return 1;
}

GAME_API int CheckMusicPaused(lua_State* pLuaVM)
{
	lua_pushboolean(pLuaVM, Mix_PausedMusic());

	return 1;
}

GAME_API int GetMusicFadingType(lua_State* pLuaVM)
{
	switch (Mix_FadingMusic())
	{
	case MIX_NO_FADING:		lua_pushstring(pLuaVM, "N"); break;
	case MIX_FADING_OUT:	lua_pushstring(pLuaVM, "O"); break;
	case MIX_FADING_IN:		lua_pushstring(pLuaVM, "I"); break;
	default:				lua_pushstring(pLuaVM, "N"); break;
	}

	return 1;
}

GAME_API int LoadSound(lua_State* pLuaVM)
{
	Mix_Chunk* _pSound = nullptr;
	if (_pSound = Mix_LoadWAV(luaL_checkstring(pLuaVM, 1)))
		PushUserdata<Mix_Chunk>(pLuaVM, _pSound, METANAME_SOUND);
	else
		lua_pushnil(pLuaVM);

	return 1;
}

GAME_API int GC_Sound(lua_State* pLuaVM)
{
	Mix_Chunk* _pSound = ToUserdata<Mix_Chunk>(pLuaVM, 1, METANAME_SOUND);

	Mix_FreeChunk(_pSound); _pSound = nullptr;

	return 0;
}

GAME_API int PlaySound(lua_State* pLuaVM)
{
	Mix_Chunk* _pSound = ToUserdata<Mix_Chunk>(pLuaVM, 1, METANAME_SOUND);

	Mix_PlayChannel(-1, _pSound, (int)luaL_checknumber(pLuaVM, 2));

	return 0;
}

GAME_API int SetSoundVolume(lua_State* pLuaVM)
{
	Mix_Chunk* _pSound = ToUserdata<Mix_Chunk>(pLuaVM, 1, METANAME_SOUND);

	Mix_VolumeChunk(_pSound, (int)luaL_checknumber(pLuaVM, 2));

	return 0;
}

GAME_API int GetSoundVolume(lua_State* pLuaVM)
{
	Mix_Chunk* _pSound = ToUserdata<Mix_Chunk>(pLuaVM, 1, METANAME_SOUND);

	lua_pushinteger(pLuaVM, _pSound->volume);

	return 1;
}

GAME_API int LoadJSON(lua_State* pLuaVM)
{
	cJSON* _pJSONRoot = cJSON_Parse(luaL_checkstring(pLuaVM, 1));
	luaL_argcheck(pLuaVM, _pJSONRoot, 1, std::string("JSON load failed before: ").append(cJSON_GetErrorPtr()).c_str());
	ConvertJSONToLuaObject(_pJSONRoot, pLuaVM);
	cJSON_Delete(_pJSONRoot);

	return 1;
}

GAME_API int DumpJSON(lua_State* pLuaVM)
{
	cJSON* _pJSONRoot = ConvertLuaObjectToJSON(1, pLuaVM);
	char* _strJSON = lua_toboolean(pLuaVM, 2) ? cJSON_Print(_pJSONRoot) : cJSON_PrintUnformatted(_pJSONRoot);
	lua_pushstring(pLuaVM, _strJSON);
	cJSON_Delete(_pJSONRoot); free(_strJSON);

	return 1;
}