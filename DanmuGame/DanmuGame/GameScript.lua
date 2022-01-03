local SIZE_PLAYER <const> = 100
local SPEED_PLAYER <const> = 50
local COLOR_BACK <const> = {
    r = 139, g = 150,
    b = 141, a = 255
}
local COLOR_NAME <const> = {
    r = 225, g = 225,
    b = 225, a = 255
}
local FONT <const> = LoadFont("SourceHanMonoSC-Regular.otf", 25)

g_WidthWindow, g_HeightWindow = GetWindowSize()

local RECT_WINDOW <const> = {
    x = 0, y = 0,
    w = g_WidthWindow,
    h = g_HeightWindow
}

g_mapPlayers = {}

g_mapCommands = {
    ["创建角色"] = function(data)
        if not g_mapPlayers[data.username] then
            g_mapPlayers[data.username] = {
                rect = {
                    x = math.random(0, g_WidthWindow - SIZE_PLAYER),
                    y = math.random(0, g_HeightWindow - SIZE_PLAYER),
                    w = SIZE_PLAYER, h = SIZE_PLAYER
                },
                color = {
                    r = math.random(50, 205),
                    g = math.random(50, 205),
                    b = math.random(50, 205),
                    a = 255
                },
                name_sprite = CreateTextSprite(FONT, data.username, COLOR_NAME)
            }        
        end
    end,
    ["向上移动"] = function(data)
        if g_mapPlayers[data.username] then
            local player = g_mapPlayers[data.username]
            if player.rect.y >= SPEED_PLAYER then 
                player.rect.y = player.rect.y - SPEED_PLAYER 
            end
        end
    end,
    ["向下移动"] = function(data)
        if g_mapPlayers[data.username] then
            local player = g_mapPlayers[data.username]
            if player.rect.y <= g_HeightWindow - SPEED_PLAYER then 
                player.rect.y = player.rect.y + SPEED_PLAYER 
            end
        end
    end,
    ["向左移动"] = function(data)
        if g_mapPlayers[data.username] then
            local player = g_mapPlayers[data.username]
            if player.rect.x >= SPEED_PLAYER then 
                player.rect.x = player.rect.x - SPEED_PLAYER 
            end
        end
    end,
    ["向右移动"] = function(data)
        if g_mapPlayers[data.username] then
            local player = g_mapPlayers[data.username]
            if player.rect.x <= g_WidthWindow - SPEED_PLAYER then 
                player.rect.x = player.rect.x + SPEED_PLAYER
            end
        end
    end,
}

function __EventHandler(event, data)
    if event == "MESSAGE" then
        if g_mapCommands[data.message] then
            g_mapCommands[data.message](data)
        end
    end
end

function __MainUpdate()

    SetDrawColor(COLOR_BACK)
    DrawRectangle(RECT_WINDOW, true)

    for name, data in pairs(g_mapPlayers) do
        SetDrawColor(data.color)
        DrawRoundRectangle(data.rect, 5, true)
        RenderSprite(data.name_sprite, data.rect)
    end

end