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
TARGET = testunidatamodelloader

CONFIG += hb
CONFIG += symbian_test


INCLUDEPATH += inc
INCLUDEPATH += ../../../../../../inc
INCLUDEPATH += ../../../../../../../inc
INCLUDEPATH += ../../../../../../../../../../epoc32/include/platform/mw
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

SOURCES += \
	src/testunidatamodelloader.cpp

				
# Input
HEADERS += \
	inc/testunidatamodelloader.h
	
	 	   			 
   
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    TARGET.EPOCALLOWDLLDATA = 1
    BLD_INF_RULES.prj_exports += "testunidatamodelloader.cfg c:/testunidatamodelloader.cfg"
    BLD_INF_RULES.prj_exports += "data/SmileyFace.gif    	 c:/SmileyFace.gif"
    BLD_INF_RULES.prj_exports += "data/sample.txt            c:/sample.txt"
    		}



packageheader = "$${LITERAL_HASH}{\"QTestUnidataModelLoader\"},(0xEadfc3ef.),1,0,0,TYPE=SA"

vendorinfo = \
			"; Localised Vendor name" \
			"%{\"Nokia\"}" \
			"; Unique Vendor name" \
			":\"Nokia\"" 
			
dependencyinfo = \
				"; Default HW/platform dependencies" \
				"[0x101F7961],0,0,0,{\"S60ProductID\"}" \
				"[0x2001E61C],4,6,3,{"Qt"}
				
default_deployment.pkg_prerules =	packageheader \
									vendorinfo \
									dependencyinfo 


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
	
#Copy the dependent DLL
symbian: {
	addCfg.sources = ./data/testunidatamodelloader.cfg
	addCfg.path = C:/
	DEPLOYMENT += addCfg

	addP2.sources = ./data/SmileyFace.gif
	addP2.path = C:/
	DEPLOYMENT += addP2

	addP3.sources = ./data/sample.txt
	addP3.path = C:/
	DEPLOYMENT += addP3	

	}

