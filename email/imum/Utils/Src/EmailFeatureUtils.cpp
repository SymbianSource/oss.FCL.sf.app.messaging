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
*       Static functions for imum
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <centralrepository.h>          // CRepository
#include <muiuflagger.h>                // CMuiuFlags
#include <featmgr.h>

#include "EmailFeatureUtils.h"
#include "ImumUtilsLogging.h"
#include <SettingEnforcementInfo.h>

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// MsvEmailMtmUiFeatureUtils::LocalFeatureL()
// ----------------------------------------------------------------------------
//
EXPORT_C TBool MsvEmailMtmUiFeatureUtils::LocalFeatureL(
    const TUid& aCenRepUid,
    const TUint32 aKeyId,
    const TUint32 aFlag )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiFeatureUtils::LocalFeatureL, 0, utils, KLogUi );

    // Create the connection to Central Repository
    CRepository* cenrep = CRepository::NewLC( aCenRepUid );

    // Fetch the value from the
    TInt flags;
    TInt error = cenrep->Get( aKeyId, flags );

    // Remove the connection
    CleanupStack::PopAndDestroy( cenrep );
    cenrep = NULL;

    // Return the result as a boolean value
    return ( flags & aFlag ) == aFlag;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiFeatureUtils::FetchGlobalEmailFeaturesL()
// ----------------------------------------------------------------------------
//
EXPORT_C void MsvEmailMtmUiFeatureUtils::FetchGlobalEmailFeaturesL(
    TMuiuGlobalFeatureArray& aFeatureArray )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiFeatureUtils::FetchGlobalEmailFeaturesL, 0, utils, KLogUi );

    // Get global features
    // EMailFeatureAlwaysOnlineEmail
    aFeatureArray.Append( KFeatureIdAlwaysOnLineEmail );
    // EMailFeatureProtocolWlan
    aFeatureArray.Append( KFeatureIdProtocolWlan );
    // EMailFeatureCsd
    aFeatureArray.Append( KFeatureIdAppCsdSupport );
    // EMailFeatureHelp
    aFeatureArray.Append( KFeatureIdHelp );
    // EMailFeatureOfflineMode
    aFeatureArray.Append( KFeatureIdOfflineMode );
    // EMailFeatureMeetingRequestSupport
    aFeatureArray.Append( KFeatureIdMeetingRequestSupport );
    // EMailFeatureOmaEmn
    aFeatureArray.Append( KFeatureIdOmaEmailNotifications );

    if( SettingEnforcementStateL() )
        {
        aFeatureArray.Append( KFeatureIdSapPolicyManagement );
        }
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiFeatureUtils::FetchLocalEmailFeaturesL()
// ----------------------------------------------------------------------------
//
EXPORT_C void MsvEmailMtmUiFeatureUtils::FetchLocalEmailFeaturesL(
    TMuiuLocalFeatureArray& aFeatureArray )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiFeatureUtils::FetchLocalEmailFeaturesL, 0, utils, KLogUi );

    // EMailFeatureAlwaysOnlineCSD
    TMuiuLocalFeatureItem temp = {
        KCRUidMuiuVariation, KMuiuEmailConfigFlags,
        KEmailFeatureIdAlwaysOnline, ETrue };
    aFeatureArray.Append( temp );

    // EmailFeatureAlwaysonlineHeaders
    temp.iFlag = KEmailFeatureIdAlwaysonlineHeaders;
    aFeatureArray.Append( temp );
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiFeatureUtils::EmailFeaturesLC()
// ----------------------------------------------------------------------------
//
EXPORT_C CMuiuFlags* MsvEmailMtmUiFeatureUtils::EmailFeaturesLC(
    const TBool aGetGlobals,
    const TBool aGetLocals )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiFeatureUtils::EmailFeaturesLC, 0, utils, KLogUi );

    TMuiuGlobalFeatureArray globals;
    TMuiuLocalFeatureArray locals;

    if ( aGetGlobals )
        {
        FetchGlobalEmailFeaturesL( globals );
        }

    if ( aGetLocals )
        {
        FetchLocalEmailFeaturesL( locals );
        }

    CMuiuFlags* flags = CMuiuFlags::NewLC( &globals, &locals );
    globals.Reset();
    locals.Reset();

    return flags;
    }

// ----------------------------------------------------------------------------
// MsvEmailMtmUiFeatureUtils::EmailFeaturesL()
// ----------------------------------------------------------------------------
//
EXPORT_C CMuiuFlags* MsvEmailMtmUiFeatureUtils::EmailFeaturesL(
    const TBool aGetGlobals,
    const TBool aGetLocals )
    {
    IMUM_STATIC_CONTEXT( MsvEmailMtmUiFeatureUtils::EmailFeaturesL, 0, utils, KLogUi );

    CMuiuFlags* flags = EmailFeaturesLC( aGetGlobals, aGetLocals );
    CleanupStack::Pop( flags );

    return flags;
    }

// ---------------------------------------------------------
// MsvEmailMtmUiUtils::SettingEnforcementStateL
// Checks if setting enforcement is activated.
// ---------------------------------------------------------
TBool MsvEmailMtmUiFeatureUtils::SettingEnforcementStateL()
    {
    FeatureManager::InitializeLibL();
    TBool sapPolicy = 
        FeatureManager::FeatureSupported( KFeatureIdSapPolicyManagement );
    FeatureManager::UnInitializeLib();
    if( !sapPolicy )
        {
        return EFalse;
        }
    
    CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
    CleanupStack::PushL(info);

    TBool active = EFalse;
    User::LeaveIfError(info->EnforcementActive( EEMailEnforcement, active ) );
    CleanupStack::PopAndDestroy(info);
    info = NULL;
    return active;
    }
    
// End of File

