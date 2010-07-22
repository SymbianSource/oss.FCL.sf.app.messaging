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

TEMPLATE = lib
TARGET = test-unidatamodel-vcal-plugin


INCLUDEPATH += .
INCLUDEPATH += ../../../../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

SOURCES += \
	testunidatamodelvcalplugin.cpp

				
# Input
HEADERS += \
	testunidatamodelvcalplugin.h
	
	   	   			 
   
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    TARGET.EPOCALLOWDLLDATA = 1
    BLD_INF_RULES.prj_exports += "vcal1.txt /epoc32/winscw/c/test/vcal1.txt"
    BLD_INF_RULES.prj_exports += "BioMtm.rsc /epoc32/winscw/c/resource/messaging/mtm/BioMtm.rsc"
		}

defBlock = \      
	  "$${LITERAL_HASH}if defined(EABI)" \
	  "DEFFILE  ../eabi/test_unidatamodel_vcal_plugin.def" \
             "$${LITERAL_HASH}else" \
             "DEFFILE  ../bwins/test_unidatamodel_vcal_plugin.def" \
             "$${LITERAL_HASH}endif"
	
MMP_RULES += defBlock

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
	-lbifu \
	-lxqutils
	

