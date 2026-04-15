#ifndef ILOG_H
#define ILOG_H

#pragma once

#include <string>
#include <QObject>
#include "IFileContainer.h"

//Определяет методы для обработки событий файловой системы.
class ILog : public QObject
{
    Q_OBJECT

public:
    virtual ~ILog() = default;

    //Записывает сообщение в лог.

    virtual void log(const std::string &data) = 0;

public slots:

    //Срабатывает при обнаружении файла.
    virtual void onFileExistence(IFileContainer *container, int index) = 0;

    //Срабатывает при обновлении файла.
    virtual void onFileUpdate(IFileContainer *container, int index) = 0;

    //Срабатывает при удалении файла.
    virtual void onFileRemoval(IFileContainer *container, int index) = 0;

    //Срабатывает в конце цикла обработки файлов
    virtual void onCycleEnd() = 0;
};

#endif // ILOG_H
