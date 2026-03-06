#include "ILog.h"

#include <QFileInfo>
#include <algorithm>
#include <cstdlib>

ConsoleLog::ConsoleLog(bool logTime)
{
    // Устанавливает, нужно ли выводить временные метки в логах
    m_logTime = logTime;
}

//Очищение консоли
void ConsoleLog::clearConsole()
{
    system("cls");
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
    // Случай: файл существует и не был изменён
    QFileInfo file = (*container)[index];

    log(std::to_string(index) + ": "
        + file.absoluteFilePath().toStdString()
        + " | "
        + qint64ToString(file.size()));
}

void ConsoleLog::onFileUpdate(IFileContainer *container, int index)
{
    // Случай: файл существует и был обновлён
    QFileInfo file = (*container)[index];

    log(std::to_string(index) + ": [UPDATED "
        + file.lastModified().time().toString().toStdString()
        + "] "
        + file.absoluteFilePath().toStdString()
        + " | "
        + qint64ToString(file.size()));
}

void ConsoleLog::onFileRemoval(IFileContainer *container, int index)
{
    // Случай: файл был удалён или не существует
    QFileInfo file = (*container)[index];

    log(std::to_string(index) + ": [NOT EXISTS] "
        + file.absoluteFilePath().toStdString());
}

void ConsoleLog::onCycleEnd()
{
    // Завершение цикла: очистка консоли
    clearConsole();
}