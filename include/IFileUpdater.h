// IFileUpdater.h
#ifndef IFILEUPDATER_H
#define IFILEUPDATER_H

#include <QSharedPointer>
#include "IFileInfo.h"

class IFileUpdater
{
public:
    virtual ~IFileUpdater() = default;

    /**
     * @brief Проверяет и обновляет информацию о файле
     * @param fileInfo Умный указатель на интерфейс файла
     * @return true, если файл был изменён и требует обработки
     */
    virtual bool update(QSharedPointer<IFileInfo>& fileInfo) = 0;

    /**
     * @brief Возвращает тип стратегии (для отладки/логирования)
     */
    virtual QString strategyName() const = 0;
};

#endif // IFILEUPDATER_H
