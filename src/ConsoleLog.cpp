#include "ConsoleLog.h"

#include <QFileInfo>
#include <cstdlib>

ConsoleLog::ConsoleLog(bool logTime)
{
    // Устанавливает, нужно ли выводить временные метки в логах
    m_logTime = logTime;
}


//Вывод сообщения в консоль
void ConsoleLog::log(const std::string &data)
{
    if (m_logTime) {
        std::cout << m_time.currentTime().toString().toStdString() << ' ';
    }

    std::cout << "[LOG] " << data << '\n';
}

std::string ConsoleLog::qint64ToString(qint64 value)
{
    std::string result;
    result.reserve(20); // максимум 20 цифр для qint64

    qint64 number = value;
    do {
        result += "0123456789"[number % 10];
        number /= 10;
    } while (number);

    std::reverse(result.begin(), result.end());
    return result;
}

void ConsoleLog::onFileExistence(IFileContainer *container, int index)
{
    if (!container) {
        log("onFileExistence: container is nullptr (index " + std::to_string(index) + ")");
        return;
    }

    try {
        QFileInfo file = (*container)[index];

        file.refresh();

        log(std::to_string(index) + ": "
            + file.absoluteFilePath().toStdString()
            + " | "
            + qint64ToString(file.size()));

    } catch (const std::out_of_range&) {
        log("onFileExistence: index " + std::to_string(index) + " out of bounds");
    } catch (const std::exception& e) {
        log("onFileExistence: error accessing index " + std::to_string(index) + ": " + e.what());
    }
}

void ConsoleLog::onFileUpdate(IFileContainer *container, int index)
{
    if (!container) {
        log("onFileUpdate: container is nullptr (index " + std::to_string(index) + ")");
        return;
    }

    try {
        QFileInfo file = (*container)[index];
        file.refresh();

        log(std::to_string(index) + ": [UPDATED "
            + file.lastModified().toString("HH:mm:ss").toStdString()
            + "] "
            + file.absoluteFilePath().toStdString()
            + " | "
            + qint64ToString(file.size()));

    } catch (const std::out_of_range&) {
        log("onFileUpdate: index " + std::to_string(index) + " out of bounds");
    } catch (const std::exception& e) {
        log("onFileUpdate: error accessing index " + std::to_string(index) + ": " + e.what());
    }
}

void ConsoleLog::onFileRemoval(IFileContainer *container, int index)
{
    if (!container) {
        log("onFileRemoval: container is nullptr (index " + std::to_string(index) + ")");
        return;
    }

    try {
        QFileInfo file = (*container)[index];

        log(std::to_string(index) + ": REMOVED "
            + file.absoluteFilePath().toStdString());

    } catch (const std::out_of_range&) {
        log("onFileRemoval: index " + std::to_string(index) + " out of bounds");
    } catch (const std::exception& e) {
        log("onFileRemoval: error accessing index " + std::to_string(index) + ": " + e.what());
    }
}
