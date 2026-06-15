#include <QTest>
#include <QTemporaryDir>
#include <QFile>
#include <QSignalSpy>
#include <QFileInfo>
#include <QTimer>
#include <stdexcept>
#include <iostream>

#include "FileContainer.h"
#include "FileMonitor.h"
#include "ConsoleLog.h"

class MockLog : public ILog {
    Q_OBJECT
public:
    QList<QString> messages;
    void log(const QString& message) override { messages.append(message); }
public slots:
    void onFileExistence(IFileContainer*, int) override {}
    void onFileUpdate(IFileContainer*, int) override {}
    void onFileRemoval(IFileContainer*, int) override {}
};

class TestSuite : public QObject {
    Q_OBJECT
private:
    QTemporaryDir tempDir;
    MockLog mockLog;

    void createFile(const QString& path, const QString& content = "data") {
        QFile f(path);
        QVERIFY2(f.open(QIODevice::WriteOnly), qPrintable("Cannot create: " + path));
        f.write(content.toUtf8());
        f.close();
    }

private slots:
    void initTestCase() {
        QVERIFY2(tempDir.isValid(), "QTemporaryDir failed");
        qRegisterMetaType<IFileContainer*>("IFileContainer*");
    }

    // ── FileContainer ─────────────────────────────────────────────────────────
    void testContainer_appendValid() {
        FileContainer c;
        QString path = tempDir.filePath("valid.txt");
        createFile(path);
        QVERIFY(c.append(path));
        QCOMPARE(c.length(), 1);
    }

    void testContainer_appendInvalid() {
        FileContainer c;
        QVERIFY(!c.append("/non/existent.txt"));
        QVERIFY(!c.append(tempDir.path()));
        QVERIFY(!c.append(""));
        QCOMPARE(c.length(), 0);
    }

    void testContainer_appendDuplicate() {
        FileContainer c;
        QString path = tempDir.filePath("dup.txt");
        createFile(path);
        QVERIFY(c.append(path));
        QVERIFY(!c.append(path));
        QCOMPARE(c.length(), 1);
    }

    void testContainer_remove() {
        FileContainer c;
        QString path = tempDir.filePath("rem.txt");
        createFile(path);
        c.append(path);
        QVERIFY(c.remove(path));
        QCOMPARE(c.length(), 0);
        QVERIFY(!c.remove(path));
    }

    void testContainer_clear() {
        FileContainer c;
        createFile(tempDir.filePath("c1.txt"));
        createFile(tempDir.filePath("c2.txt"));
        c.append(tempDir.filePath("c1.txt"));
        c.append(tempDir.filePath("c2.txt"));
        c.clear();
        QCOMPARE(c.length(), 0);
    }

    void testContainer_operatorBounds() {
        FileContainer c;
        createFile(tempDir.filePath("idx.txt"));
        c.append(tempDir.filePath("idx.txt"));
        QVERIFY_EXCEPTION_THROWN(c[-1], std::out_of_range);
        QVERIFY_EXCEPTION_THROWN(c[100], std::out_of_range);
    }

    void testContainer_operatorRefresh() {
        FileContainer c;
        QString path = tempDir.filePath("refresh.txt");
        createFile(path, "init");
        c.append(path);

        QFileInfo f1 = c[0];
        qint64 size1 = f1.size();

        createFile(path, "modified content with different size!!!");
        QFileInfo f2 = c[0];

        QVERIFY2(f2.size() != size1, "File size should change after modification");
    }

    // ── FileMonitor ───────────────────────────────────────────────────────────
    void testMonitor_singleton() {
        FileContainer c;

        FileMonitor* m1 = FileMonitor::instance(&c, &mockLog);
        QVERIFY(m1 != nullptr);

        FileMonitor* m2 = FileMonitor::instance(&c, &mockLog);
        QCOMPARE(m1, m2);

        QVERIFY_EXCEPTION_THROWN(
            FileMonitor::instance(nullptr, nullptr),
            std::invalid_argument
        );
    }

    void testMonitor_nullContainerThrows() {
        QVERIFY_EXCEPTION_THROWN(
            FileMonitor::instance(nullptr, &mockLog),
            std::invalid_argument
        );
    }

    void testMonitor_startStop() {
        FileContainer c;
        createFile(tempDir.filePath("run.txt"));
        c.append(tempDir.filePath("run.txt"));

        auto* m = FileMonitor::instance(&c, &mockLog);
        QVERIFY(!m->isRunning());
        m->start();
        QVERIFY(m->isRunning());
        m->stop();
        QVERIFY(!m->isRunning());
    }

    void testMonitor_signalEmission() {
        FileContainer c;
        QString path = tempDir.filePath("signal.txt");
        createFile(path, "initial");
        c.append(path);

        auto* m = FileMonitor::instance(&c, &mockLog);
        m->setFileUpdateDisappearInterval(1);

        QSignalSpy spyExist(m, &FileMonitor::onFileExistence);
        QSignalSpy spyUpdate(m, &FileMonitor::onFileUpdate);

        m->start();
        QTest::qWait(1500);

        QVERIFY2(spyUpdate.count() >= 1,
                "Expected onFileUpdate for a freshly created file");

        createFile(path, "modified_content!!!");
        QTest::qWait(1500);

        QVERIFY2(spyUpdate.count() >= 2,
                "Expected second onFileUpdate after modification");

        m->stop();
    }

    void testMonitor_nullptrSafety() {
        FileContainer c;
        auto* m = FileMonitor::instance(&c, &mockLog);
        QVERIFY_EXCEPTION_THROWN(m->setContainer(nullptr), std::invalid_argument);
        QVERIFY(true);
    }

    // ── ConsoleLog ────────────────────────────────────────────────────────────
    void testConsoleLog_logOutput() {
        ConsoleLog log(false);
        log.log("Test message");
        QVERIFY(true);
    }

    void testConsoleLog_slotsNullptr() {
        ConsoleLog log;
        log.onFileExistence(nullptr, 0);
        log.onFileUpdate(nullptr, 0);
        log.onFileRemoval(nullptr, 0);
        QVERIFY(true);
    }

    void testConsoleLog_slotsValid() {
        ConsoleLog log(true);
        FileContainer c;
        QString path = tempDir.filePath("log.txt");
        createFile(path);
        c.append(path);

        log.onFileExistence(&c, 0);
        log.onFileUpdate(&c, 0);
        c.remove(path);
        log.onFileRemoval(&c, 0);
        QVERIFY(true);
    }

    void testConsoleLog_numberFormatting() {
        ConsoleLog log(false);
        log.log("Zero: 0");
        log.log("Negative: -123");
        log.log("Large: 999999999");
        QVERIFY(true);
    }
};

QTEST_MAIN(TestSuite)
#include "test_suite.moc"
