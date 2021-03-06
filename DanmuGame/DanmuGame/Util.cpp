#include "Util.h"

void ConvertJSONToLuaObject(cJSON* pJSONNode, lua_State* pLuaVM)
{
	switch (pJSONNode->type)
	{
	case cJSON_False:
		lua_pushboolean(pLuaVM, false);
		break;
	case cJSON_True:
		lua_pushboolean(pLuaVM, true);
		break;
	case cJSON_Number:
		lua_pushnumber(pLuaVM, pJSONNode->valuedouble);
		break;
	case cJSON_String:
		lua_pushstring(pLuaVM, pJSONNode->valuestring);
		break;
	case cJSON_Array:
	{
		lua_createtable(pLuaVM, cJSON_GetArraySize(pJSONNode), 0);
		cJSON* _pElement = nullptr; int _idx = 1;
		cJSON_ArrayForEach(_pElement, pJSONNode)
		{
			lua_pushinteger(pLuaVM, _idx++);
			ConvertJSONToLuaObject(_pElement, pLuaVM);
			lua_rawset(pLuaVM, -3);
		}
	}
	break;
	case cJSON_Object:
	{
		lua_createtable(pLuaVM, 0, cJSON_GetArraySize(pJSONNode));
		cJSON* _pElement = nullptr;
		cJSON_ArrayForEach(_pElement, pJSONNode)
		{
			lua_pushstring(pLuaVM, _pElement->string);
			ConvertJSONToLuaObject(_pElement, pLuaVM);
			lua_rawset(pLuaVM, -3);
		}
	}
	break;
	default:
		lua_pushnil(pLuaVM);
		break;
	}
}

cJSON* ConvertLuaObjectToJSON(int idx, lua_State* pLuaVM)
{
	cJSON* _pJSONNode = nullptr;
	switch (lua_type(pLuaVM, idx))
	{
	case LUA_TNIL:
		_pJSONNode = cJSON_CreateNull();
		break;
	case LUA_TBOOLEAN:
		_pJSONNode = cJSON_CreateBool(lua_toboolean(pLuaVM, idx));
		break;
	case LUA_TNUMBER:
		_pJSONNode = cJSON_CreateNumber(lua_tonumber(pLuaVM, idx));
		break;
	case LUA_TSTRING:
		_pJSONNode = cJSON_CreateString(lua_tostring(pLuaVM, idx));
		break;
	case LUA_TTABLE:
	{
		bool _hasStringKey = false;
		TraverseTable(
			pLuaVM, idx,
			[&]()->bool {
				return !(_hasStringKey = (lua_type(pLuaVM, -2) == LUA_TSTRING));
			}
		);
		_pJSONNode = _hasStringKey ? cJSON_CreateObject() : cJSON_CreateArray();

		if (_pJSONNode->type == cJSON_Array)
			TraverseTable(
				pLuaVM, idx,
				[&]()->bool {
					cJSON_AddItemToArray(_pJSONNode, ConvertLuaObjectToJSON(lua_gettop(pLuaVM), pLuaVM));
					return true;
				}
		);
		else
			TraverseTable(
				pLuaVM, idx,
				[&]()->bool {
					std::string _key;
					switch (lua_type(pLuaVM, -2))
					{
					case LUA_TSTRING:
						_key = lua_tostring(pLuaVM, -2);
						break;
					case LUA_TNUMBER:
						if (lua_isinteger(pLuaVM, -2))
							_key = std::to_string(lua_tointeger(pLuaVM, -2));
						else
						{
							_key = std::to_string(lua_tonumber(pLuaVM, -2));
							_key = _key.substr(0, _key.find_last_not_of('0') + 1);
						}
						break;
					default:
						luaL_argerror(pLuaVM, 1, std::string("JSON dump failed key type: ").append(lua_typename(pLuaVM, -2)).c_str());
						break;
					}
					cJSON_AddItemToObject(_pJSONNode, _key.c_str(), ConvertLuaObjectToJSON(lua_gettop(pLuaVM), pLuaVM));
					return true;
				}
		);
	}
	break;
	default:
		luaL_argerror(pLuaVM, 1, std::string("JSON dump failed value type: ").append(lua_typename(pLuaVM, idx)).c_str());
		break;
	}
	return _pJSONNode;
}