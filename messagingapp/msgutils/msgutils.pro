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

TEMPLATE = subdirs

CONFIG += ordered

SYMBIAN_PLATFORMS = WINSCW ARMV5

SUBDIRS += s60qconversions/s60qconversions.pro
SUBDIRS += convergedmessageutils/convergedmessageutils.pro
SUBDIRS += unieditorutils/unieditorutils.pro
SUBDIRS += unidatautils/unidatautils.pro