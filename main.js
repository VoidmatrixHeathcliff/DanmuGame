const fs            = require('fs');
const util          = require('util');
const request       = require('request');
const sd            = require('silly-datetime');
const { LiveWS }    = require('bilibili-live-ws');

const __META_INFO = {
    author: "Voidmatrix",
    version: "202112001_alpha",
    feedback: "Voidmatrix@qq.com",
    special_thanks: "https://github.com/simon300000/bilibili-live-ws"
};

// type param: -1 - Error, 0 - Info, 1 - Warning
function __Log(type, content) {
    switch (type) {
        case 0:
            console.log(util.format("[INFO][%s]: %s", sd.format(new Date(), 'YYYY-MM-DD_HH:mm:ss'), content));
            break;
        case -1:
            console.log(util.format("\033[31m[ERRO][%s]: %s\033[0m", sd.format(new Date(), 'YYYY-MM-DD_HH:mm:ss'), content));
            break;
        case 1:
            console.log(util.format("\033[33m[WARN][%s]: %s\033[0m", sd.format(new Date(), 'YYYY-MM-DD_HH:mm:ss'), content));
            break;
        default:
            break;
    }
}

try {
    var config_file = fs.readFileSync('config.json');
} catch (error) {
    __Log(-1, "Failed to open file DanmuUtil.json.");
    process.exit();
}

try {
    var config_json = JSON.parse(config_file);
} catch (error) {
    __Log(-1, "Failed to parse configuration file.");
    process.exit();
}

const live = new LiveWS(config_json.room)

console.log(util.format(
    "\033[36mAuthor:\t\t%s\nVersion:\t%s\nFeedback:\t%s\nSpecialThanks:\t%s\033[0m",
    __META_INFO.author, __META_INFO.version, __META_INFO.feedback, __META_INFO.special_thanks));

__Log(0, util.format("Try to connect to the room: %d...", config_json.room));

live.on('open', () => __Log(0, 'Connection is established!'));

live.on('live', () => {
    __Log(0, 'Successfully logged in to the room.');
    live.on('DANMU_MSG', (data) => {
        let _url = util.format("http://127.0.0.1:%d/message", config_json.port);
        request({
            url: _url,
            method: "POST",
            json: true,
            headers: {
                "content-type": "application/json",
            },
            body: JSON.stringify({
                username : data.info[2][1],
                message: data.info[1]
            })
        }, function(error, response, body) {
            if (!error && response.statusCode == 200)
                __Log(0, util.format("Successfully forwarded data to [%s]", _url));
            else
                __Log(1, util.format("Failed to forward data to [%s]", _url));
        });
        __Log(0, util.format("[%s] %s", data.info[2][1], data.info[1]));
    });
    live.on('INTERACT_WORD', (data) => {
        let _url = util.format("http://127.0.0.1:%d/enter", config_json.port);
        request({
            url: _url,
            method: "POST",
            json: true,
            headers: {
                "content-type": "application/json",
            },
            body: JSON.stringify({
                username : data.data.uname
            })
        }, function(error, response, body) {
            if (!error && response.statusCode == 200)
                __Log(0, util.format("Successfully forwarded data to [%s]", _url));
            else
                __Log(1, util.format("Failed to forward data to [%s]", _url));
        });
        __Log(0, util.format("Welcome [%s] to the room!", data.data.uname));
    });
    live.on('close', () => __Log(-1, 'Room disconnected!'));
})