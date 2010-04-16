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
TARGET = $$qtLibraryTarget(unifiededitorplugin)
CONFIG += plugin
CONFIG += hb

INCLUDEPATH += . inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../../../inc

symbian:TARGET.EPOCALLOWDLLDATA = 1
symbian:TARGET.CAPABILITY = CAP_GENERAL_DLL
symbian:TARGET.UID3 = 0x2001FE63

SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/unifiededitorplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(unifiededitorplugin.iby)"

# Input
HEADERS += inc/unifiededitorplugin.h
SOURCES += src/unifiededitorplugin.cpp

RESOURCES += unifiededitorplugin.qrc


