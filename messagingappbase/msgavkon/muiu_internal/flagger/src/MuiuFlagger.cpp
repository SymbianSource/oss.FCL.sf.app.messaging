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
* Description:  MuiuFlagger implementation
*
*/




// INCLUDE FILES
#include <e32base.h>        
#include <featmgr.h>            // Feature Manager
#include <centralrepository.h>  // CRepository
#include "MuiuFlagger.h"        // CMuiuFlags


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
const TInt KMuiuFlaggerMaxFlags = 64;
_LIT( KMuiuFlaggerPanic, "MuiuFlagger" );

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ==============================


// ------------------------------ CONSTRUCTORS --------------------------------


// ----------------------------------------------------------------------------
// CMuiuFlags::CMuiuFlags()
// ----------------------------------------------------------------------------
//
CMuiuFlags::CMuiuFlags()
    {    
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::~CMuiuFlags()
// ----------------------------------------------------------------------------
//
EXPORT_C CMuiuFlags::~CMuiuFlags()
    {
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::NewL()
// ----------------------------------------------------------------------------
//
EXPORT_C CMuiuFlags* CMuiuFlags::NewL(
    const TMuiuGlobalFeatureArray* aGlobalFeatures,
    const TMuiuLocalFeatureArray* aLocalFeatures,
    const TMuiuFlagArray* aFlags )
    {
    CMuiuFlags* self = NewLC( aGlobalFeatures, aLocalFeatures, aFlags );
    CleanupStack::Pop( self );
    
    return self;
    }
    
// ----------------------------------------------------------------------------
// CMuiuFlags::NewLC()
// ----------------------------------------------------------------------------
//
EXPORT_C CMuiuFlags* CMuiuFlags::NewLC(
    const TMuiuGlobalFeatureArray* aGlobalFeatures,
    const TMuiuLocalFeatureArray* aLocalFeatures,
    const TMuiuFlagArray* aFlags )
    { 
    CMuiuFlags* self = new ( ELeave ) CMuiuFlags();  
    
    CleanupStack::PushL( self );
    self->ConstructL( aGlobalFeatures, aLocalFeatures, aFlags );
    
    return self;
    }    

        
// ----------------------------------------------------------------------------
// CMuiuFlags::ConstructL()
// ----------------------------------------------------------------------------
//
void CMuiuFlags::ConstructL(
    const TMuiuGlobalFeatureArray* aGlobalFeatures,
    const TMuiuLocalFeatureArray* aLocalFeatures,
    const TMuiuFlagArray* aFlags )
    {
    if ( aGlobalFeatures )
        {
        PrepareGlobalFeaturesL( *aGlobalFeatures );
        }
    
    // Local features
    if ( aLocalFeatures )
        {
        PrepareLocalFeaturesL( *aLocalFeatures );
        }

    // Normal flags
    if ( aFlags )
        {
        PrepareGeneralFlags( *aFlags );
        }
    }
    
    
// ----------------------------- FLAG HANDLERS --------------------------------
    
        
// ----------------------------------------------------------------------------
// CMuiuFlags::PrepareGlobalFeaturesL()
// ----------------------------------------------------------------------------
//    
void CMuiuFlags::PrepareGlobalFeaturesL( 
    const TMuiuGlobalFeatureArray& aGlobalFeatures )
    {
    TUint64 feature = aGlobalFeatures.Count();
    
    // Compare the number of given features against the maximum number
    // of featureflags possible. Panic if the number of flags has been 
    // exceeded.
    __ASSERT_ALWAYS( 
        feature <= KMuiuFlaggerMaxFlags, 
        User::Panic( KMuiuFlaggerPanic, KErrOverflow ) );
    
    FeatureManager::InitializeLibL();
    
    // Check all the features in array from feature manager and set
    // the flags in the same order as in the array
    while ( feature-- > 0 )
        {
        ChangeGF( feature, 
            FeatureManager::FeatureSupported( 
                aGlobalFeatures[feature] ) );      
        }
              
    FeatureManager::UnInitializeLib();
    }

// ----------------------------------------------------------------------------
// CMuiuFlags::PrepareLocalFeaturesL()
// ----------------------------------------------------------------------------
//
void CMuiuFlags::PrepareLocalFeaturesL(
    const TMuiuLocalFeatureArray& aLocalFeatures )
    {
    TUint64 feature = aLocalFeatures.Count();
    
    // Compare the number of given features against the maximum number
    // of featureflags possible. Panic if the number of flags has been 
    // exceeded.
    __ASSERT_ALWAYS( 
        feature <= KMuiuFlaggerMaxFlags, 
        User::Panic( KMuiuFlaggerPanic, KErrOverflow ) );              
    
    TInt value = 0;
    
    // Get Next CenRep session
    while ( feature-- > 0 )
        {
        // Create temporary structure for the feature
        TMuiuLocalFeatureItem localFeature = aLocalFeatures[feature];
        
        // Create the connection
        CRepository* cenrep = 
            CRepository::NewLC( localFeature.iUid );
            
        // Get the value from repository
        User::LeaveIfError( 
            cenrep->Get( localFeature.iKeyId, value ) );
                    
        // Define the feature flag
        if ( !localFeature.iIsFlag )
            {
            // The value in iFlag is actually an index, so roll on to right
            // and mask the other flags
            ChangeLF( feature, ( value >> localFeature.iFlag ) & 0x01 );
            }
        else
            {
            // The value in iFlag is a real flag, so do the normal 
            // and -operation and check if the bit is on
            ChangeLF( feature, ( value & localFeature.iFlag ) > 0 );
            }            
                            
        // Destroy the object
        CleanupStack::PopAndDestroy( cenrep );
        cenrep = NULL;
        }         
    }
    

// ----------------------------------------------------------------------------
// CMuiuFlags::PrepareGeneralFlags()
// ----------------------------------------------------------------------------
//
void CMuiuFlags::PrepareGeneralFlags(
    const TMuiuFlagArray& aFlags )
    {
    TUint64 feature = aFlags.Count();
    
    // Compare the number of given features against the maximum number
    // of featureflags possible. Panic if the number of flags has been 
    // exceeded.
    __ASSERT_ALWAYS( 
        feature <= KMuiuFlaggerMaxFlags, 
        User::Panic( KMuiuFlaggerPanic, KErrOverflow ) );  
        
    // Check all the flags in array and set the flags in the 
    // same order as in the array
    while ( feature-- > 0 )
        {
        ChangeFlag( feature, 
            FeatureManager::FeatureSupported( 
                aFlags[feature] ) );
        }
    }    
    
    
// End of File


