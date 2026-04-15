#ifndef FILEMONITOR_H
#define FILEMONITOR_H
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QFileInfo>
#include <memory>
#include "IFileContainer.h"
#include "ILog.h"

class FileMonitor : public QObject
{
    Q_OBJECT
public:
    ~FileMonitor() override;
    static FileMonitor* instance(IFileContainer* container, ILog* logger);

    void setContainer(IFileContainer* container);
    void setLogger(ILog* logger);
    void setFileUpdateDisappearInterval(unsigned int intervalSeconds);

    void start();
    void stop();
    bool isRunning() const;

signals:
    void onFileExistence(IFileContainer* container, int index);
    void onFileUpdate(IFileContainer* container, int index);
    void onFileRemoval(IFileContainer* container, int index);
    void onCycleEnd();

private slots:
    void checkFiles();

private:
    explicit FileMonitor(IFileContainer* container, ILog* logger);
    void connectSignalsToLogger();
    static std::unique_ptr<FileMonitor> m_instance;

    IFileContainer* m_container = nullptr;
    ILog* m_logger = nullptr;
    unsigned int m_intervalSec = 30;
    std::unique_ptr<QTimer> m_timer;
    QVector<QFileInfo> m_previousStates;
};
#endif // FILEMONITOR_H
