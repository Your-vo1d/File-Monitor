#include "ConsoleLog.h"
#include <iostream>
#include <algorithm>

ConsoleLog::ConsoleLog(bool logTime)
    : m_logTime(logTime)
{
}

void ConsoleLog::log(const std::string &data)
{
    if (m_logTime) {
        std::cout << QTime::currentTime().toString("HH:mm:ss").toStdString() << ' ';
    }
    std::cout << "[LOG] " << data << '\n';
    std::cout.flush();
}

std::string ConsoleLog::qint64ToString(qint64 value)
{
    if (value == 0) return "0";
    std::string result;
    bool neg = value < 0;
    qint64 n = neg ? -value : value;
    do {
        result += static_cast<char>('0' + (n % 10));
        n /= 10;
    } while (n > 0);
    if (neg) result += '-';
    std::reverse(result.begin(), result.end());
    return result;
}

void ConsoleLog::onFileExistence(IFileContainer *container, int index)
{
    if (!container) return;

    try {
        QFileInfo file = (*container)[index];
        file.refresh();
        log(std::to_string(index) + ": " +
            file.absoluteFilePath().toStdString() + " | " +
            qint64ToString(file.size()));
    } catch (...) {
        log("Error accessing index " + std::to_string(index));
    }
}

void ConsoleLog::onFileUpdate(IFileContainer *container, int index)
{
    if (!container) return;

    try {
        QFileInfo file = (*container)[index];
        file.refresh();
        log(std::to_string(index) + ": [UPDATED " +
            file.lastModified().toString("HH:mm:ss").toStdString() + "] " +
            file.absoluteFilePath().toStdString() + " | " +
            qint64ToString(file.size()));
    } catch (...) {
        log("Error accessing index " + std::to_string(index));
    }
}

void ConsoleLog::onFileRemoval(IFileContainer *container, int index)
{
    if (!container) return; // Проверка nullptr

    try {
        QFileInfo file = (*container)[index];
        log(std::to_string(index) + ": REMOVED " +
            file.absoluteFilePath().toStdString());
    } catch (...) {
        log("Error accessing index " + std::to_string(index));
    }
}
