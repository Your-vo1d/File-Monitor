#include "FileMonitor.h"
#include <QDateTime>
#include <stdexcept>

std::unique_ptr<FileMonitor> FileMonitor::m_instance = nullptr;

FileMonitor::FileMonitor(IFileContainer* container, ILog* logger, IFileRefresher* refresher)
    : m_container(container), m_logger(logger), m_refresher(refresher) {
    if (!m_container) throw std::invalid_argument("IFileContainer cannot be null");
}

FileMonitor::~FileMonitor() { stop(); }

FileMonitor* FileMonitor::instance(IFileContainer* container, ILog* logger, IFileRefresher* refresher) {
    if (!m_instance) {
        m_instance.reset(new FileMonitor(container, logger, refresher));
    } else {
        if (container) m_instance->m_container = container;
        if (logger)    m_instance->m_logger = logger;
        if (refresher) m_instance->m_refresher = refresher;
    }
    return m_instance.get();
}

void FileMonitor::resetForTesting() {
    if (m_instance) {
        m_instance->stop();
        m_instance.reset();
    }
}

void FileMonitor::setContainer(IFileContainer* c) {
    if (!c) throw std::invalid_argument("Container cannot be null");
    m_container = c;
}
void FileMonitor::setLogger(ILog* l) {
    if (m_logger) disconnect(this, nullptr, m_logger, nullptr);
    m_logger = l;
    connectSignalsToLogger();
}
void FileMonitor::setFileRefresher(IFileRefresher* r) { m_refresher = r; }
void FileMonitor::setFileUpdateDisappearInterval(unsigned int s) { m_intervalSec = s; }

void FileMonitor::start() {
    if (!m_container) throw std::runtime_error("Container is nullptr");
    if (m_timer && m_timer->isActive()) return;

    if (!m_timer) {
        m_timer = new QTimer(this);
        connect(m_timer, &QTimer::timeout, this, &FileMonitor::checkFiles);
        connectSignalsToLogger();
    }
    m_timer->start(m_intervalSec * 1000);
    checkFiles();
}

void FileMonitor::stop() {
    if (m_timer) { m_timer->stop(); delete m_timer; m_timer = nullptr; }
    m_previousStates.clear();
}

bool FileMonitor::isRunning() const { return m_timer && m_timer->isActive(); }

void FileMonitor::connectSignalsToLogger() {
    if (m_logger) {
        connect(this, &FileMonitor::onFileUpdate,    m_logger, &ILog::onFileUpdate);
        connect(this, &FileMonitor::onFileRemoval,   m_logger, &ILog::onFileRemoval);
        connect(this, &FileMonitor::onFileExistence, m_logger, &ILog::onFileExistence);
        connect(this, &FileMonitor::onCycleEnd,      m_logger, &ILog::onCycleEnd);
    }
}

void FileMonitor::checkFiles() {
    if (!m_container) return;
    if (m_refresher) m_refresher->refresh();

    int currentLen = m_container->length();
    bool sizeChanged = m_previousStates.size() != currentLen;
    bool hasChanges = sizeChanged;

    if (sizeChanged) {
        m_previousStates.resize(currentLen);
        for (int i = 0; i < currentLen; ++i) {
            try {
                QFileInfo f = m_container->operator[](i);
                f.refresh();
                m_previousStates[i] = f;
            } catch (...) { m_previousStates[i] = QFileInfo(); }
        }
    } else {
        for (int i = 0; i < currentLen; ++i) {
            try {
                QFileInfo cur = m_container->operator[](i);
                cur.refresh();
                const QFileInfo& prev = m_previousStates[i];
                if (cur.exists() != prev.exists() || cur.size() != prev.size() || cur.lastModified() != prev.lastModified()) {
                    hasChanges = true; break;
                }
            } catch (...) { hasChanges = true; break; }
        }
    }

    if (hasChanges || m_previousStates.isEmpty()) {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        for (int i = 0; i < currentLen; ++i) {
            try {
                QFileInfo file = m_container->operator[](i);
                file.refresh();
                qint64 ageMs = now - file.lastModified().toMSecsSinceEpoch();
                if (file.exists()) {
                    if (ageMs <= static_cast<qint64>(m_intervalSec) * 1000)
                        emit onFileUpdate(m_container, i);
                    else
                        emit onFileExistence(m_container, i);
                } else {
                    emit onFileRemoval(m_container, i);
                }
                m_previousStates[i] = file;
            } catch (...) { /* Игнорируем невалидные индексы безопасно */ }
        }
        emit onCycleEnd();
    }
}
