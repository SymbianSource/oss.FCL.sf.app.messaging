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
TARGET = s60qconversions
DEPENDPATH += . inc src
INCLUDEPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_S60QCONV_DLL
#QMAKE_CXXFLAGS.ARMCC -= --no_hide_all

# UID3
TARGET.UID3 = 0x2001FE7B

# Capability
TARGET.CAPABILITY = CAP_GENERAL_DLL

TARGET.EPOCALLOWDLLDATA = 1

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += \
     "$${LITERAL_HASH}include <platform_paths.hrh>" \
     "rom/s60qconversions.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(s60qconversions.iby)"

# Input
HEADERS += inc/s60qconversions.h
SOURCES += src/s60qconversions.cpp

