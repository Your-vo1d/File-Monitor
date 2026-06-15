#ifndef IFILECONTAINER_H
#define IFILECONTAINER_H

#include <QFileInfo>
#include <QString>

class IFileContainer
{
public:
    virtual ~IFileContainer() = default;
    virtual bool append(const QString& path) = 0;
    virtual bool remove(const QString& path) = 0;
    virtual void clear() = 0;
    virtual int length() const = 0;
    virtual QFileInfo operator[](int index) = 0;
};

#endif // IFILECONTAINER_H
