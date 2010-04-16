#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

TEMPLATE = lib
TARGET = msginit


DEFINES += MSGINIT_DLL

DEPENDPATH += . \
    inc \
    src
    
INCLUDEPATH += .
INCLUDEPATH += ../../../inc


symbian:
{
TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0x2002E679
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
SYMBIAN_PLATFORMS = WINSCW ARMV5
TARGET.EPOCALLOWDLLDATA = 1
}


CONFIG += hb

symbian {

BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/msginit.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(msginit.iby)" 

}

HEADERS += inc/startupmonitor.h \
					 inc/simscnumberdetector.h \
					 inc/msgsimnumberdetector.h
					
SOURCES += src/startupmonitor.cpp \
					 src/simscnumberdetector.cpp \
					 src/msgsimnumberdetector.cpp

LIBS += -lsmcm \
		    -lmsgs \	
        -lcentralrepository \
        -lcenrepnotifhandler \
        -letelmm
