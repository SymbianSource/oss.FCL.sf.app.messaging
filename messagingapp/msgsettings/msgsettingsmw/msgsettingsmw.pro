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
# Description:  Message settings detail 
#

TEMPLATE = lib
TARGET = msgsettingsmw


DEFINES += MSGSETTINGENGINE_DLL

DEPENDPATH += . \
    inc \
    src
    
INCLUDEPATH += .
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../msgutils/s60qconversions/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

TARGET.UID3 = 0x20026F49
TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW \
    ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/msgsettingsmw.iby CORE_APP_LAYER_IBY_EXPORT_PATH(msgsettingsmw.iby)" \
     "stub_sis/msgsettingsmw_stub.sis   /epoc32/data/z/system/install/msgsettingsmw_stub.sis"



HEADERS += msgsettingengine.h \
					smssettingsprivate.h \
					mmssettingprivate.h \
					msgsettingsutil.h
					
SOURCES += msgsettingengine.cpp \
					smssettingsprivate.cpp \
					mmssettingsprivate.cpp \
					msgsettingsutil.cpp

LIBS += -lSmcm \
		    -lmsgs \	
        -ls60qconversions \
        -lcommdb \
        -lmmsserversettings \
		    -lmmsgenericsettings \
		    -lcentralrepository
		