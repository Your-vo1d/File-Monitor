// fileinfo.h
#ifndef FILEINFO_H
#define FILEINFO_H

#include "IFileInfo.h"

class FileInfo : public IFileInfo {
public:
    explicit FileInfo(const QString& filePath);
    QString path() const override;
    qint64 size() const override;
    bool exists() const override;
    void setSize(qint64 newSize) override;
    void setExists(bool newExists) override;

private:
    QString m_path;
    qint64 m_size;
    bool m_exists;
};

#endif // FILEINFO_H
