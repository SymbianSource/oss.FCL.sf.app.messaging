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
TARGET = $$qtLibraryTarget(conversationviewplugin)

TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = All -TCB
#UID 3
TARGET.UID3 = 0x2001FE75

CONFIG += plugin hb
INCLUDEPATH += . inc ../../conversationview/inc ../../../../inc

symbian:TARGET.EPOCALLOWDLLDATA = 1
symbian:TARGET.CAPABILITY = CAP_GENERAL_DLL
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Input
HEADERS += inc/conversationviewplugin.h
SOURCES += src/conversationviewplugin.cpp

RESOURCES += conversationviewplugin.qrc

# The plugin stub is exported and its used in the sis or image creation
symbian 		{
    BLD_INF_RULES.prj_exports += \
    "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/conversationviewplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(conversationviewplugin.iby)" 
		}

