#ifndef IFILEINFO_H
#define IFILEINFO_H

#include <QString>

class IFileInfo {
public:
    virtual ~IFileInfo() = default;
    virtual QString path() const = 0;
    virtual qint64 size() const = 0;
    virtual bool exists() const = 0;
    virtual void setSize(qint64 newSize) = 0;
    virtual void setExists(bool newExists) = 0;
};

#endif // IFILEINFO_H
