#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#pragma once
#include <QObject>
#include <QFileInfo>
#include <QPointer>       
#include <memory>        
#include "ILog.h"
#include "IFileContainer.h"
#include "IFileRefresher.h"

class FileMonitor : public QObject
{
    Q_OBJECT

private:
    QPointer<IFileContainer> m_container;
    QPointer<ILog> m_logger;
    QPointer<IFileRefresher> m_fileRefresher;


private:
    FileMonitor();
    FileMonitor(IFileContainer* container, ILog* logger, IFileRefresher* fileRefresher);
    ~FileMonitor() override; 
    FileMonitor(const FileMonitor&) = delete;
    FileMonitor& operator=(const FileMonitor&) = delete;

    static std::unique_ptr<FileMonitor> m_instance;

public:
    static FileMonitor* instance(IFileContainer* container, ILog* logger, IFileRefresher* refresher);

    void setContainer(IFileContainer* container);
    void setLogger(ILog* logger);
    void setFileRefresher(IFileRefresher* refresher);

    void start();

signals:
    void onFileUpdate(IFileContainer* container, int index);
    void onFileRemoval(IFileContainer* container, int index);
    void onFileExistence(IFileContainer* container, int index);
    void onCycleEnd();
};

#endif // FILEMONITOR_H