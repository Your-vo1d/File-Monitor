#ifndef TIMEFILEUPDATER_H
#define TIMEFILEUPDATER_H

#include "IFileUpdater.h"
#include "IFileInfo.h"

class TimeUpdater : public IFileUpdater
{
public:
    TimeUpdater() = default;

    bool update(QSharedPointer<IFileInfo>& fileInfo) override;
    QString strategyName() const override { return "TimeUpdater"; }

    void setMinChangeInterval(qint64 ms) { m_minInterval = ms; }

private:
    qint64 m_minInterval = 1000;
};

#endif // TIMEFILEUPDATER_H
