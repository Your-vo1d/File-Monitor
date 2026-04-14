#ifndef DYNAMICFILECONTAINER_H
#define DYNAMICFILECONTAINER_H
#include "IFileContainer.h"
#include <QVector>
#include <QString>
#include <QFileInfo>
#include <QMutex>

class DynamicFileContainer : public IFileContainer
{
private:
    QVector<QFileInfo> m_baseFiles;
    QVector<QFileInfo> m_manualFiles;
    QString m_fileContainerPath;
    mutable QMutex m_mutex;

    void refreshInternal();

public:
    ~DynamicFileContainer() = default;
    explicit DynamicFileContainer(const QString& path);
    void setPath(const QString& path);
    void refresh();

    bool addManualFile(const QString& path);
    bool removeManualFile(const QString& path);
    QVector<QFileInfo> getSnapshot() const;

    QFileInfo operator[](int index) override;
    void append(QFileInfo file) override;
    void clear() override;
    int length() override;
};
#endif // DYNAMICFILECONTAINER_H
