#
# Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  MmsUi_Icons.mk#

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=\epoc32\release\$(PLATFORM)\$(CFG)\z
else
ZDIR=\epoc32\data\z
endif

# ----------------------------------------------------------------------------
# TODO: Configure these
# ----------------------------------------------------------------------------

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=\epoc32\include
ICONTARGETFILENAME=$(TARGETDIR)\mmsui.mif
HEADERFILENAME=$(HEADERDIR)\mmsui.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN :
	if exist $(ICONTARGETFILENAME) erase $(ICONTARGETFILENAME)
	if exist $(HEADERFILENAME) erase $(HEADERFILENAME)

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
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
#// NOTE: Order of bitmaps is important. 
#//
#//  TRANSITIONS
#//  -----------
#//      qgn_graf_mms_trans_snake must be first transition
#//      qgn_graf_mms_trans_none must be last transition
#//      Transitions must be in single block
#//
#//  EDITOR ICONS
#//  ------------
#//  

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) \
	/c8,8 qgn_graf_mms_play.bmp \
	/c8,8 qgn_indi_mce_priority_low.svg \
	/c8,8 qgn_indi_mce_priority_high.svg \
	/c8,8 qgn_prop_mce_mms_title \
	/c8,8 qgn_indi_tb_retrieve_mms.svg
	
	
FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
