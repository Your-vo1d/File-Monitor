#include "TimeFileUpdater.h"
#include <QFileInfo>
#include <QDateTime>

bool TimeUpdater::update(QSharedPointer<IFileInfo>& fileInfo)
{
    if (!fileInfo) return false;

    QFileInfo current(fileInfo->path());

    bool nowExists = current.exists();
    if (nowExists != fileInfo->exists()) {
        fileInfo->setExists(nowExists);
        if (nowExists)
            fileInfo->setSize(current.size());
        return true;
    }

    if (!nowExists) return false;

    qint64 newSize = current.size();
    bool sizeChanged = (newSize != fileInfo->size());

    qint64 ageMs = QDateTime::currentMSecsSinceEpoch()
                   - current.lastModified().toMSecsSinceEpoch();
    bool recentlyModified = ageMs >= 0 && ageMs <= m_minInterval;

    if (sizeChanged || recentlyModified) {
        fileInfo->setSize(newSize);
        return true;
    }

    return false;
}
