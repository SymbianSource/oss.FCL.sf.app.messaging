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

QT -= gui

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules

symbian {
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "./rom/csmsg.iby             CORE_APP_LAYER_IBY_EXPORT_PATH(csmsg.iby)"
}

symbian {
BLD_INF_RULES.prj_mmpfiles = "./group/csmsg.mmp"
}
