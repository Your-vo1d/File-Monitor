#ifndef ILOG_H
#define ILOG_H

#include <QObject>
#include <QString>

class IFileContainer;

class ILog : public QObject
{
    Q_OBJECT
protected:
    explicit ILog(QObject* parent = nullptr) : QObject(parent) {}
public:
    virtual ~ILog() = default;
    virtual void log(const QString& message) = 0;

public slots:
    virtual void onFileExistence(IFileContainer* container, int index) = 0;
    virtual void onFileUpdate(IFileContainer* container, int index) = 0;
    virtual void onFileRemoval(IFileContainer* container, int index) = 0;
};

#endif // ILOG_H
