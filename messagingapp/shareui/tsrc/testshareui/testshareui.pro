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

TEMPLATE = app
TARGET = testshareui

DEPENDPATH  += .
INCLUDEPATH += .
INCLUDEPATH += ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

QT += core
DEFINES+=_DEBUG

CONFIG += hb

HEADERS += ./testshareuimainwindow.h \
           ./testshareuiview.h

SOURCES += main.cpp \
		   testshareuimainwindow.cpp \
		   testshareuiview.cpp

LIBS += -lshareui


TARGET.CAPABILITY = ALL -TCB
TARGET.EPOCHEAPSIZE = 0x20000 0x25000
symbian:TARGET.UID3 = 0x2002DD18
