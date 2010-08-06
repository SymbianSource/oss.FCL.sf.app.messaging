#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules
symbian{
		BLD_INF_RULES.prj_exports += \
    "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "messaging.confml APP_LAYER_CONFML(messaging.confml)" \
    "CI_messaging.confml APP_LAYER_CONFML(CI_messaging.confml)" \
		"messaging_101F87EB.crml APP_LAYER_CRML(messaging_101F87EB.crml)" \
		"messaging_101F87EC.crml APP_LAYER_CRML(messaging_101F87EC.crml)" \
		"messaging_101F87ED.crml APP_LAYER_CRML(messaging_101F87ED.crml)" \
		"messaging_101F87EE.crml APP_LAYER_CRML(messaging_101F87EE.crml)" \
		"messaging_101F87EF.crml APP_LAYER_CRML(messaging_101F87EF.crml)" \
		"messaging_101F8873.crml APP_LAYER_CRML(messaging_101F8873.crml)" \
		"messaging_102071F2.crml APP_LAYER_CRML(messaging_102071F2.crml)" \
		"messaging_102072E2.crml APP_LAYER_CRML(messaging_102072E2.crml)" \
		"messaging_102072E5.crml APP_LAYER_CRML(messaging_102072E5.crml)" \
		"messaging_102824A0.crml APP_LAYER_CRML(messaging_102824A0.crml)" \
		"messaging_1028281E.crml APP_LAYER_CRML(messaging_1028281E.crml)" \
		"messaging_101F8F28.crml APP_LAYER_CRML(messaging_101F8F28.crml)" \
		"backup_registration.xml z:/private/2001FE79/backup_registration.xml"
		}

