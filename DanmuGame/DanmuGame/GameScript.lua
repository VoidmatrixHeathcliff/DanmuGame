function __MainUpdate()

    SetDrawColor({r = 200, g = 196, b = 60, a = 255})
    ClearScreen()

    while UpdateEvent() do
        if CheckEvent("ENTER") then
            print("["..GetDanmuUserName().."] Enter!")
        elseif CheckEvent("MESSAGE") then
            print("["..GetDanmuUserName().."] "..GetDanmuContent())
        end
    end

    UpdateScreen()

    return true
end