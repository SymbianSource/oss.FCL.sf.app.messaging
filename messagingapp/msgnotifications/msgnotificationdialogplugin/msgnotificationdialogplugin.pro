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
TARGET = msgnotificationdialogplugin
CONFIG += plugin

CONFIG += hb

# directories
INCLUDEPATH += .
DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../../inc

HEADERS += inc/msgnotificationdialogplugin.h
HEADERS += inc/msgnotificationdialogwidget.h

SOURCES += src/msgnotificationdialogplugin.cpp
SOURCES += src/msgnotificationdialogwidget.cpp

RESOURCES += msgnotificationdialogplugin.qrc

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x2002432C

    pluginstub.sources = msgnotificationdialogplugin.dll
    pluginstub.path = /resource/plugins/devicedialogs
    DEPLOYMENT += pluginstub
}
BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
  "rom/msgnotificationdialogplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(msgnotificationdialogplugin.iby)"
  
LIBS += -lxqservice
