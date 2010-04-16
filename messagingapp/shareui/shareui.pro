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

TEMPLATE = lib
TARGET = shareui
DEFINES += BUILD_SHAREUI_DLL

DEPENDPATH += inc src
INCLUDEPATH += inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

    
QT += core
CONFIG += hb


TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.UID3 = 0x2002DD00
TARGET.EPOCHEAPSIZE = 0x20000 0x25000

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/shareui.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(shareui.iby)" 
     

HEADERS += shareuiprivate.h 
    
SOURCES += shareui.cpp \
    	   shareuiprivate.cpp

LIBS += -lxqservice \
        -lxqserviceutil
