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

QT += testlib
QT -= gui

TEMPLATE = app
TARGET = test-unidatamodel-vcal-plugin


INCLUDEPATH += .
INCLUDEPATH += ../../../../../../inc
INCLUDEPATH += ../../../../../../../inc
#INCLUDEPATH += ../../../../../s60qconversions/inc
INCLUDEPATH += ../../../../../../../../../../epoc32/include/platform/mw
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

SOURCES += \
	../src/testunidatamodelvcalplugin.cpp

				
# Input
HEADERS += \
	../inc/testunidatamodelvcalplugin.h
	
	   	   			 
   
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    TARGET.EPOCALLOWDLLDATA = 1
    BLD_INF_RULES.prj_exports += "vcal1.txt /epoc32/winscw/c/test/vcal1.txt"
    BLD_INF_RULES.prj_exports += "BioMtm.rsc /epoc32/winscw/c/resource/messaging/mtm/BioMtm.rsc"
    BLD_INF_RULES.prj_exports += "TestUniDataModelVCalPlugin.cfg c:/TestUniDataModelVCalPlugin.cfg"
    BLD_INF_RULES.prj_exports += "TestUniDataModelVCalPlugin.pl c:/TestUniDataModelVCalPlugin.pl"

		}

 LIBS += -leuser \
	-lconvergedmessageutils\
	-lmsgs \
	-lsmcm \
	-lgsmu \
	-letext \
	-lmsgs \
	-lunidatamodelloader\
	-lQtCore \
	-lbioc \
	-lefsrv \
	-lbiodb \
     -lxqutils \
	-lbifu
	

