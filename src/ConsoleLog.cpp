#include "ConsoleLog.h"
#include "IFileContainer.h"
#include <stdexcept>

ConsoleLog::ConsoleLog(bool showTimestamp, QObject* parent)
    : ILog(parent), m_stdout(stdout), m_showTimestamp(showTimestamp)
{
}

void ConsoleLog::log(const QString& message)
{
    if (m_showTimestamp) {
        m_stdout << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")
                 << " - ";
    }
    m_stdout << message << Qt::endl;
}

void ConsoleLog::onFileExistence(IFileContainer* container, int index)
{
    if (!container) { log("File existence: null container"); return; }
    try {
        QFileInfo f = (*container)[index];
        log(QString("File exists: %1 (%2 bytes)").arg(f.absoluteFilePath()).arg(f.size()));
    } catch (const std::exception& e) {
        log(QString("File existence error at index %1: %2").arg(index).arg(e.what()));
    }
}

void ConsoleLog::onFileUpdate(IFileContainer* container, int index)
{
    if (!container) { log("File updated: null container"); return; }
    try {
        QFileInfo f = (*container)[index];
        log(QString("File updated: %1 (%2 bytes)").arg(f.absoluteFilePath()).arg(f.size()));
    } catch (const std::exception& e) {
        log(QString("File update error at index %1: %2").arg(index).arg(e.what()));
    }
}

void ConsoleLog::onFileRemoval(IFileContainer* container, int index)
{
    if (!container) { log("File removed: null container"); return; }
    try {
        QFileInfo f = (*container)[index];
        log(QString("File removed: %1").arg(f.absoluteFilePath()));
    } catch (...) {
        log(QString("File removed at index %1").arg(index));
    }
}
