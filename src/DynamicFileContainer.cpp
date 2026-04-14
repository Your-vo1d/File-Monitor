#include "DynamicFileContainer.h"
#include <QFile>
#include <QTextStream>
#include <stdexcept>

DynamicFileContainer::DynamicFileContainer(const QString& path)
    : m_fileContainerPath(path) { refresh(); }

void DynamicFileContainer::setPath(const QString& path) {
    QMutexLocker locker(&m_mutex);
    m_fileContainerPath = path;
    refreshInternal();
}

void DynamicFileContainer::refresh() {
    QMutexLocker locker(&m_mutex);
    refreshInternal();
}

void DynamicFileContainer::refreshInternal() {
    m_baseFiles.clear();
    QFile file(m_fileContainerPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            QFileInfo fi(line);
            if (fi.exists() && fi.isFile()) m_baseFiles.append(fi);
        }
    }
}

bool DynamicFileContainer::addManualFile(const QString& path) {
    if (path.isEmpty()) return false;
    QFileInfo fi(path);
    if (!fi.exists() || !fi.isFile()) return false;

    QMutexLocker locker(&m_mutex);
    m_manualFiles.append(fi);
    return true;
}

bool DynamicFileContainer::removeManualFile(const QString& path) {
    QMutexLocker locker(&m_mutex);
    QString target = QFileInfo(path).absoluteFilePath();
    auto it = std::remove_if(m_manualFiles.begin(), m_manualFiles.end(),
                             [&target](const QFileInfo& f){ return f.absoluteFilePath() == target; });
    if (it != m_manualFiles.end()) {
        m_manualFiles.erase(it, m_manualFiles.end());
        return true;
    }
    return false;
}

QVector<QFileInfo> DynamicFileContainer::getSnapshot() const {
    QMutexLocker locker(&m_mutex);
    QVector<QFileInfo> res = m_baseFiles;
    res.append(m_manualFiles);
    return res;
}

QFileInfo DynamicFileContainer::operator[](int index) {
    QMutexLocker locker(&m_mutex);
    int baseSize = m_baseFiles.size();
    int totalSize = baseSize + m_manualFiles.size();
    if (index < 0 || index >= totalSize)
        throw std::out_of_range("DynamicFileContainer: index out of bounds");

    QFileInfo f = (index < baseSize) ? m_baseFiles[index] : m_manualFiles[index - baseSize];
    f.refresh();
    return f;
}

void DynamicFileContainer::append(QFileInfo file) {
    QMutexLocker locker(&m_mutex);
    m_manualFiles.append(file);
}

void DynamicFileContainer::clear() {
    QMutexLocker locker(&m_mutex);
    m_baseFiles.clear();
    m_manualFiles.clear();
}

int DynamicFileContainer::length() {
    QMutexLocker locker(&m_mutex);
    return m_baseFiles.size() + m_manualFiles.size();
}
