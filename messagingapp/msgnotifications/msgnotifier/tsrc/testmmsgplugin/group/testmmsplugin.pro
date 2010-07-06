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
TARGET = test-mms-plugin

INCLUDEPATH += ../inc
INCLUDEPATH += ../../../inc
#INCLUDEPATH += ../../../../inc

INCLUDEPATH += ../../../../../../../../inc
INCLUDEPATH += ../../../../../../inc
INCLUDEPATH += ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/inc
INCLUDEPATH += ../../../../../msgutils/unidatautils/unidatamodel/inc
INCLUDEPATH += ../../../../../msgutils/unieditorutils/editorgenutils/inc
INCLUDEPATH += ../../../../../msgutils/s60qconversions/inc
INCLUDEPATH += ../../../../../../../../mmsengine/mmsmessage/inc 
INCLUDEPATH += ../../../../../../../../mmsengine/inc
INCLUDEPATH += ../../mmstestbed/inc
INCLUDEPATH +=
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

DEFINES += BUILD_TEST_DLL

CONFIG += hb plugin

SOURCES += \
  ../src/testmmsplugin.cpp \
  ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/src/unieditormmsplugin.cpp \
  ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/src/unieditormmsplugin_p.cpp  
      

# Input
HEADERS += \
  ../inc/testmmsplugin.h \
  ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/inc/unieditormmsplugin.h \
  ../../../../../msgutils/unieditorutils/unieditorplugins/unieditormmsplugin/inc/unieditormmsplugin_p.h 
               
   
SYMBIAN_PLATFORMS = WINSCW ARMV5
    symbian {
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCSTACKSIZE = 0x8000
    TARGET.EPOCHEAPSIZE = 0x1000 0x1F00000
    TARGET.EPOCALLOWDLLDATA = 1
	}

 LIBS += -leuser \
    -lconvergedmessageutils \
    -ls60qconversions \
    -lMsgMedia \
    -leikctl \
    -leditorgenutils \
    -lcone \
    -leikcoctl \
    -leikcore \
    -leikdlg \
    -lmsgs \
    -letext \
    -lmmsserversettings \
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
    -lmmsmessage \  
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
    -lxqutils\ 
    -lmmstestbed
  	

