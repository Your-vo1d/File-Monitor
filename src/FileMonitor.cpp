#include "FileMonitor.h"
#include <QDateTime>
#include <stdexcept>
#include <iostream>

std::unique_ptr<FileMonitor> FileMonitor::m_instance = nullptr;

FileMonitor::FileMonitor(IFileContainer* container, ILog* logger)
    : m_container(container), m_logger(logger) {
    if (!m_container) {
        std::cerr << "IFileContainer cannot be null" << std::endl;
        throw std::invalid_argument("IFileContainer cannot be null");
    }
    if (!m_logger) {
        std::cerr << "ILog cannot be null" << std::endl;
        throw std::invalid_argument("ILog cannot be null");
    }
}

FileMonitor::~FileMonitor() { stop(); }

FileMonitor* FileMonitor::instance(IFileContainer* container, ILog* logger) {
    if (!container) {
        if (m_instance && m_instance->m_logger) {
            m_instance->m_logger->log("IFileContainer cannot be null");
        } else {
            std::cerr << "IFileContainer cannot be null" << std::endl;
        }
        throw std::invalid_argument("IFileContainer cannot be null");
    }
    if (!logger) {
        if (m_instance && m_instance->m_logger) {
            m_instance->m_logger->log("ILog cannot be null");
        } else {
            std::cerr << "ILog cannot be null" << std::endl;
        }
        throw std::invalid_argument("ILog cannot be null");
    }


    if (!m_instance) {
        m_instance.reset(new FileMonitor(container, logger));
    } else {
        m_instance->m_container = container;
        m_instance->m_logger = logger;
    }
    return m_instance.get();
}

void FileMonitor::setContainer(IFileContainer* c) {
    if (!c) {
        if (m_logger) {
            m_logger->log("Container cannot be null");
        } else {
            std::cerr << "Container cannot be null" << std::endl;
        }
        throw std::invalid_argument("Container cannot be null");
    }
    m_container = c;
}

void FileMonitor::setLogger(ILog* l) {
    if (!l) {
        if (m_logger) {
            m_logger->log("Logger cannot be null");
        } else {
            std::cerr << "Logger cannot be null" << std::endl;
        }
        throw std::invalid_argument("Logger cannot be null");
    }
    if (m_logger) {
        disconnect(this, nullptr, m_logger, nullptr);
    }
    m_logger = l;
    connectSignalsToLogger();
}

void FileMonitor::setFileUpdateDisappearInterval(unsigned int s) {
    m_intervalSec = s;
}

void FileMonitor::start() {
    if (!m_container) {
        if (m_logger) {
            m_logger->log("Container is nullptr, cannot start");
        } else {
            std::cerr << "Container is nullptr, cannot start" << std::endl;
        }
        throw std::runtime_error("Container is nullptr");
    }
    if (m_timer && m_timer->isActive()) {
        return;
    }

    if (!m_timer) {
        m_timer = std::make_unique<QTimer>();
        connect(m_timer.get(), &QTimer::timeout, this, &FileMonitor::checkFiles);
        connectSignalsToLogger();
    }
    m_timer->start(m_intervalSec * 1000);
    checkFiles();
}

void FileMonitor::stop() {
    if (m_timer) {
        m_timer->stop();
        m_timer.reset();
    }
    m_previousStates.clear();
}

bool FileMonitor::isRunning() const {
    return m_timer && m_timer->isActive();
}

void FileMonitor::connectSignalsToLogger() {
    if (m_logger) {
        connect(this, &FileMonitor::onFileUpdate,    m_logger, &ILog::onFileUpdate);
        connect(this, &FileMonitor::onFileRemoval,   m_logger, &ILog::onFileRemoval);
        connect(this, &FileMonitor::onFileExistence, m_logger, &ILog::onFileExistence);
    }
}

void FileMonitor::checkFiles() {
    if (!m_container) {
        if (m_logger) {
            m_logger->log("Container is null in checkFiles");
        } else {
            std::cerr << "Container is null in checkFiles" << std::endl;
        }
        return;
    }


    int currentLen = m_container->length();
    bool hasChanges = false;

    while (m_previousStates.size() > static_cast<size_t>(currentLen)) {
        emit onFileRemoval(m_container, static_cast<int>(m_previousStates.size()) - 1);
        m_previousStates.pop_back();
        hasChanges = true;
    }

    if (static_cast<int>(m_previousStates.size()) < currentLen) {
        m_previousStates.resize(currentLen);
        hasChanges = true;
    }

    for (int i = 0; i < currentLen; ++i) {
        QFileInfo cur = m_container->operator[](i);
        cur.refresh();

        const QFileInfo& prev = m_previousStates[i];
        bool isNew = prev.absoluteFilePath().isEmpty();

        if (isNew || cur.exists() != prev.exists() ||
            cur.size() != prev.size() ||
            cur.lastModified() != prev.lastModified()) {
            hasChanges = true;
        }
        m_previousStates[i] = cur;
    }

    if (hasChanges) {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        for (int i = 0; i < currentLen; ++i) {
            QFileInfo file = m_previousStates[i];
            if (file.exists()) {
                qint64 ageMs = now - file.lastModified().toMSecsSinceEpoch();
                if (ageMs <= static_cast<qint64>(m_intervalSec) * 1000) {
                    emit onFileUpdate(m_container, i);
                } else {
                    emit onFileExistence(m_container, i);
                }
            } else {
                emit onFileRemoval(m_container, i);
            }
        }
        emit onCycleEnd();
    }
}
