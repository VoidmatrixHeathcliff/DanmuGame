<div align="center">
    <p align="center">
        <img align="center" src="doc/image/bilibili-fill-live.png" alt="logo" width="200">
    </p>
    <h1 align="center">DanmuGame</h1>
    <p align="center">Bilibili 直播间弹幕控制游戏快速开发框架</p>
    <p align="center">
        <a href="https://github.com/VoidmatrixHeathcliff/DanmuGame/stargazers">
            <img src="https://img.shields.io/github/stars/VoidmatrixHeathcliff/DanmuGame.svg?style=flat&labelColor=e49e61">
        </a>
        <a href="https://github.com/VoidmatrixHeathcliff/DanmuGame/network/members">
            <img src="https://img.shields.io/github/forks/VoidmatrixHeathcliff/DanmuGame.svg?&color=green&style=flat-square">
        </a>
        <a href="https://jq.qq.com/?_wv=1027&k=SFCIfMx6" target="_blank">
            <img src="https://img.shields.io/static/v1?label=chatting&message=QQ&color=blue&style=flat-square">
        </a>
        <a href="https://github.com/VoidmatrixHeathcliff/DanmuGame/graphs/contributors" target="_blank">
            <img src="https://img.shields.io/github/contributors/VoidmatrixHeathcliff/DanmuGame">
        </a>
        <a href="https://github.com/VoidmatrixHeathcliff/DanmuGame/issuese" target="_blank">
            <img src="https://img.shields.io/static/v1?label=feedback&message=Issues&color=pink&style=flat-square">
        </a>
        <a href="https://github.com/VoidmatrixHeathcliff/DanmuGame/blob/main/LICENSE" target="_blank">
            <img src="https://img.shields.io/github/license/VoidmatrixHeathcliff/DanmuGame?style=flat-square">
        </a>
       <img src="https://img.shields.io/github/commit-activity/m/VoidmatrixHeathcliff/DanmuGame">
    </p>
    </br>
</div>

