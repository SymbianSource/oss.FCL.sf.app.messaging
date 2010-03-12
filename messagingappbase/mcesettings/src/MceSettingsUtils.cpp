/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Offers utility functions.
*
*/



// INCLUDE FILES
#include <msvids.h>
#include <msvapi.h>
#include <AknUtils.h>     // AknTextUtils
#include "MceSettingsUtils.h"
#include <featmgr.h>

#include <messagingvariant.hrh>
#include <bldvariant.hrh>

#include <centralrepository.h>
#include <messaginginternalcrkeys.h>

#include <ImumInternalApi.h>
#include <ImumInHealthServices.h>

#include <muiuflags.h>

// CONSTANTS


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// MceSettingsUtils::MemoryInUseOptionL
// This is static function
// Returns ETrue if 'Memory in use' option is enabled in
// mce/settings
// ---------------------------------------------------------
//
TBool MceSettingsUtils::MemoryInUseOptionL( )
    {
#ifdef RD_HIDE_MEMORY_IN_USE
    return EFalse;
#endif // RD_HIDE_MEMORY_IN_USE
    TBool memoryInUse = ETrue;
    if ( FeatureManager::FeatureSupported( KFeatureIdMmc ) )
        {        
        if ( FeatureManager::FeatureSupported( KFeatureIdMmcHotswap ) )
            {
            // if not hotswap phone, then Memory in use is enabled
            // if hotswap phone, check from shared data, if
            // Memory in use is enabled            

            CRepository* repository = NULL;
            TRAPD( ret, repository = CRepository::NewL( KCRUidMuiuVariation ) );
            CleanupStack::PushL( repository );

            TInt featureBitmask = 0;

            if ( ret == KErrNone )
                {
                if ( repository->Get( KMuiuMceFeatures,featureBitmask ) != KErrNone )
                    {
                    memoryInUse = ETrue;
                    }
                else
                    {
                    if ( !( featureBitmask & KMceFeatureIdHotswap ) )
                        {
                        memoryInUse = EFalse;
                        }
                    }
                }
            CleanupStack::Pop( repository );
            delete repository;
            }        
        }
    else
        {
        //no mmc support
        memoryInUse = EFalse;
        }
    return memoryInUse;       
    }


// ---------------------------------------------------------
// MceSettingsUtils::NewMailIndicatorL
// This is static function
// Returns ETrue if new mail indicator setting is supported
// ---------------------------------------------------------
//
TBool MceSettingsUtils::NewMailIndicatorL()
    {    
    TBool newMailIndicator = EFalse;

    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KCRUidMuiuVariation ) );
    CleanupStack::PushL( repository );

    TInt featureBitmask = 0;

    if ( ret == KErrNone )
        {
        if ( repository->Get( KMuiuMceFeatures,featureBitmask ) != KErrNone )
            {
            newMailIndicator = EFalse;
            }
        else
            {
            newMailIndicator = featureBitmask & KMceFeatureIdNewMailIndicator;
            }
        }

    CleanupStack::Pop( repository );
    delete repository;
   
    return newMailIndicator;
    }

// ---------------------------------------------------------
// MceSettingsUtils::CspBitsL
// Returns ETrue if csp bits is supported
// ---------------------------------------------------------
//
TBool MceSettingsUtils::CspBitsL( )
    {
    TBool csp = EFalse;

    CRepository* repository = NULL;
    TRAPD( ret, repository = CRepository::NewL( KCRUidMuiuVariation ) );
    CleanupStack::PushL( repository );

    TInt featureBitmask = 0;

    if ( ret == KErrNone )
        {
        if ( repository->Get( KMuiuMceFeatures,featureBitmask ) != KErrNone )
            {
            csp = EFalse;
            }
        else
            {
            csp = featureBitmask & KMceFeatureIdCSPSupport;
            }
        }

    CleanupStack::Pop( repository );
    delete repository;

    return csp;
    }
    
// ---------------------------------------------------------
// MceUtils::GetHealthyMailboxList
// This is static function
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt MceSettingsUtils::GetHealthyMailboxListL( 
            const MImumInHealthServices& aHealthServices,
            MImumInHealthServices::RMailboxIdArray& aMailboxIdArray,
            TBool aGetImap4,
            TBool aGetPop3,
            TBool aGetSyncML,
            TBool aGetOther )
    {
    TInt64 mailboxFlags = MImumInHealthServices::EFlagGetHealthy;

    if ( aGetImap4 )
        {
        mailboxFlags |= MImumInHealthServices::EFlagIncludeImap4;
        }
    if ( aGetPop3 )
        {
        mailboxFlags |= MImumInHealthServices::EFlagIncludePop3;
        }
    if ( aGetSyncML )
        {
        mailboxFlags |= MImumInHealthServices::EFlagIncludeSyncMl;
        }
    if ( aGetOther )
        {
        mailboxFlags |= MImumInHealthServices::EFlagIncludeOther;
        }
    
    TInt error = aHealthServices.GetMailboxList( 
        aMailboxIdArray,
        mailboxFlags );
    
    return error;
    }

    
//  End of File
