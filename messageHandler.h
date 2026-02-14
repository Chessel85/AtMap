//messageHandler.h
#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <QtGlobal>
#include <QMessageLogContext>
#include <QString>

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);

#endif // LOGHANDLER_H