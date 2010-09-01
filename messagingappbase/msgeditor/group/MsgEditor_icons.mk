#
# Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  MsgEditor_icons.mk#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=\epoc32\data\z
endif

# ----------------------------------------------------------------------------
# TODO: Configure these
# ----------------------------------------------------------------------------

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\msgeditor.mif
HEADERFILENAME=$(HEADERDIR)\msgeditor.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : 
	if exist $(ICONTARGETFILENAME) erase $(ICONTARGETFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# TODO: Configure these.
#
# NOTE 1: DO NOT DEFINE MASK FILE NAMES! They are included automatically by
# MifConv if the mask detph is defined.
#
# NOTE 2: Usually, source paths should not be included in the bitmap
# definitions. MifConv searches for the icons in all icon directories in a
# predefined order, which is currently \s60\icons, \s60\bitmaps2.
# The directory \s60\icons is included in the search only if the feature flag
# __SCALABLE_ICONS is defined.
# ----------------------------------------------------------------------------

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
		/c8,8 qgn_prop_unknown.bmp \
		/c8,1 qgn_prop_folder_sms_tab1.bmp \
		/c8,1 qgn_prop_folder_message_tab1.bmp \
		/c8,1 qgn_prop_folder_message_attach_tab1.bmp \
		/c8,8 qgn_prop_drm_exp_forbid_super \
		/c8,8 qgn_prop_drm_rights_exp_super.bmp \
		/c8,8 qgn_prop_drm_send_forbid_super.bmp \
		/c8,8 qgn_prop_drm_rights_valid_super.bmp \
		/c8,8 qgn_graf_line_message_horizontal.svg \
		/c8,8 qgn_indi_mce_attach.bmp \
		/c8,8 qgn_indi_mce_priority_low.svg \
	    /c8,8 qgn_indi_mce_priority_high.svg \
	    /c8,8 qgn_indi_msg_list_tab1.svg \
	    /c8,8 qgn_indi_navi_arrow_left.svg \
	    /c8,8 qgn_indi_navi_arrow_right.svg


FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
