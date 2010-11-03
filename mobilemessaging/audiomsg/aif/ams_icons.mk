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
# Description:   icons makefile for project audiomessage
#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=$(EPOCROOT)epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\audiomessage.mif
HEADERFILENAME=$(HEADERDIR)\audiomessage.mbg

MAKMAKE : ;
RESOURCE :
	mifconv $(ICONTARGETFILENAME)  /h$(HEADERFILENAME) \
		/c8,8 qgn_prop_mce_audio_title.svg\
		/c8,8 qgn_indi_mms_audio_record.svg\
		/c8,8 qgn_indi_mms_audio_pause.svg\
		/c8,8 qgn_indi_mms_audio_play.svg\
		/c8,8 qgn_graf_mms_audio_insert.bmp\
		/c8,8 qgn_graf_mms_audio_inserted.bmp\
		/c8,8 qgn_graf_mms_audio_corrupted.bmp\
		/c8,8 qgn_indi_mce_priority_high.svg\
		/c8,8 qgn_indi_mce_priority_low.svg\
		/c8,8 qgn_indi_cams_tb_rec.svg

BLD : ;

CLEAN : ;

LIB : ;

CLEANLIB : ;


FREEZE : ;

SAVESPACE : ;

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : ;

