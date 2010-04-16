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

# Platforms
SYMBIAN_PLATFORMS = WINSCW ARMV5

# Build.inf rules

symbian {
    BLD_INF_RULES.prj_exports += "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "./rom/MmsEngine.iby                          CORE_APP_LAYER_IBY_EXPORT_PATH(MmsEngine.iby)" \
    "./clientmtm/inc/mmsnotificationclient.h      |../inc/mmsnotificationclient.h" \
    "./inc/mmsgenutils.h                          |../inc/mmsgenutils.h" \
    "./inc/mmsmsventry.h                          |../inc/mmsmsventry.h" \
    "./inc/mmsmsventry.inl                        |../inc/mmsmsventry.inl" \
    "./inc/mmsattachmentwaiter.h                  |../inc/mmsattachmentwaiter.h" \
    "./inc/mmsattachmenthandler.h                 |../inc/mmsattachmenthandler.h" \
    "./inc/mmscmds.h                              |../inc/mmscmds.h" \
#### MmsHeaders ####
    "./mmsmessage/inc/mmsheaders.h                |../inc/mmsheaders.h"\
    "./mmsmessage/inc/mmsheaders.inl              |../inc/mmsheaders.inl"\
    "./mmsmessage/inc/mmssendingchain.h           |../inc/mmssendingchain.h"\
    "./mmsmessage/inc/mmssendingchain.inl         |../inc/mmssendingchain.inl"\
    "./mmsmessage/inc/mmsdeleteresultarray.h      |../inc/mmsdeleteresultarray.h"\
    "./mmsmessage/inc/mmsdeleteresultarray.inl    |../inc/mmsdeleteresultarray.inl"\
#### xml files containing backup_registration ####
    "./group/backup_registration.xml              /epoc32/data/z/private/100058DB/backup_registration.xml" \
#### loc file needed for rsc generation
    "./loc/mmsui.loc                              APP_LAYER_LOC_EXPORT_PATH(mmsui.loc)" \
#### Export confml and crml files ####
    "./conf/mmsengine.confml                      APP_LAYER_CONFML(mmsengine.confml)" \
    "./conf/mmsengine_100058db.crml               APP_LAYER_CRML(mmsengine_100058db.crml)" \
    "./conf/mmsengine_101fb0d5.crml               APP_LAYER_CRML(mmsengine_101fb0d5.crml)" \
    "./conf/mmsengine_101fb0d9.crml               APP_LAYER_CRML(mmsengine_101fb0d9.crml)" \
    "./conf/messaging.confml APP_LAYER_CONFML(messaging.confml)" \
		"./conf/messaging_101F87EB.crml APP_LAYER_CRML(messaging_101F87EB.crml)" \
		"./conf/messaging_101F87EC.crml APP_LAYER_CRML(messaging_101F87EC.crml)" \
		"./conf/messaging_101F87ED.crml APP_LAYER_CRML(messaging_101F87ED.crml)" \
		"./conf/messaging_101F87EE.crml APP_LAYER_CRML(messaging_101F87EE.crml)" \
		"./conf/messaging_101F87EF.crml APP_LAYER_CRML(messaging_101F87EF.crml)" \
		"./conf/messaging_101F8873.crml APP_LAYER_CRML(messaging_101F8873.crml)" \
		"./conf/messaging_102071F2.crml APP_LAYER_CRML(messaging_102071F2.crml)" \
		"./conf/messaging_102072E2.crml APP_LAYER_CRML(messaging_102072E2.crml)" \
		"./conf/messaging_102072E5.crml APP_LAYER_CRML(messaging_102072E5.crml)" \
		"./conf/messaging_102824A0.crml APP_LAYER_CRML(messaging_102824A0.crml)" \
		"./conf/messaging_1028281E.crml APP_LAYER_CRML(messaging_1028281E.crml)" \
		"./conf/messaging_101F8F28.crml APP_LAYER_CRML(messaging_101F8F28.crml)"
    
    
    
}

symbian {
BLD_INF_RULES.prj_mmpfiles = "./mmsconninit/bld/mmsconninit.mmp" \
                             "./genutils/bld/mmsgenutils.mmp" \
                             "./mmsserversettings/bld/mmsserversettings.mmp" \
                             "./mmsmessage/bld/mmsmessage.mmp" \
                             "./mmshttptransport/bld/mmstransport.mmp" \
                             "./mmscodec/bld/mmscodec.mmp" \
                             "./mmsserver/bld/mmsserver.mmp" \
                             "./clientmtm/bld/mmsclient.mmp" \
                             "./mmswatcher/bld/mmswatcher.mmp" \
                             "./mmspushhandler/bld/mmspushhandler.mmp" \
                             "./applicationadapter/bld/mmsapplicationadapter.mmp" \
                             "./mmscodecclient/bld/mmscodecclient.mmp"
}