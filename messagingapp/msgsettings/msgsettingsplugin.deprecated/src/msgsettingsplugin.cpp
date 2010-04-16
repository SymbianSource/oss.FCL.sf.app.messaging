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
 * Description: This is for Message Settings plugin for ControlPanel
 *
 */

#include "msgsettingsplugin.h"
#include "msgsettingsview.h"
#include <cpsettingformentryitemdataimpl.h>
#include <QFile>
#include "debugtraces.h"

#include <QDateTime>

const QString debugFileName("c://ControlPanelSettings.txt");

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
        //out << " WARN:";
        //out << msg;
        break;
        case QtCriticalMsg:
        //out << "\n ";
        //out << dt.toString("dd/MM/yyyy hh:mm:ss.zzz:ap");
        //out << " CRITICAL:";
        //out << msg;
        break;
        case QtFatalMsg:
        //out << " FATAL:";
        //out << msg;
        //abort();
        break;
        default:
        //out << " No Log Selection Type:";
        //out << msg;
        break;

    }
 }
#endif 
 
MsgSettingsPlugin::MsgSettingsPlugin()
{
#ifdef _DEBUG_TRACES_
    //Debug Logs
	  QFile ofile;
    if (ofile.exists(debugFileName))
    {
      ofile.remove(debugFileName);
    }

		qInstallMsgHandler(debugInit);
#endif		
}

MsgSettingsPlugin::~MsgSettingsPlugin()
{

}

QList<CpSettingFormItemData*> MsgSettingsPlugin::createSettingFormItemData(CpItemDataHelper &itemDataHelper) const
{
		QDEBUG_WRITE("Enter MsgSettingsPlugin::createSettingFormItemData\n");

    return QList<CpSettingFormItemData*>() 
            << new CpSettingFormEntryItemDataImpl<MsgSettingsView>(
			   itemDataHelper,
               tr("Messaging"), 
			   tr("Message Settings"));
}

Q_EXPORT_PLUGIN2(msgsettingsplugin, MsgSettingsPlugin);
