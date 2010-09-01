#
# Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
#       Make copy of SMUM.DLL from WINS\UDEB\Z\SYSTEM\LIBS
#       to WINS\UDEB, same for UREL
#

TARGETDIRUDEB=\EPOC32\release\$(PLATFORM)\udeb
TARGETDIRUREL=\EPOC32\release\$(PLATFORM)\urel
SOURCEDIRUDEB=\EPOC32\release\$(PLATFORM)\udeb\z\system\libs
SOURCEDIRUREL=\EPOC32\release\$(PLATFORM)\urel\z\system\libs

COPYFILE=SMUM.DLL

SMUM_DLL_COPY_UDEB=$(TARGETDIRUDEB)\\$(COPYFILE)
SMUM_DLL_COPY_UREL=$(TARGETDIRUREL)\\$(COPYFILE)

$(TARGETDIRUDEB) :
	@if not exist "\EPOC32" md "\EPOC32"
	@if not exist "\EPOC32\release" md "\EPOC32\release"
	@if not exist "\EPOC32\release\$(PLATFORM)\" md "\EPOC32\release\$(PLATFORM)"
	@if not exist "\EPOC32\release\$(PLATFORM)\udeb" md "\EPOC32\release\$(PLATFORM)\udeb"


$(TARGETDIRUREL) :
	@if not exist "\EPOC32" md "\EPOC32"
	@if not exist "\EPOC32\release" md "\EPOC32\release"
	@if not exist "\EPOC32\release\$(PLATFORM)\" md "\EPOC32\release\$(PLATFORM)"
	@if not exist "\EPOC32\release\$(PLATFORM)\urel" md "\EPOC32\release\$(PLATFORM)\urel"


#
# CFG could be UDEB, UREL or ALL...
#
$(SMUM_DLL_COPY_UDEB) : $(TARGETDIRUDEB)
	@if exist $(SOURCEDIRUDEB)\\$(COPYFILE) copy $(SOURCEDIRUDEB)\\$(COPYFILE) $(TARGETDIRUDEB)

$(SMUM_DLL_COPY_UREL) : $(TARGETDIRUREL)
	@if exist $(SOURCEDIRUREL)\\$(COPYFILE) copy $(SOURCEDIRUREL)\\$(COPYFILE) $(TARGETDIRUREL)

do_nothing:
	rem do nothing

#
# The targets invoked by bld...
#

MAKMAKE : do_nothing

BLD : $(SMUM_DLL_COPY_UDEB) $(SMUM_DLL_COPY_UREL)

SAVESPACE : BLD

CLEAN :
	@if exist $(TARGETDIRUDEB)\\$(COPYFILE) erase $(TARGETDIRUDEB)\\$(COPYFILE)
	@if exist $(TARGETDIRUREL)\\$(COPYFILE) erase $(TARGETDIRUREL)\\$(COPYFILE)

FREEZE : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE : do_nothing

FINAL : do_nothing

RELEASABLES :
	@echo $(SMUM_DLL_COPY_UDEB) $(SMUM_DLL_COPY_UREL)
