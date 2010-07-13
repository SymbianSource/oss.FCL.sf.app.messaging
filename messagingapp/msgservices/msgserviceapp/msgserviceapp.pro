# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = app
TARGET = msgserviceapp

CONFIG += hb service

DEPENDPATH += . inc src
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

INCLUDEPATH += inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../smartmessaging/ringbc/inc
INCLUDEPATH += ../../msgutils/unidatamodelloader/inc
INCLUDEPATH += ../../msgui/inc
INCLUDEPATH += ../../msgui/unifiedviewer/inc
INCLUDEPATH += ../../msgui/unifiededitor/inc
INCLUDEPATH += ../../msgsettings/settingsview/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input
HEADERS += inc/msgviewinterface.h \
	         inc/msgservicewindow.h \
	         inc/msgstorehandler.h \
	         inc/msgsendinterface.h \
			 inc/msgshareuiinterface.h \
			 inc/msgserviceviewmanager.h
			
SOURCES += src/main.cpp \
           src/msgviewinterface.cpp \
           src/msgservicewindow.cpp \
           src/msgstorehandler.cpp \
           src/msgsendinterface.cpp \
		   src/msgshareuiinterface.cpp \
		   src/msgserviceviewmanager.cpp
			
# Capability
TARGET.CAPABILITY = ALL -TCB
TARGET.UID3 = 0x2002E6DA

LIBS += -lxqservice \
        -lxqserviceutil \
        -lconvergedmessageutils \
        -lmsgs \
        -lringbc \
        -lunidatamodelloader \
        -lunifiedviewer \
        -lunifiededitor \
        -lsettingsview
	

BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
                             ".\rom\msgserviceapp.iby CORE_APP_LAYER_IBY_EXPORT_PATH(msgserviceapp.iby)"
