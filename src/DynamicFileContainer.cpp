#include <fstream>
#include "IFileContainer.h"

// Очищает информацию о хранимых файлах и заново считывает её из файла контейнера
void DynamicFileContainer::refresh()
{
    // Открываем файл для чтения
    std::ifstream mFile(fileContainerPath);

    // Переменная для хранения пути к файлу
    std::string path;

    // Проверка: успешно ли открылся файл
    if (mFile.is_open())
    {
        // Очищаем текущий список файлов
        this->clear();

        // Читаем построчно до конца файла или ошибки
        while (getline(mFile, path))
        {
            // Создаём объект QFileInfo из прочитанного пути
            QFileInfo newFile(QString::fromStdString(path));

            // Добавляем файл в контейнер
            this->append(newFile);
        }
    }
}

// Конструктор: инициализация пути и загрузка файлов
DynamicFileContainer::DynamicFileContainer(std::string path)
{
    // Сохраняем путь к файлу-контейнеру
    this->fileContainerPath = path;

    // Сразу загружаем данные из файла
    this->refresh();
}

// Сеттер пути: изменение источника данных
void DynamicFileContainer::setPath(std::string path)
{
    // Обновляем путь к файлу-контейнеру
    this->fileContainerPath = path;

    // Перезагружаем данные по новому пути
    this->refresh();
}

// Оператор доступа по индексу с авто-обновлением данных
QFileInfo DynamicFileContainer::operator[](int index)
{
    // Обновляем список файлов из файла-контейнера
    this->refresh();

    // Возвращаем файл по запрошенному индексу
    return this->container[index];
}

// Добавление файла во внутренний контейнер
void DynamicFileContainer::append(QFileInfo file)
{
    // Добавляем готовый объект QFileInfo в вектор
    container.push_back(file);
}

// Очистка внутреннего контейнера
void DynamicFileContainer::clear()
{
    // Очищаем вектор хранимых файлов
    this->container.clear();
}

// Получение актуального количества файлов
int DynamicFileContainer::length()
{
    // Обновляем данные перед возвратом размера
    this->refresh();

    // Возвращаем текущее количество элементов
    return this->container.size();
}
