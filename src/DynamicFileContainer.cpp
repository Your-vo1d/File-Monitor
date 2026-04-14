#include "DynamicFileContainer.h"
#include <QFile>
#include <QTextStream>
#include <stdexcept>


QFileInfo DynamicFileContainer::operator[](int index) {
    QMutexLocker locker(&m_mutex);
    if (index < 0 || index >= m_files.size())
        throw std::out_of_range("DynamicFileContainer: index out of bounds");

    QFileInfo f = m_files.at(index);
    f.refresh();
    return f;
}

bool DynamicFileContainer::append(const QString& path) {
    if (path.isEmpty()) return false;

    QFileInfo fi(path);

    if (!fi.exists() || !fi.isFile()) return false;

    QMutexLocker locker(&m_mutex);

    // Проверка на дубликаты по абсолютному пути
    QString absPath = fi.absoluteFilePath();
    for (const auto& existing : m_files) {
        if (existing.absoluteFilePath() == absPath)
            return false; // Уже в списке
    }

    m_files.append(fi);
    return true;
}

bool DynamicFileContainer::remove(const QString& path) {
    if (path.isEmpty()) return false;

    QString targetPath = QFileInfo(path).absoluteFilePath();

    QMutexLocker locker(&m_mutex);

    for (int i = 0; i < m_files.size(); ++i) {
        if (m_files.at(i).absoluteFilePath() == targetPath) {
            m_files.removeAt(i);
            return true;
        }
    }
    return false;
}

void DynamicFileContainer::clear() {
    QMutexLocker locker(&m_mutex);
    m_files.clear();
}

int DynamicFileContainer::length() const {
    QMutexLocker locker(&m_mutex);
    return m_files.size();
}
