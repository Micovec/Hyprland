#include "Log.hpp"
#include "../defines.hpp"
#include "../Compositor.hpp"
#include "RollingLogFollow.hpp"

#include <fstream>
#include <iostream>

void Debug::init(const std::string& IS) {
    logFile = IS + (ISDEBUG ? "/hyprlandd.log" : "/hyprland.log");
    logOfs.open(logFile, std::ios::out | std::ios::app);
}

void Debug::close() {
    logOfs.close();
}

void Debug::log(LogLevel level, std::string str) {
    if (level == TRACE && !trace)
        return;

    if (shuttingDown)
        return;

    std::string coloredStr = str;
    switch (level) {
        case LOG:
            str        = "[LOG] " + str;
            coloredStr = str;
            break;
        case WARN:
            str        = "[WARN] " + str;
            coloredStr = "\033[1;33m" + str + "\033[0m"; // yellow
            break;
        case ERR:
            str        = "[ERR] " + str;
            coloredStr = "\033[1;31m" + str + "\033[0m"; // red
            break;
        case CRIT:
            str        = "[CRITICAL] " + str;
            coloredStr = "\033[1;35m" + str + "\033[0m"; // magenta
            break;
        case INFO:
            str        = "[INFO] " + str;
            coloredStr = "\033[1;32m" + str + "\033[0m"; // green
            break;
        case TRACE:
            str        = "[TRACE] " + str;
            coloredStr = "\033[1;34m" + str + "\033[0m"; // blue
            break;
        default: break;
    }

    rollingLog += str + "\n";
    if (rollingLog.size() > ROLLING_LOG_SIZE)
        rollingLog = rollingLog.substr(rollingLog.size() - ROLLING_LOG_SIZE);

    if (RollingLogFollow::Get().IsRunning())
        RollingLogFollow::Get().AddLog(str);

    if (!disableLogs || !**disableLogs) {
        // log to a file
        logOfs << str << "\n";
        logOfs.flush();
    }

    // log it to the stdout too.
    if (!disableStdout)
        std::cout << ((coloredLogs && !**coloredLogs) ? str : coloredStr) << "\n";
}
