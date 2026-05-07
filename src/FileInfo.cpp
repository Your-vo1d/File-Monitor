#include "FileInfo.h"


FileInfo::FileInfo(const QString& filePath)
    : m_path(filePath)
    , m_size(0)
    , m_exists(false)
{
}

QString FileInfo::path() const
{
    return m_path;
}

qint64 FileInfo::size() const
{
    return m_size;
}

bool FileInfo::exists() const
{
    return m_exists;
}

void FileInfo::setSize(qint64 newSize)
{
    m_size = newSize;
}

void FileInfo::setExists(bool newExists)
{
    m_exists = newExists;
}
