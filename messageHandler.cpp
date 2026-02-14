//messageHandler.cpp
#include "messageHandler.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) 
{
    static bool firstRun = true;

    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/AccessibleAtlas";
    QDir().mkpath(path);
    QFile outFile(path + "/AccessibleAtlas.log");

    // On the very first call, use Truncate to wipe the file 
    // Otherwise use Append so do not overwrite logs from 2 seconds ago
    auto openMode = firstRun ? (QIODevice::WriteOnly | QIODevice::Truncate)
        : (QIODevice::WriteOnly | QIODevice::Append);

    if (outFile.open(openMode)) 
    {
        firstRun = false; // Set to false so we append for the rest of the session
        QTextStream stream(&outFile);
        //QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

        stream << msg << Qt::endl;
    }
}