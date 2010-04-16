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
TARGET = $$qtLibraryTarget(unimmsdataplugin)

TARGET.CAPABILITY = All -TCB
TARGET.EPOCALLOWDLLDATA = 1

#UID 3
TARGET.UID3 = 0xE01F5465 #need to get a uid 

DEPENDPATH += . inc src

INCLUDEPATH += ./inc
INCLUDEPATH += ../../../msgutils/s60qconversions/inc
INCLUDEPATH += ../../unieditorutils/inc
INCLUDEPATH += ../../../../inc
INCLUDEPATH += ../../../../mmsengine/mmsmessage/inc 
INCLUDEPATH += ../../../../mmsengine/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

SYMBIAN_PLATFORMS = WINSCW ARMV5

CONFIG += plugin

MMP_RULES += "SOURCEPATH ."
MMP_RULES += "TARGETPATH resource"
MMP_RULES += "RESOURCE UniDataModel.rss"

# Input
HEADERS += inc/sessioneventhandler.h \
	inc/unimmsdataplugin.h \
	inc/unimmsdataplugin_p.h \
	inc/UniDataUtils.h \
        inc/UniDataModel.h \
        inc/UniDrmInfo.h \
	inc/UniMimeInfo.h \
	inc/UniObject.h \
	inc/UniObjectList.h \
	inc/UniTextObject.h \
	inc/UniSmilUtils.h \
	inc/UniSmilList.h \
	inc/UniSmilModel.h \
	inc/UniSmilParams.h \
	inc/UniSmilSlide.h \
	inc/ConformanceChecker.h \
	inc/MsgAttachmentUtils.h \
	inc/UniModelConst.h \
	inc/smilliterals.h \
	inc/UniSmil.rh \
	inc/UniSmil.hrh
	
	   	   
		   
SOURCES += src/sessioneventhandler.cpp \
	src/unimmsdataplugin.cpp \
	src/unimmsdataplugin_p.cpp \
	inc/UniDataUtils.inl \
	src/UniDataUtils.cpp \
	inc/UniDataModel.inl \
	src/UniDataModel.cpp \
	inc/UniDrmInfo.inl \
	src/UniDrmInfo.cpp \
	inc/UniMimeInfo.inl \
	src/UniMimeInfo.cpp \
	inc/UniObject.inl \
	src/UniObject.cpp \
	inc/UniObjectList.inl \
	src/UniObjectList.cpp \
	inc/UniTextObject.inl \
	src/UniTextObject.cpp \
	src/UniSmilUtils.cpp \
	inc/UniSmilList.inl \
	src/UniSmilList.cpp \
	inc/UniSmilModel.inl \
	src/UniSmilModel.cpp \
	inc/UniSmilParams.inl \
	src/UniSmilParams.cpp \
	src/UniSmilSlide.cpp \
	src/MsgAttachmentUtils.cpp \
	src/ConformanceChecker.cpp
 
# Build.inf rules 
BLD_INF_RULES.prj_exports += \
  "$${LITERAL_HASH}include <platform_paths.hrh>" \
  "rom/unimmsdataplugin.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(unimmsdataplugin.iby)" \
	
    
 LIBS += -leuser \
	-lbafl \
	-lestor \
	-lconvergedmessageutils\
	-ls60qconversions \
	-lmsgs \
	-letext \
	-leikctl \
	-lQtCore \
	-lmmscli \
	-lefsrv \
	-lmmsmessage \
	-lmmsserversettings \
	-lcone \
	-lapmime \
	-leikcoctl \
	-lInetProtUtil \
	-lxmlparser \
	-lxmldom \
	-lmsgmedia \
	-ldrmcommon \
	-ldrmrights \
	-ldrmhelper \
	-lcaf \
        -lDrmServerInterfaces \
	-lmsgcommonutils \
	-lcharconv \
	-lunieditorutils
	
# plugin stub deployment
plugin.sources = unimmsdataplugin.dll
plugin.path = \resource\qt\plugins\messaging\datamodel
DEPLOYMENT += plugin


