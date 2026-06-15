#include <QCoreApplication>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include "FileContainer.h"
#include "ConsoleLog.h"
#include "FileMonitor.h"

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

void printStatus(FileContainer& container, ILog& logger) {
    logger.log(QString("Watchlist (%1 files):").arg(container.length()));
    for (int i = 0; i < container.length(); ++i) {
        try {
            QFileInfo f = container[i];
            f.refresh();
            QString status = f.exists() ? " OK " : " MISSING ";
            logger.log(QString("  [%1] %2%3| %4B | %5")
                .arg(i)
                .arg(f.absoluteFilePath())
                .arg(status)
                .arg(f.size())
                .arg(f.lastModified().toString("HH:mm:ss")));
        } catch (...) {
            logger.log(QString("  [%1] Error accessing file").arg(i));
        }
    }
    logger.log("----------------------------------------");
}

static void showMenu() {
    std::cout << "\n MENU: 1:add  2:remove  3:status  0:exit\n> ";
    std::cout.flush();
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    FileContainer container;
    ConsoleLog logger(true);

    FileMonitor* monitor = FileMonitor::instance(&container, &logger);
    monitor->setFileUpdateDisappearInterval(3);
    monitor->start();

    std::atomic<bool> running{true};
    std::thread inputThread([&]() {
        std::string line;
        showMenu();

        while (std::getline(std::cin, line)) {
            line = trim(line);
            if (line.empty()) { showMenu(); continue; }

            if (line == "0") {
                logger.log("Stopping monitor...");
                running = false;
                QCoreApplication::quit();
                break;
            }

            if (line == "3") {
                printStatus(container, logger);
                showMenu();
                continue;
            }

            if (line == "1" || line == "2") {
                std::cout << "Enter full file path: ";
                std::cout.flush();

                std::string filePath;
                std::getline(std::cin, filePath);
                filePath = trim(filePath);

                if (filePath.empty()) {
                    logger.log("Path cannot be empty.");
                    showMenu();
                    continue;
                }

                QString qPath = QString::fromStdString(filePath);
                bool ok = (line == "1") ? container.append(qPath)
                                        : container.remove(qPath);

                if (ok) {
                    logger.log(line == "1" ? "Added to watchlist." : "Removed from watchlist.");
                } else {
                    QFileInfo check(qPath);
                    if (check.exists() && check.isDir())  logger.log("Error: path is a directory.");
                    else if (!check.exists())              logger.log("Error: file not found.");
                    else if (line == "1")                  logger.log("Error: file already in watchlist.");
                    else                                   logger.log("Error: file not in watchlist.");
                }
                showMenu();
                continue;
            }

            logger.log("Unknown command. Use 1, 2, 3 or 0.");
            showMenu();
        }
    });

    app.exec();
    running = false;
    if (inputThread.joinable()) inputThread.join();
    monitor->stop();
    return 0;
}
