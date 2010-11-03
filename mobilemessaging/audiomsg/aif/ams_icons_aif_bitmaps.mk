#
# Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:   scalable icons makefile for project audiomessage
#


ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\SYSTEM\APPS\audiomessage
ICONTARGETFILENAME=$(TARGETDIR)\audiomessage_aif.mbm


MAKMAKE : ;

RESOURCE :
	mifconv $(ICONTARGETFILENAME) \
		/c8,8 qgn_prop_mce_audio_title.bmp \
		/c8,8 qgn_prop_mce_audio_title_mask_soft.bmp

BLD : ;

CLEAN : ;

LIB : ;

CLEANLIB : ;

RESOURCE : ;

FREEZE : ;

SAVESPACE : ;

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : ;



