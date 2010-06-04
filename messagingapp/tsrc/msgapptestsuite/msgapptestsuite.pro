#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#

QT += testlib
QT -= gui

CONFIG += symbian_test

TEMPLATE = app
TARGET = MsgAppTestSuite
TARGET.CAPABILITY = All -TCB -DRM
DEPENDPATH += .
INCLUDEPATH += . 
INCLUDEPATH += ../../msgui/appengine/tsrc
INCLUDEPATH += ../../msgui/appengine/tsrc/testconversationengine/inc


# Input
SOURCES += src/main.cpp

SYMBIAN_PLATFORMS = WINSCW ARMV5
symbian {
	    TARGET.UID3 = 0x2001FE76
	    TARGET.EPOCSTACKSIZE = 0x8000
	    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
	    BLD_INF_RULES.prj_testexports += "msgapptestsuite.cfg c:/msgapptestsuite.cfg"
	    BLD_INF_RULES.prj_testexports += "msgapptestsuitreport.pl c:/msgapptestsuitreport.pl"
	    
	    testdll.sources = testconversationengine.dll
	    testdll.path = /sys/bin
	    DEPLOYMENT += testdll 
		}
		
LIBS += \
		-ltestconversationengine \