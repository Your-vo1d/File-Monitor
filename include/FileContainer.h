#ifndef FILECONTAINER_H
#define FILECONTAINER_H

#include "IFileContainer.h"
#include <QList>
#include <QMutex>

class FileContainer : public IFileContainer
{
    QList<QFileInfo> m_files;
    mutable QMutex m_mutex;
public:
    ~FileContainer() = default;

    bool append(const QString& path) override;
    bool remove(const QString& path) override;
    void clear() override;
    int length() const override;
    QFileInfo operator[](int index) override;
};

#endif // FILECONTAINER_H
