#
# Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:  Postcard_icons_dc.mk#

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
ICONTARGETFILENAME=$(TARGETDIR)\postcard.mif
HEADERFILENAME=$(HEADERDIR)\postcard.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN :
	if exist $(ICONTARGETFILENAME) erase $(ICONTARGETFILENAME) if exist $(HEADERFILENAME) erase $(HEADERFILENAME)


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
	mifconv $(ICONTARGETFILENAME)  /h$(HEADERFILENAME) \
/c8,8 qgn_graf_mms_postcard_back.svg\
/c8,8 qgn_graf_mms_postcard_front.svg\
/c8,8 qgn_graf_mms_postcard_insert_image_bg.svg\
/c8,8 qgn_indi_mms_postcard_down.svg\
/c8,8 qgn_indi_mms_postcard_image.svg\
/c8,8 qgn_indi_mms_postcard_stamp.svg\
/c8,8 qgn_indi_mms_postcard_text.svg\
/c8,8 qgn_indi_mms_postcard_up.svg\
/c8,8 qgn_prop_mce_postcard.svg\
/c8,8 qgn_prop_mce_postcard_title.svg\
/c8,8 qgn_prop_mms_postcard_address_active.svg\
/c8,8 qgn_prop_mms_postcard_address_inactive.svg\
/c8,8 qgn_prop_mms_postcard_greeting_active.svg\
/c8,8 qgn_prop_mms_postcard_greeting_inactive.svg\
/c8,8 qgn_graf_mms_insert_image.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
		@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
