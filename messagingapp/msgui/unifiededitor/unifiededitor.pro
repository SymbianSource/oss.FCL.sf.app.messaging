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
TARGET = unifiededitor
DEFINES += UNIFIEDEDITOR_DLL
DEPENDPATH += . \
    inc \
    ../inc \
    src 
INCLUDEPATH += . inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../msguiutils/inc
INCLUDEPATH += ../../msgutils/unieditorutils/inc
INCLUDEPATH += ../../msgutils/unidatamodelloader/inc
INCLUDEPATH += ../../msgutils/s60qconversions/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

CONFIG += hb

TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = All -TCB
TARGET.UID3 = 0x2001FE68

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "rom/unifiededitor.iby CORE_APP_LAYER_IBY_EXPORT_PATH(unifiededitor.iby)"

# Input
HEADERS += msgmonitor.h \
    msgtypenotifier.h \
    msgunieditoraddress.h \
    msgunieditorattachment.h \
    msgunieditorbody.h \
    msgunieditorsubject.h \
    msgunieditorview.h \
    msgunifiededitorlineedit.h \
    msgattachmentcontainer.h \
    mmsinsertcheckoperation.h \
    msgbaseview.h \
    msgunieditorimageprocessor.h \
    msgunieditorprocessimageoperation.h 

SOURCES += msgmonitor.cpp \
    msgtypenotifier.cpp \
    msgunieditoraddress.cpp \
    msgunieditorattachment.cpp \
    msgunieditorbody.cpp \
    msgunieditorsubject.cpp \
    msgunieditorview.cpp \
    msgunifiededitorlineedit.cpp \
    msgattachmentcontainer.cpp \
    msgunieditorimageprocessor.inl \
    msgunieditorimageprocessor.cpp \
    msgunieditorprocessimageoperation.cpp \
    mmsinsertcheckoperation.cpp 

RESOURCES += unifiededitor.qrc

# Libs
LIBS += -ls60qconversions \
        -lconvergedmessageutils \
        -lmsguiutils \
        -lunieditorpluginloader \
        -lunieditorutils \
        -lfbscli \
        -limageconversion \
        -lDRMHelper \
        -lbitmaptransforms \
        -lmsgmedia \
        -lapmime \
        -lcentralrepository


        