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
#

TEMPLATE = lib

TARGET = $$qtLibraryTarget(unieditorsmsplugin)
TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

# UID 3
TARGET.UID3 = 0x102072DA

INCLUDEPATH += ../../s60qconversions/inc
INCLUDEPATH += ../../unieditorutils/inc
INCLUDEPATH += ../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

SYMBIAN_PLATFORMS = WINSCW \
    ARMV5

CONFIG += plugin

# Input
HEADERS += inc/sessioneventhandler.h \
    inc/unieditorsmsplugin.h \
    inc/unieditorsmsplugin_p.h
    
SOURCES += src/sessioneventhandler.cpp \
    src/unieditorsmsplugin.cpp \
    src/unieditorsmsplugin_p.cpp

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/unieditorsmsplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(unieditorsmsplugin.iby)"

LIBS += -leuser \
    -lconvergedmessageutils \
    -ls60qconversions \
    -lMsgMedia \
    -lunieditorutils \
    -lcone \
    -leikcoctl \
    -leikcore \
    -leikdlg \
    -lmsgs \
    -letext \
    -lgsmu \
    -lmmsgenutils \
    -lefsrv \
    -lestor \
    -lsmcm \
    -lCommonEngine \
    -lbafl \
    -lCdlEngine \
    -lFeatMgr \
    -lapmime

# plugin stub deployment
plugin.sources = unieditorsmsplugin.dll
plugin.path = \resource\qt\plugins\messaging\editorplugins
DEPLOYMENT += plugin
