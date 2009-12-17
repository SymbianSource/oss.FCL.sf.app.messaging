/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*       Static feature handling functions for imum
*
*/


#ifndef __EMAILFEATUREUTILS_H__
#define __EMAILFEATUREUTILS_H__

// INCLUDES
#include <e32base.h>
#include <muiuflagger.h>        // CMuiuFlags
#include <messaginginternalcrkeys.h>
#include <bldvariant.hrh>
#include <messagingvariant.hrh>

// GLOBAL FEATURES
// This enumeration contains all the global features as a flag index
// that can be used in any of the objects under email. Make sure
// that the new features are presented in same order as the indexes
// are here in FetchGlobalFeaturesL method.
// These flags can be found from
// \epoc32\include\oem\bldvariant.hrh
enum TIMSGlobalFeatures
    {
    // KFeatureIdAlwaysOnLineEmail
    EMailFeatureAlwaysOnlineEmail = 0,
    // KFeatureIdProtocolWlan
    EMailFeatureProtocolWlan,
    // KFeatureIdAppCsdSupport
    EMailFeatureCsd,
    // KFeatureIdHelp
    EMailFeatureHelp,
    // KFeatureIdOfflineMode
    EMailFeatureOfflineMode,
    // KFeatureIdMeetingRequestSupport
    EMailFeatureMeetingRequestSupport,
    // KFeatureIdOmaEmailNotifications
    EMailFeatureOmaEmn,
    // KFeatureIdSapPolicyManagement
    EMailFeatureIdSapPolicyManagement,
    // Last global feature
    EMailLastGlobal
    };

// LOCAL FEATURES
// This enumeration contains all the local features as a flag index
// that can be used in any of the objects under email. Make sure
// that the new features are presented in same order as the indexes
// are here in FetchGlobalFeaturesL method.
// These flags can be found from \epoc32\include\oem\MessagingVariant.hrh
enum TIMSLocalFeatures
    {
    // KEmailFeatureIdAlwaysOnlineCSD
    EMailFeatureAlwaysOnlineCSD = 0,
    // KEmailFeatureIdAlwaysonlineHeaders
    EmailFeatureAlwaysonlineHeaders,
    // Last local feature
    EMailLastLocal
    };

/**
*  Static functions for feature handling in imum
*
*  @lib imum.lib
*  @since S60 3.0
*/
class MsvEmailMtmUiFeatureUtils
    {
    public: // New functions

        /**
        * Checks if given local feature is turned on
        * @since S60 3.0
        */
        IMPORT_C static TBool LocalFeatureL(
            const TUid& aCenRepUid,
            const TUint32 aKeyId,
            const TUint32 aFlag );

        /**
        *
        * @since S60 3.0
        */
        IMPORT_C static void FetchGlobalEmailFeaturesL(
            TMuiuGlobalFeatureArray& aFeatureArray );

        /**
        *
        * @since S60 3.0
        */
        IMPORT_C static void FetchLocalEmailFeaturesL(
            TMuiuLocalFeatureArray& aFeatureArray );

        /**
        *
        * @since S60 3.0
        */
        IMPORT_C static CMuiuFlags* EmailFeaturesLC(
            const TBool aGetGlobals = ETrue,
            const TBool aGetLocals = EFalse );
            
        /**
        *
        * @since S60 3.2
        */
        IMPORT_C static CMuiuFlags* EmailFeaturesL(
            const TBool aGetGlobals = ETrue,
            const TBool aGetLocals = EFalse );     
            
        /**
        * Checks if TARM lock is set active. 
        * @since S60 3.2
        * @return ETrue email settings are locked.
        */                
        static TBool SettingEnforcementStateL();                
                   
    };

#endif // __EMAILFEATUREUTILS_H__

