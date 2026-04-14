#ifndef DYNAMICFILECONTAINER_H
#define DYNAMICFILECONTAINER_H
#include "IFileContainer.h"
#include <QString>
#include <QFileInfoList>
#include <QMutex>

class DynamicFileContainer : public IFileContainer
{
private:
    QFileInfoList m_files;
    mutable QMutex m_mutex;

public:
    ~DynamicFileContainer() = default;

    QFileInfo operator[](int index) override;
    bool append(const QString& path) override;
    bool remove(const QString& path) override;
    void clear() override;
    int length() const override;
};
#endif // DYNAMICFILECONTAINER_H
