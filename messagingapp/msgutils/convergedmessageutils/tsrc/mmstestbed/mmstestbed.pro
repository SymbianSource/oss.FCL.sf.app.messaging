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

TEMPLATE = lib

TARGET = mmstestbed

INCLUDEPATH += .
INCLUDEPATH += inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../../../mmsengine/inc
INCLUDEPATH += ../../../../../mmsengine/mmscodec/inc
INCLUDEPATH += ../../../../../mmsengine/mmshttptransport/inc
INCLUDEPATH += ../../../../../mmsengine/mmsmessage/inc
INCLUDEPATH += ../../../../../mmsengine/mmsconninit/inc
INCLUDEPATH += ../../../../../mmsengine/mmsserver/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_MMSTESTBED_DLL

SOURCES += src/mmstestbed.cpp \
           src/mmsteststaticutils.cpp \
           src/mmsreadfile.cpp \
           src/mmstestuitimer.cpp

HEADERS += inc/mmstestbed.h \
           inc/mmsteststaticutils.h \
           inc/mmsreadfile.h \
           inc/mmstestuitimer.h \
           inc/mmstestbed.hrh

SYMBIAN_PLATFORMS = WINSCW ARMV5

symbian {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    TARGET.EPOCALLOWDLLDATA = 1 
	}

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>"

LIBS += -lmsgs \
        -lmmsmessage \
        -lmmsconninit \
        -lmmscodec \
        -lmmsserversettings \
        -lcentralrepository \
        -lapparc \
        -lbafl \
        -lefsrv \
        -lesock \
        -lestor \
        -leuser \
        -llogcli \
        -llogwrap \
        -lapgrfx \
        -lapmime
