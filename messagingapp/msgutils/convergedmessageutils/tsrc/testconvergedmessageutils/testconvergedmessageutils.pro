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
# Description: Test Case Project Definition for ConvergedMessageUtils
#

QT += testlib
QT -= gui

TEMPLATE = app
TARGET = testconvergedmessageutils


INCLUDEPATH += ./inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../mmstestbed/inc
INCLUDEPATH += ../../../../../inc
INCLUDEPATH += ../../../s60qconversions/inc
INCLUDEPATH += ../../../unidatautils/unidatamodel/inc
INCLUDEPATH += ../../../../../../../mw/hb/include/hbcore
INCLUDEPATH += ../../../unieditorutils/editorgenutils/inc
INCLUDEPATH += ../../../unieditorutils/unieditorplugins/unieditormmsplugin/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL
CONFIG += hb plugin

SYMBIAN_PLATFORMS = WINSCW ARMV5

SOURCES += \
  ./src/testconvergedmessageutils.cpp \
  ../../../unieditorutils/unieditorplugins/unieditormmsplugin/src/unieditormmsplugin.cpp \
  ../../../unieditorutils/unieditorplugins/unieditormmsplugin/src/unieditormmsplugin_p.cpp  

# Input
HEADERS += \
  ./inc/testconvergedmessageutils.h \
  ../../../unieditorutils/unieditorplugins/unieditormmsplugin/inc/unieditormmsplugin.h \
  ../../../unieditorutils/unieditorplugins/unieditormmsplugin/inc/unieditormmsplugin_p.h 
   
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.CAPABILITY = All -TCB -DRM
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    BLD_INF_RULES.prj_exports += "testconvergedmessageutils.cfg c:/testconvergedmessageutils.cfg"
    BLD_INF_RULES.prj_exports += "testconvergedmessageutils.pl c:/testconvergedmessageutils.pl"
	}
	
LIBS += -leuser \
    -lconvergedmessageutils \
    -ls60qconversions \
    -lMsgMedia \
    -leditorgenutils \
    -lcone \
    -leikcoctl \
    -leikcore \
    -leikdlg \
    -lmsgs \
    -letext \
    -lgsmu \
    -lmmsgenutils \
    -lefsrv \
    -lestor \
    -lsmcm \
    -lCommonEngine \
    -lbafl \
    -lCdlEngine \
    -lFeatMgr \
    -lapmime \
    -lapgrfx \
    -lcharconv \
    -lInetProtUtil \
		-lsmildtd \  
		-lxmldom \
  	-lxmlparser \
  	-lcone \
  	-lQtCore \
  	-letel \
  	-lcommdb \
  	-lcommsdat \
  	-letelmm \
  	-lunidatamodelloader \
  	-lunidatamodel \
  	-lavkon \
  	-leikcoctl \
  	-leikctl \
  	-lform \
  	-luiklaf\ 
  	-lmmstestbed \
        -lmmsmessage \ 
	-lmmsserversettings \
	-lxqutils