#ifndef CONSOLELOG_H
#define CONSOLELOG_H

#include "IFileContainer"
#include "ILog.h"
#include <QTime>
#include <QObject>
#include <QTextStream>
#include <string>

class ConsoleLog : public ILog
{
    Q_OBJECT

public:
    // Конструктор
    explicit ConsoleLog(bool logTime = false);

    // Деструктор
    ~ConsoleLog() override = default;

    // Вывод сообщения в консоль
    void log(const std::string &data) override;

    // Вспомогательная функция: преобразует qint64 в std::string
    static std::string qint64ToString(qint64 value);

public slots:
    // Обработка события обнаружения файла
    void onFileExistence(IFileContainer *container, int index) override;

    // Обработка события обновления файла
    void onFileUpdate(IFileContainer *container, int index) override;

    // Обработка события удаления файла
    void onFileRemoval(IFileContainer *container, int index) override;

private:
    QTime m_time;                    // Текущее время (для вывода временной метки)
    bool m_logTime = false;          // Флаг вывода времени перед сообщением
    QTextStream m_out;               // Поток вывода в консоль
};

#endif // CONSOLELOG_H
