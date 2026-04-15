#include <QCoreApplication>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include "DynamicFileContainer.h"
#include "ConsoleLog.h"
#include "FileMonitor.h"

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

void printStatus(DynamicFileContainer& container, ILog& logger) {
    logger.log("Watchlist (" + std::to_string(container.length()) + " files):");
    for (int i = 0; i < container.length(); ++i) {
        try {
            QFileInfo f = container[i];
            f.refresh();
            std::string status = f.exists() ? " ✅ " : " ❌ ";
            logger.log("  [" + std::to_string(i) + "] " +
                       f.absoluteFilePath().toStdString() + status +
                       "| " + std::to_string(f.size()) + "B | " +
                       f.lastModified().toString("HH:mm:ss").toStdString());
        } catch (...) { logger.log("  [" + std::to_string(i) + "] Error accessing file"); }
    }
    logger.log("----------------------------------------");
}

static void showMenu() {
    std::cout << "\n MENU: 1:add  2:remove  3:status  0:exit\n> ";
    std::cout.flush();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    DynamicFileContainer container;
    ConsoleLog logger(true);

    FileMonitor* monitor = FileMonitor::instance(&container, &logger);
    monitor->setFileUpdateDisappearInterval(3);
    monitor->start();

    std::atomic<bool> running{true};
    std::thread inputThread([&]() {
        std::string line;
        showMenu(); // Показываем меню при старте

        while (std::getline(std::cin, line)) {
            line = trim(line);
            if (line.empty()) { showMenu(); continue; }


            if (line == "0") {
                logger.log("Остановка мониторинга...");
                running = false;
                break;
            }

            if (line == "3") {
                printStatus(container, logger);
                showMenu();
                continue;
            }

            if (line == "1" || line == "2") {
                std::cout << "Введите полный путь к файлу: ";
                std::cout.flush();

                std::string filePath;
                std::getline(std::cin, filePath);
                filePath = trim(filePath);

                if (filePath.empty()) {
                    logger.log(" Путь не может быть пустым");
                    showMenu();
                    continue;
                }

                QString qPath = QString::fromStdString(filePath);
                bool ok = (line == "1") ? container.append(qPath)
                                        : container.remove(qPath);

                if (ok) {
                    logger.log(line == "1" ? "Добавлено в список." : "Удалено из списка.");
                } else {
                    QFileInfo check(qPath);
                    if (check.exists() && check.isDir()) logger.log("Ошибка: указан каталог.");
                    else if (!check.exists())            logger.log("Ошибка: файл не найден.");
                    else if (line == "1")                logger.log("Ошибка: файл уже в списке.");
                    else                                 logger.log("Ошибка: файл не найден в списке.");
                }
                showMenu();
                continue;
            }

            logger.log("Неизвестная команда. Используйте 1, 2, 3 или 0.");
            showMenu();
        }
    });

    app.exec();
    running = false;
    if (inputThread.joinable()) inputThread.join();
    monitor->stop();
    return 0;
}
