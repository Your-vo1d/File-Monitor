#ifndef IFILECONTAINER_H
#define IFILECONTAINER_H
#include <QFileInfo>

class IFileContainer
{
public:
    virtual ~IFileContainer() = default;
    virtual QFileInfo operator[](int index) = 0;
    virtual void append(QFileInfo file) = 0;
    virtual void clear() = 0;
    virtual int length() = 0;
};
#endif // IFILECONTAINER_H
