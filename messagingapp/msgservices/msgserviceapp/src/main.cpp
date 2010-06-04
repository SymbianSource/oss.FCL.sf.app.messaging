/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Messaging service application startup main().
*
*/

#include <HbApplication.h>
#include <QTranslator>
#include <QLocale>
#include <QFile>
#include <QPointer>
#include <QDateTime>

#include "msgservicewindow.h"
#include "debugtraces.h"

//Localised constants
#define LOC_TITLE hbTrId("txt_messaging_title_messaging")

const QString debugFileName("c:/msgservice_app.txt");

#ifdef _DEBUG_TRACES_
void debugInit(QtMsgType type, const char *msg)
{

    QFile ofile(debugFileName);
    if (!ofile.open(QIODevice::Append | QIODevice::Text))
    {
        qFatal("error opening results file");
        return;
    }
    QDateTime dt = QDateTime::currentDateTime();

    QTextStream out(&ofile);
    switch (type)
    {
        case QtDebugMsg:
            out << " DEBUG:";
            out << msg;
            break;
        case QtWarningMsg:
            out << " WARN:";
            out << msg;
            break;
        case QtCriticalMsg:
            out << "\n ";
            out << dt.toString("dd/MM/yyyy hh:mm:ss.zzz:ap");
            out << " CRITICAL:";
            out << msg;
            break;
        case QtFatalMsg:
            out << " FATAL:";
            out << msg;
            abort();
            break;
        default:
            out << " No Log Selection Type:";
            out << msg;
            break;

    }
}
#endif

int main(int argc, char **argv)
{
    HbApplication app( argc, argv );
    
    //TODO: Uncomment the lines when actual 
    //translation files are available in sdk and remove loading locally.
    QString locale = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    QTranslator translator;
    //QTranslator translator_comm;
    //translator.load(path + QString("messaging_") + locale);
    //translator_comm.load(path + QString("common_") + locale);
    translator.load( "messaging_en_GB", ":/translations" );
    app.installTranslator(&translator);
    //app.installTranslator(&translator_comm);

    app.setApplicationName(LOC_TITLE);
    
    QPointer<MsgServiceWindow> window = new MsgServiceWindow();
    window->show();
    
    int rv = app.exec();
    delete window;
    return rv;
}

