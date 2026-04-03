#ifndef CONSOLELOG_H
#define CONSOLELOG_H

#include "ILog.h"
#include <QTime>

class ConsoleLog : public ILog
{
    Q_OBJECT

private:

    //Текущее время (используется для вывода временной метки)
    QTime m_time;

    //Флаг вывода времени перед сообщением
    bool m_logTime = false;

    //Преобразует qint64 в std::string
    static std::string qint64ToString(qint64 value);

public:

    ~ConsoleLog() = default;

    //Конструктор
    explicit ConsoleLog(bool logTime);

    //Вывод сообщения в консоль
    void log(const std::string &data) override;

    //Очистка консоли
    void clearConsole();

public slots:

    //Обработка события обнаружения файла
    void onFileExistence(IFileContainer *container, int index) override;

    //Обработка события обновления файла
    void onFileUpdate(IFileContainer *container, int index) override;

    //Обработка события удаления файла
    void onFileRemoval(IFileContainer *container, int index) override;

    //Завершение цикла обработки
    void onCycleEnd() override;
};

#endif // CONSOLELOG_H
