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
 * Description: main class for msgapptestsuite.
 */

#include <QtTest/QtTest>
#include "testconversationengine.h"

//out put directory for test results.
QString OUTPUTDIRECTORY = "c:/logs/messagingtestsuite";
//o/p directory for data to be written on temp file.
QString TEMPDIR = "c:/logs/messagingtestsuite/testdata";
//test result O/P file name.
QString RESULTFILE = "c:/logs/messagingtestsuite/result_%1.txt";
// folder named UID3 of msgapptestsuite inside private folder.
const QString PRIVATE_DIR("C:/private/2001fe76");

//factory method to create objects.
QObject* getObject(QString className)
{
    if(className == "TConversationEngine" )
    {
        return new TConversationEngine();
    }
 	else
	{
		return 0;
	}
}

//creating o/p directory.
void createOutPutDirectory()
    {
    QDir dir;
    //o/p dir
    dir.mkdir(OUTPUTDIRECTORY);
    //tmp dir
    dir.mkdir(TEMPDIR);
    // dir inside private folder.
    dir.mkdir(PRIVATE_DIR);
    }

//main entry point
int main(int argc, char *argv[])
    { 
    int ret = -1;
    QCoreApplication app(argc, argv);    
    
    //creating output directory.
    createOutPutDirectory();
    
    //the configuration file.
    QFile data("c:/msgapptestsuite.cfg");

    if (data.open(QFile::ReadOnly)) 
        {
        QTextStream in(&data);
        while(!in.atEnd())
            {
            QStringList args;
            QString appName = argv[0];
            args << appName;

            QString option  = "-o";
            args << option;

            QString outFile = RESULTFILE;
            QString name = in.readLine();
            outFile = outFile.arg(name);
            args << outFile;

            QObject* tc = getObject(name);

            if(tc)
                {

                ret =  QTest::qExec(tc, args); 
                delete tc;
                }
            }
        }    
    data.close();
    return ret;
    }
