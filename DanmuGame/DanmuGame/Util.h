#ifndef _UTIL_H_
#define _UTIL_H_

#include "Macro.h"

#include <SDL.h>
#include <lua.hpp>
#include <cJSON.h>

#include <string>
#include <functional>

template <class T>
inline T* ToUserdata(lua_State* pLuaVM, int idx,
	const char* type_name)
{
	return (T*)(*(void**)luaL_checkudata(pLuaVM, idx, type_name));
}

template <class T>
inline void PushUserdata(lua_State* pLuaVM, T* obj,
	const char* type_name)
{
	T** ppUserdata = (T**)lua_newuserdata(pLuaVM, sizeof(T*));
	*ppUserdata = obj;
	luaL_getmetatable(pLuaVM, type_name);
	lua_setmetatable(pLuaVM, -2);
}

inline void CheckPoint(lua_State* pLuaVM, int idx, SDL_Point& point)
{
	luaL_argexpected(pLuaVM, lua_istable(pLuaVM, idx), idx, LUA_TABLIBNAME);

	lua_pushstring(pLuaVM, "x"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : x").c_str());
	point.x = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);

	lua_pushstring(pLuaVM, "y"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : y").c_str());
	point.y = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);
}

inline void CheckRect(lua_State* pLuaVM, int idx, SDL_Rect& rect)
{
	luaL_argexpected(pLuaVM, lua_istable(pLuaVM, idx), idx, LUA_TABLIBNAME);

	lua_pushstring(pLuaVM, "x"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : x").c_str());
	rect.x = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);

	lua_pushstring(pLuaVM, "y"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : y").c_str());
	rect.y = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);

	lua_pushstring(pLuaVM, "w"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : w").c_str());
	rect.w = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);

	lua_pushstring(pLuaVM, "h"); lua_rawget(pLuaVM, idx);
	luaL_argcheck(pLuaVM, lua_isnumber(pLuaVM, -1),
		idx, std::string(ERRMSG_INVALIDMEMBER).append(" : h").c_str());
	rect.h = (int)lua_tonumber(pLuaVM, -1);
	lua_pop(pLuaVM, 1);
}

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

// 'key' at index - 2 and 'value' at index - 1
inline void TraverseTable(lua_State* pLuaVM, int idx,
	std::function<bool()> callback)
{
	bool _flag = true;
	lua_pushnil(pLuaVM);
	while (_flag && lua_next(pLuaVM, idx)) {
		_flag = callback();
		if (!_flag) lua_pop(pLuaVM, 1);
		lua_pop(pLuaVM, 1);
	}
}

void ConvertJSONToLuaObject(cJSON* pJSONNode, lua_State* pLuaVM);
cJSON* ConvertLuaObjectToJSON(int idx, lua_State* pLuaVM);

#endif // !_UTIL_H_
