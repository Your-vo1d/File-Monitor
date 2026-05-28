#ifndef CONSOLELOG_H
#define CONSOLELOG_H

#include "ILog.h"
#include <QTextStream>
#include <QDateTime>

class ConsoleLog : public ILog
{
    Q_OBJECT
    QTextStream m_stdout;
    bool m_showTimestamp;
public:
    explicit ConsoleLog(bool showTimestamp = false, QObject* parent = nullptr);
    void log(const QString& message) override;

public slots:
    void onFileExistence(IFileContainer* container, int index) override;
    void onFileUpdate(IFileContainer* container, int index) override;
    void onFileRemoval(IFileContainer* container, int index) override;
};

#endif // CONSOLELOG_H
