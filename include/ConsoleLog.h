#ifndef CONSOLELOG_H
#define CONSOLELOG_H

#include "ILog.h"
#include <QTime>
#include <string>

class ConsoleLog : public ILog
{
    Q_OBJECT
private:
    bool m_logTime;
    static std::string qint64ToString(qint64 value);

public:
    explicit ConsoleLog(bool logTime = false);
    ~ConsoleLog() override = default;

    void log(const std::string &data) override;

public slots:
    void onFileExistence(IFileContainer *container, int index) override;
    void onFileUpdate(IFileContainer *container, int index) override;
    void onFileRemoval(IFileContainer *container, int index) override;
};

#endif // CONSOLELOG_H
