#include <SDL.h>
#include <cJSON.h>
#include <httplib.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include <string>
#include <fstream>
#include <sstream>

#ifdef __WIN32__
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#endif // __WIN32__

SDL_Window* g_pWindow = nullptr;
SDL_Renderer* g_pRenderer = nullptr;

#undef main
int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);

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
	if (!(pJSONPort && pJSONPort->type == cJSON_Number))
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

	int iPort = pJSONPort->valueint;

	cJSON_Delete(pJSONConfig);

    httplib::Client client(std::string("http://127.0.0.1:").append(std::to_string(iPort)));
    client.set_default_headers({ { "content-type", "application/json" } });

    g_pWindow = SDL_CreateWindow("Bilibili直播间模拟调试器", 
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 375, 140, SDL_WINDOW_SHOWN);

    g_pRenderer = SDL_CreateRenderer(g_pWindow, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(g_pWindow);
    ImGui_ImplSDLRenderer_Init(g_pRenderer);
    io.Fonts->AddFontFromFileTTF("SourceHanMonoSC-Regular.otf", 22, 
        nullptr, io.Fonts->GetGlyphRangesChineseFull());

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowBorderSize = 1.0f; style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f; style.FrameRounding = 3.0f;

    bool bIsQuit = false;

    while (!bIsQuit)
    {
        Uint32 u32TimeFrameStart = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                bIsQuit = true;
        }

        SDL_SetRenderDrawColor(g_pRenderer, 35, 35, 35, 255);
        SDL_RenderClear(g_pRenderer);

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame(g_pWindow);
        ImGui::NewFrame();

        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove 
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos); ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGui::Begin("VisualDebugTool", nullptr, flags);

        ImGui::Text("事件类型："); ImGui::SameLine();
        static const char* items[] = { "ENTER - 用户进入", "MESSAGE - 发送弹幕" };
        static int item_current = 0;
        ImGui::Combo("##event_type", &item_current, items, IM_ARRAYSIZE(items));

        ImGui::Text("用户昵称："); ImGui::SameLine();
        static char strUsername[256] = "";
        ImGui::InputTextWithHint("##username", "data.username", strUsername, IM_ARRAYSIZE(strUsername));

        if (!item_current) ImGui::BeginDisabled();

        ImGui::Text("弹幕内容："); ImGui::SameLine();
        static char strMessage[256] = "";
        ImGui::InputTextWithHint("##message", "data.message", strMessage, IM_ARRAYSIZE(strMessage));

        if (!item_current) ImGui::EndDisabled();
        
        ImGui::Indent(160.0f);
        if (ImGui::Button("   发送   "))
        {
            // event - ENTER
            if (!item_current)
            {
                cJSON* pJSON = cJSON_CreateObject();
                cJSON_AddItemToObject(pJSON, "username", cJSON_CreateString(strUsername));
                // strRawJSON like this: {"username":"Voidmatrix"}
                char* strRawJSON = cJSON_PrintUnformatted(pJSON);
                std::string strJSON = strRawJSON;
                std::string::size_type szPos = -2;
                while ((szPos = strJSON.find('\"', szPos + 2)) != std::string::npos)
                    strJSON.replace(szPos, 1, "\\\"");
                strJSON = std::string("\"").append(strJSON).append("\"");
                // current strJSON like this: "{\"username\":\"Voidmatrix\"}"
                auto response = client.Post("/message", strJSON, "application/json");
                if (!response || response->status != 200)
                {
                    SDL_ShowSimpleMessageBox(
                        SDL_MESSAGEBOX_WARNING,
                        "Send Data Failed",
                        ("Reason: " + httplib::to_string(response.error())).c_str(),
                        g_pWindow
                    );
                }
                cJSON_Delete(pJSON); pJSON = nullptr;
                free(strRawJSON); strRawJSON = nullptr;
            }
            // event - MESSAGE
            else
            {
                cJSON* pJSON = cJSON_CreateObject();
                cJSON_AddItemToObject(pJSON, "username", cJSON_CreateString(strUsername));
                cJSON_AddItemToObject(pJSON, "message", cJSON_CreateString(strMessage));
                // process raw json string like before
                char* strRawJSON = cJSON_PrintUnformatted(pJSON);
                std::string strJSON = strRawJSON;
                std::string::size_type szPos = -2;
                while ((szPos = strJSON.find('\"', szPos + 2)) != std::string::npos)
                    strJSON.replace(szPos, 1, "\\\"");
                strJSON = std::string("\"").append(strJSON).append("\"");
                auto response = client.Post("/message", strJSON, "application/json");
                if (response->status != 200)
                {
                    SDL_ShowSimpleMessageBox(
                        SDL_MESSAGEBOX_WARNING,
                        ("Reason: " + httplib::to_string(response.error())).c_str(),
                        httplib::to_string(response.error()).append(" : ")
                        .append(std::to_string(response->status)).c_str(),
                        g_pWindow
                    );
                }
                cJSON_Delete(pJSON); pJSON = nullptr;
                free(strRawJSON); strRawJSON = nullptr;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(" 取消 ")) bIsQuit = true;
        ImGui::Unindent();

        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(g_pRenderer);

        Uint32 u32TimeFrameEnd = SDL_GetTicks();

        if (u32TimeFrameEnd - u32TimeFrameStart < 16)
            SDL_Delay(17 - (u32TimeFrameEnd - u32TimeFrameStart));
    }

	return 0;
}