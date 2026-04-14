#include "ConsoleLog.h"
#include <iostream>
#include <algorithm>
#include <cstdio>

ConsoleLog::ConsoleLog(bool logTime) : m_out(stdout), m_logTime(logTime) {}

void ConsoleLog::clearConsole() { /* Отключено для сохранения истории */ }

void ConsoleLog::log(const std::string &data) {
    if (m_logTime) m_out << QTime::currentTime().toString("HH:mm:ss") << ' ';
    m_out << "[LOG] " << QString::fromStdString(data) << '\n';
    m_out.flush();
}

std::string ConsoleLog::qint64ToString(qint64 value) {
    if (value == 0) return "0";
    std::string result; bool neg = value < 0; qint64 n = neg ? -value : value;
    do { result += "0123456789"[n % 10]; n /= 10; } while (n);
    if (neg) result += '-';
    std::reverse(result.begin(), result.end());
    return result;
}

void ConsoleLog::onFileExistence(IFileContainer *container, int index) {
    if (!container) return;
    try {
        QFileInfo f = container->operator[](index);
        log(std::to_string(index) + ": " + f.absoluteFilePath().toStdString() +
            " | Size: " + qint64ToString(f.size()));
    } catch (...) { log("⚠️ Error accessing index " + std::to_string(index)); }
}

void ConsoleLog::onFileUpdate(IFileContainer *container, int index) {
    if (!container) return;
    try {
        QFileInfo f = container->operator[](index);
        log(std::to_string(index) + ": [UPDATED " + f.lastModified().toString("HH:mm:ss").toStdString() + "] " +
            f.absoluteFilePath().toStdString() + " | Size: " + qint64ToString(f.size()));
    } catch (...) { log("⚠️ Error accessing index " + std::to_string(index)); }
}

void ConsoleLog::onFileRemoval(IFileContainer *container, int index) {
    if (!container) return;
    try {
        QFileInfo f = container->operator[](index);
        log(std::to_string(index) + ": [NOT EXISTS] " + f.absoluteFilePath().toStdString());
    } catch (...) { log("⚠️ Error accessing index " + std::to_string(index)); }
}

void ConsoleLog::onCycleEnd() { /* Пусто */ }