> 本项目基于 [bilibili-live-ws](https://github.com/simon300000/bilibili-live-ws) 开发，在此特别感谢为本项目开拓道路的大佬们 ~

## 程序结构及流程

![程序结构及流程](doc/image/DanmuGame-framework.jpg)

## 项目依赖项

DanmuUtil 直播间监听程序依赖于 `bilibili-live-ws` 等库，可以直接通过 `npm install` 安装相关依赖；在 Release 版本中，DanmuUtil 程序已打包为 `.exe` 形式，方便无 Node.js 环境的设备开箱即用。

DanmuGame 游戏框架和 VisualDebugTool 调试器项目提供了基于 VisualStudio 2019 的 x64 版本项目工程，可以直接拉取到本地进行编译，如需在其他编译器环境或其他平台下编译，请先编译并配置如下依赖库：
| 库名称                  | 网址                                                                                     | 协议                |
|-------------------------|------------------------------------------------------------------------------------------|---------------------|
| **Lua - 5.4.0**         | [http://www.lua.org/](http://www.lua.org/)                                               | MIT                 |
| **SDL - 2.0.14**        | [https://www.libsdl.org/](https://www.libsdl.org/)                                       | zlib license        |
| **SDL_image - 2.0.5**   | [https://www.libsdl.org/projects/SDL_image/](https://www.libsdl.org/projects/SDL_image/) | zlib license        |
| **SDL_gfx - 1.0.4**     | [https://sourceforge.net/projects/sdlgfx/](https://sourceforge.net/projects/sdlgfx/)     | zlib/libpng license |
| **SDL_mixer - 2.0.4**   | [https://www.libsdl.org/projects/SDL_mixer/](https://www.libsdl.org/projects/SDL_mixer/) | zlib license        |
| **SDL_ttf - 2.0.15**    | [https://www.libsdl.org/projects/SDL_ttf/](https://www.libsdl.org/projects/SDL_ttf/)     | zlib license        |
| **cpp-httplib - 0.8.4** | [https://github.com/yhirose/cpp-httplib/](https://github.com/yhirose/cpp-httplib/)       | MIT                 |
| **cJSON - 1.7.14**      | [https://github.com/DaveGamble/cJSON/](https://github.com/DaveGamble/cJSON/)             | MIT                 |
| **imgui - 1.86**        | [https://github.com/ocornut/imgui/](https://github.com/ocornut/imgui/)                   | MIT                 |

## 快速上手

发布版本中提供了开箱即用的程序，首先在 `config.json` 中配置好直播间信息及游戏设置（详细配置内容见稍后描述），然后启动 DanmuUtil 程序，当程序显示如下内容时证明启动并连接成功：
![屏幕截图](doc/image/screenshot.png)

接下来启动 `DanmuGame.exe`，程序将自动加载并执行 `GameScript.lua` 中的脚本内容，调整直播画面到游戏窗口即可。

## 配置文件 `Config.json` 详解
+ `room`：需要监听的直播间房间号（完整房间号，非短房间号）
+ `port`：DanmuUtil 程序和 DanmuGame 游戏框架在本地通信的端口号
+ `window-title`：游戏窗口标题文本
+ `window-size`：游戏窗口尺寸
    - `width`：游戏窗口宽度（单位：像素）
    - `height`：游戏窗口高度（单位：像素）
+ `full-screen`：游戏窗口全屏（将覆盖游戏窗口尺寸设置）
+ `max-fps`：游戏最高帧率，设置过大的最高帧率可能导致实际帧率不稳定（建议区间：60 ~ 144）
+ `linear-filtering`：对游戏画面开启线性过滤抗锯齿（某些需要对图片素材缩放的像素风游戏可能不适合开启）

## 游戏脚本编写及 API 详解

`GameScript.lua` 将作为游戏逻辑的入口脚本进行加载运行，使用 `Lua 5.4` 版本语法编写，请确保已在脚本环境中声明 `__EventHandler` 函数和 `__MainUpdate` 函数，这两个函数会分别在 **直播间事件更新**（如弹幕发送）和 **游戏画面帧更新** 时被调用：
+ 对于函数 `__EventHandler(event, data)`，第一个参数 `event` 是代表当前事件类型的字符串，第二个参数包含了事件相关数据，可能的值如下：
    | 值          | 事件         | 数据                                                 |
    |-------------|--------------|------------------------------------------------------|
    | `"ENTER"`   | 观众进入房间 | `data.username` 观众昵称                             |
    | `"MESSAGE"` | 观众发送弹幕 | `data.username` 观众昵称<br>`data.username` 弹幕内容 |
+ 对于函数 `__MainUpdate()`，程序会在每次尝试刷新游戏画面时调用此函数，可以在函数内编写游戏画面渲染相关的逻辑；

---

***以下内容正在维护 ...***

<details>
    <summary>🕹 游戏控制相关 API</summary>

### QuitGame()
+ **功能：** 退出游戏
+ **参数简介：** 无
+ **返回值简介：** 无

</details>

<details>
    <summary>🖥 游戏窗口相关 API</summary>

| 函数名                                                      | 简介                         |
|:------------------------------------------------------------|:-----------------------------|
| [ETHX_InitWindow()](#ETHX_InitWindow)                       | 初始化 EtherX 并创建窗口     |
| [ETHX_QuitWindow()](#ETHX_QuitWindow)                       | 退出 EtherX 并关闭窗口       |
| [ETHX_ShowMessageBox()](#ETHX_ShowMessageBox)               | 显示信息提示框               |
| [ETHX_ShowConfirmMessageBox()](#ETHX_ShowConfirmMessageBox) | 显示信息提示确认窗口         |
| [ETHX_SetWindowTitle()](#ETHX_SetWindowTitle)               | 重新设置窗口标题             |
| [ETHX_GetWindowTitle()](#ETHX_GetWindowTitle)               | 获取窗口标题                 |
| [ETHX_SetWindowFullscreen()](#ETHX_SetWindowFullscreen)     | 设置窗口是否全屏             |
| [ETHX_SetWindowSize()](#ETHX_SetWindowSize)                 | 设置窗口大小                 |
| [ETHX_GetWindowSize()](#ETHX_GetWindowSize)                 | 获取窗口大小                 |
| [ETHX_GetWindowSize_HDPI()](#ETHX_GetWindowSize_HDPI)       | 获取高分辨率下窗口的实际大小 |
| [ETHX_SetWindowIcon()](#ETHX_SetWindowIcon)                 | 设置窗口图标                 |
| [ETHX_ClearWindow()](#ETHX_ClearWindow)                     | 清空窗口内容                 |
| [ETHX_UpdateWindow()](#ETHX_UpdateWindow)                   | 刷新窗口                     |

</details>

<details>
    <summary>🖼 游戏画面相关 API</summary>

| 函数名                                            | 简介                               |
|:--------------------------------------------------|:-----------------------------------|
| [ETHX_LoadImage()](#ETHX_LoadImage)               | 加载图像对象                       |
| [ETHX_SetImageColorKey()](#ETHX_SetImageColorKey) | 设置图像对象指定的透明颜色是否启用 |
| [ETHX_SetImageAplha()](#ETHX_SetImageAplha)       | 设置图像透明度                     |
| [ETHX_GetImageSize()](#ETHX_GetImageSize)         | 获取图像尺寸                       |
| [ETHX_DrawImage()](#ETHX_DrawImage)               | 绘制图像                           |
| [ETHX_SetDrawColor()](#ETHX_SetDrawColor)         | 设置绘图颜色                       |
| [ETHX_GetDrawColor()](#ETHX_GetDrawColor)         | 获取当前绘图颜色                   |
| [ETHX_DrawPoint()](#ETHX_DrawPoint)               | 绘制点                             |
| [ETHX_DrawLine()](#ETHX_DrawLine)                 | 绘制线段                           |
| [ETHX_DrawRectangle()](#ETHX_DrawRectangle)       | 绘制矩形                           |
| [ETHX_DrawCircle()](#ETHX_DrawCircle)             | 绘制圆                             |
| [ETHX_DrawEllipse()](#ETHX_DrawEllipse)           | 绘制椭圆                           |
| [ETHX_DrawPie()](#ETHX_DrawPie)                   | 绘制扇形                           |
| [ETHX_DrawTriangle()](#ETHX_DrawTriangle)         | 绘制三角形                         |
| [ETHX_LoadFont()](#ETHX_LoadFont)                 | 加载字体对象                       |
| [ETHX_SetFontStyle()](#ETHX_SetFontStyle)         | 设置字体样式                       |
| [ETHX_GetFontStyle()](#ETHX_GetFontStyle)         | 获取字体样式                       |
| [ETHX_GetTextSize()](#ETHX_GetTextSize)           | 获取指定字体的文本尺寸             |
| [ETHX_DrawText()](#ETHX_DrawText)                 | 绘制文本内容                       |
| [ETHX_CreateTextImage()](#ETHX_CreateTextImage)   | 创建文本图像                       |

</details>

<details>
    <summary>🎼 音乐音效相关 API</summary>

| 函数名                                              | 简介                     |
|:----------------------------------------------------|:-------------------------|
| [ETHX_LoadMusic()](#ETHX_LoadMusic)                 | 加载音乐对象             |
| [ETHX_PlayMusic()](#ETHX_PlayMusic)                 | 播放音乐对象             |
| [ETHX_StopMusic()](#ETHX_StopMusic)                 | 显停止正在播放的音乐     |
| [ETHX_CheckMusicPlaying()](#ETHX_CheckMusicPlaying) | 检查音乐是否正在播放     |
| [ETHX_PauseMusic()](#ETHX_PauseMusic)               | 暂停正在播放的音乐       |
| [ETHX_ResumeMusic()](#ETHX_ResumeMusic)             | 恢复播放暂停状态的音乐   |
| [ETHX_RewindMusic()](#ETHX_RewindMusic)             | 将当前音乐重新从起点播放 |
| [ETHX_SetMusicVolume()](#ETHX_SetMusicVolume)       | 设置音乐播放的音量       |
| [ETHX_GetMusicVolume()](#ETHX_GetMusicVolume)       | 获取音乐播放的音量       |
| [ETHX_LoadSound()](#ETHX_LoadSound)                 | 加载音效对象             |
| [ETHX_PlaySound()](#ETHX_PlaySound)                 | 播放音效对象             |
| [ETHX_SetSoundVolume()](#ETHX_SetSoundVolume)       | 设置音效播放的音量       |
| [ETHX_GetSoundVolume()](#ETHX_GetSoundVolume)       | 获取音效播放的音量       |

</details>

<details>
    <summary>⏲ 时间控制相关 API</summary>

| 函数名                                  | 简介                         |
|:----------------------------------------|:-----------------------------|
| [ETHX_Sleep()](#ETHX_Sleep)             | 暂停程序指定时长             |
| [ETHX_GetInitTime()](#ETHX_GetInitTime) | 获取程序从初始化到现在的时长 |

</details>

<details>
    <summary>📌 数据存储相关 API</summary>

+ [窗口坐标系](#window-coordinate-system)
+ [图像坐标系](#image-coordinate-system)
+ [字符串编码](#string-encoding)

</details>