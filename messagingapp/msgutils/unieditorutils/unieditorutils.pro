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
TARGET = unieditorutils
DEPENDPATH += . inc src

INCLUDEPATH += .
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../s60qconversions/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_MUIU_UITLS_DLL
#QMAKE_CXXFLAGS.ARMCC -= --no_hide_all

# UID3
TARGET.UID3 = 0x2001FE70

# Capability
TARGET.CAPABILITY = CAP_GENERAL_DLL

TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/unieditorutils.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(unieditorutils.iby)"

# Input
HEADERS += inc/MuiuOperationWait.h \
    inc/UniEditorGenUtils.h
    
SOURCES += src/MuiuOperationWait.cpp \
    src/UniEditorGenUtils.cpp

LIBS += -leuser \
    -lCentralRepository \
    -lconvergedmessageutils \
    -lsssettings \
    -ls60qconversions \
    -lMsgMedia \
    -lapmime

