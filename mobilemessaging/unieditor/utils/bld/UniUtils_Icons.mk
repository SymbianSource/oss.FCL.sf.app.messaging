#
# Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  UniUtils_Icons.mk#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=\epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\uniutils.mif
HEADERFILENAME=$(HEADERDIR)\uniutils.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : 
	if exist $(ICONTARGETFILENAME) erase $(ICONTARGETFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
	/c8,8 qgn_prop_mms_empty_page_sub.bmp \
	/c8,8 qgn_graf_mms_edit.bmp \
	/c8,8 qgn_graf_mms_unedit.bmp \
	/c8,8 qgn_graf_mms_object_corrupt.bmp \
	/c8,1 qgn_graf_mms_inserted_video_edit.bmp \
	/c8,1 qgn_graf_mms_inserted_video_view.bmp \
	/c8,8 qgn_prop_drm_rights_exp_large.bmp \
	/c8,8 qgn_graf_mms_audio_play.bmp \
	/c8,8 qgn_graf_mms_audio_inserted.bmp \
	/c8,8 qgn_graf_mms_audio_corrupted.bmp \
	/c8,8 qgn_graf_mms_audio_drm.bmp \
	/c8,8 qgn_graf_mms_audio_drm_valid_send.bmp \
	/c8,8 qgn_graf_mms_audio_drm_valid_send_forbid.bmp \
	/c8,8 qgn_graf_mms_audio_drm_invalid_send_forbid.bmp \
	/c8,8 qgn_graf_mms_image_corrupted.bmp \
	/c8,8 qgn_graf_mms_image_drm.bmp \
	/c8,8 qgn_graf_mms_image_drm_invalid_send_forbid.bmp \
	/c8,8 qgn_graf_mms_video_corrupted.bmp \
	/c8,8 qgn_graf_mms_video_drm.bmp \
	/c8,8 qgn_graf_mms_video_drm_valid_send.bmp \
	/c8,8 qgn_graf_mms_video_drm_valid_send_forbid.bmp \
	/c8,8 qgn_graf_mms_video_drm_invalid_send_forbid.bmp \
	/c8,8 qgn_indi_mce_priority_low.svg \
	/c8,8 qgn_indi_mce_priority_high.svg \
	/c8,8 qgn_prop_mce_sms_title.svg \
	/c8,8 qgn_prop_mce_mms_title.svg \
	/c8,8 qgn_graf_mms_pres \
	/c8,8 qgn_graf_mms_pres_corrupted \
	/c8,8 qgn_graf_mms_pres_drm_invalid_send \
	/c8,8 qgn_graf_mms_pres_drm_invalid_send_forbid \
	/c24,8 qgn_graf_ring_wait_01.svg \
	/c24,8 qgn_graf_ring_wait_02.svg \
	/c24,8 qgn_graf_ring_wait_03.svg \
	/c24,8 qgn_graf_ring_wait_04.svg \
	/c24,8 qgn_graf_ring_wait_05.svg \
	/c24,8 qgn_graf_ring_wait_06.svg \
	/c24,8 qgn_graf_ring_wait_07.svg \
	/c24,8 qgn_graf_ring_wait_08.svg \
	/c24,8 qgn_graf_ring_wait_09.svg \
	/c24,8 qgn_graf_ring_wait_10.svg
	
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
