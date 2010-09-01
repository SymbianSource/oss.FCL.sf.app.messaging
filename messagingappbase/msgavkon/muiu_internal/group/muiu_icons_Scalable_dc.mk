#
# Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
# Description:   Icon makefile for muiu_internal
#

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
ICONTARGETFILENAME=$(TARGETDIR)\muiu.mif
HEADERFILENAME=$(HEADERDIR)\muiu.mbg

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
# Muiu icons
# ----------------------------------------------------------------------------

RESOURCE :
	mifconv $(ICONTARGETFILENAME) /h$(HEADERFILENAME) /Ficonlist_scalable.txt




FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(HEADERFILENAME)&& \
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
