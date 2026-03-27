#ifndef IFILECONTAINER_H
#define IFILECONTAINER_H

#include <QObject>
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

class DynamicFileContainer : public IFileContainer
{
private:
    std::vector<QFileInfo> container;
    std::string fileContainerPath;

public:
    ~DynamicFileContainer() = default;
    DynamicFileContainer(std::string path);
    void setPath(std::string path);

    void refresh();

    QFileInfo operator[](int index);
    void append(QFileInfo file);
    void clear();
    int length();
};

#endif // IFILECONTAINER_H

