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
* Description:   Calls voice call or VoIP call to either of addresses provided. 
*
*/



#include <eikenv.h>
#include <coemain.h>
#include <ItemFinder.h>
#include <msgvoipextension.h>
#include <commonphoneparser.h>      // Common phone number validity checker

#include "MuiuContactService.h"
#include "MuiuContactAddressMatcher.h"
    
// ---------------------------------------------------------
// CMuiuContactService::NewL
// ---------------------------------------------------------
EXPORT_C CMuiuContactService* CMuiuContactService::NewL( 
        CEikonEnv&          aEnv,
        CMsgVoIPExtension*  aVoipExtension ) 
    {
    CMuiuContactService* self =
        new( ELeave ) CMuiuContactService( aEnv, aVoipExtension );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------
// CMuiuContactService::CMuiuContactService
// ---------------------------------------------------------
CMuiuContactService::CMuiuContactService(   CEikonEnv&          aEnv,
                                            CMsgVoIPExtension*  aVoipExtension ):
    iEikEnv( aEnv ),
    iVoipExtension( aVoipExtension )
    {
    }


// ---------------------------------------------------------
// CMuiuContactService::ConstructL
// ---------------------------------------------------------
void CMuiuContactService::ConstructL()
    {
    if ( !iVoipExtension )
        {
        iVoipExtension = CMsgVoIPExtension::NewL( );
        }
    }

// ---------------------------------------------------------
// CMuiuContactService::~CMuiuContactService
// ---------------------------------------------------------
EXPORT_C CMuiuContactService::~CMuiuContactService()
    {
    delete iVoipExtension;
    delete iContactAddressMatcher;
    delete iMatchAddress;
    delete iMatchName;
    }

// ---------------------------------------------------------
// CMuiuContactService::PreferredCallTypeL
// ---------------------------------------------------------
EXPORT_C CMuiuContactService::TMuiuPreferredCallType CMuiuContactService::PreferredCallTypeL() const
    {
    CMuiuContactService::TMuiuPreferredCallType preferredCallType( EMuiuCallTypeVoice );
    if (    iVoipExtension
        &&  iVoipExtension->IsPreferredTelephonyVoIP( )
        &&  iVoipExtension->VoIPProfilesExistL() )
        {
        preferredCallType = EMuiuCallTypeVoip;
        }
    return preferredCallType;
    }

// ---------------------------------------------------------
// CMuiuContactService::SetAddress
// ---------------------------------------------------------
EXPORT_C void CMuiuContactService::SetAddress( const TDesC& aAddress )
    {
    if ( iWait.IsStarted( ) )
        {
        // if searching, changes are not allowed, which may affect call handling
        // iPrimaryAddress may be used after search completes
        return;
        }

    iPrimaryAddress.Set( aAddress );
    delete iMatchAddress;
    iMatchAddress = NULL;
    delete iMatchName;
    iMatchName = NULL;
    }

// ---------------------------------------------------------
// CMuiuContactService::SetName
// ---------------------------------------------------------
EXPORT_C void CMuiuContactService::SetName( const TDesC& aName )
    {
    if ( iWait.IsStarted( ) )
        {
        // if searching, changes are not allowed, which may affect call handling
        // iPrimaryName may be used after search completes
        return;
        }
    iPrimaryName.Set( aName );
    delete iMatchAddress;
    iMatchAddress = NULL;
    delete iMatchName;
    iMatchName = NULL;
    }

// ---------------------------------------------------------
// CMuiuContactService::CallL
// ---------------------------------------------------------
EXPORT_C TBool CMuiuContactService::CallL( )
    {
    return CallL( KNullDesC );
    }

// ---------------------------------------------------------
// CMuiuContactService::CallL
// ---------------------------------------------------------
EXPORT_C TBool CMuiuContactService::CallL( const TDesC& aSecondaryAddress )
    {
    if ( iWait.IsStarted( ) )
        {
        // if searching, changes are not allowed, which may affect call handling
        User::Leave( KErrNotSupported );
        }

    TBool ret = EFalse;
    // Primary address data
    TPtrC primaryAddress( KNullDesC );
    TPtrC primaryName( KNullDesC );
    TMuiuAddressType primaryAddressType = MsvUiServiceUtilitiesInternal::ResolveAddressTypeL( 
              iPrimaryAddress ); 
    
    // resolve type of secondary (find item) address
    iSecondaryAddress.Set( aSecondaryAddress );
    TMuiuAddressType secondaryAddressType = MsvUiServiceUtilitiesInternal::ResolveAddressTypeL( 
        iSecondaryAddress ); 

    // Get preferred call type    
    CMuiuContactService::TMuiuPreferredCallType preferredCallType = PreferredCallTypeL( );
    if ( preferredCallType == EMuiuCallTypeVoice ) 
        {
        TMuiuAddressType foundAddressType = MsvUiServiceUtilitiesInternal::ResolveAddressTypeL( 
              FoundContactAddressL() ); 

        if (    primaryAddressType == EMuiuAddressTypeEmail
            &&  foundAddressType !=  EMuiuAddressTypePhoneNumber )
            {
            // Search to phonebook must be done, if sender's address is email and
            // found address is not phone number
            ret = ETrue;
            if ( !iContactAddressMatcher )
                {
                iContactAddressMatcher = CMuiuContactAddressMatcher::NewL( iEikEnv ); 
                }
            iContactAddressMatcher->FindContactL(   iPrimaryAddress,
                                                    TCallBack( MatchDone, this) );
            iWait.Start( );

            // get results
            delete iMatchAddress;
            iMatchAddress = NULL;
            delete iMatchName;
            iMatchName = NULL;

            iMatchAddress = iContactAddressMatcher->GetAddressL();    
            iMatchName = iContactAddressMatcher->GetNameL(); 
            if (    iMatchAddress
                &&  iMatchAddress->Length() )
                {
                // Use found address 
                primaryAddress.Set( FoundContactAddressL() );
                primaryAddressType = 
                    MsvUiServiceUtilitiesInternal::ResolveAddressTypeL( primaryAddress );
                
                // Use 'remote' alias, if it is visible on the UI
                if ( iPrimaryName.Length() == 0 )
                    {
                    primaryName.Set( FoundContactNameL() );
                    }
                }
            else 
                {
                // If found contact had not address, use primary address whatever it was. 
                // Ignore also found name
                primaryAddress.Set( iPrimaryAddress );
                primaryName.Set( iPrimaryName );
                }
                
            }
        else if ( foundAddressType == EMuiuAddressTypePhoneNumber )
            {                        
            // Use search results
            primaryAddress.Set( FoundContactAddressL() );
            primaryAddressType = 
                MsvUiServiceUtilitiesInternal::ResolveAddressTypeL( primaryAddress );
                            
            // Use 'remote' alias, if it is visible on the UI
            if ( iPrimaryName.Length() == 0 )
                {
                primaryName.Set( FoundContactNameL() );
                }
            }
        else if ( primaryAddressType == EMuiuAddressTypePhoneNumber )
            {
            //  Use original address  
            primaryAddress.Set( iPrimaryAddress );
            primaryName.Set( iPrimaryName );              
            }
        else
            {
            primaryAddressType = EMuiuAddressTypeNone;
            }
        }
    else if ( preferredCallType != EMuiuCallTypeVoip )
        {
        return EFalse;
        }
    else // preferredCallType == EMuiuCallTypeVoip. Use original address information
        {
        primaryAddress.Set( iPrimaryAddress );
        primaryName.Set( iPrimaryName );
        }

    MsvUiServiceUtilitiesInternal::InternetOrVoiceCallServiceL( 
        iEikEnv,
        *iVoipExtension,
        primaryAddress,
        primaryName,
        primaryAddressType,
        iSecondaryAddress,
        secondaryAddressType,
        EFalse );
    return ret;
    }

// ---------------------------------------------------------
// CMuiuContactService::CallL
// ---------------------------------------------------------
EXPORT_C TBool CMuiuContactService::CallL( CItemFinder* aSecondaryItem )
    {
    TPtrC focusedAddr = KNullDesC( );
    if ( aSecondaryItem )
        {
        CItemFinder::CFindItemExt& item =
            aSecondaryItem->CurrentItemExt();
        if (    item.iItemDescriptor
            &&  (   item.iItemType == CItemFinder::EPhoneNumber
                ||  item.iItemType == CItemFinder::EEmailAddress ) )
            {
            focusedAddr.Set( *(item.iItemDescriptor) );
            }
        }
    return CallL( focusedAddr );
    }

// ---------------------------------------------------------
// CMuiuContactService::IsContactFound
// ---------------------------------------------------------
EXPORT_C TBool CMuiuContactService::IsContactFound() const
    {
    if ( iMatchAddress )
        {
        return iMatchAddress->Length();
        }
    return EFalse;
    }
    
// ---------------------------------------------------------
// CMuiuContactService::FoundContactAddressL
// ---------------------------------------------------------
EXPORT_C TPtrC CMuiuContactService::FoundContactAddressL() const
    {
    if ( iMatchAddress )
        {
        return *iMatchAddress;
        }
    return KNullDesC();
    }
    
// ---------------------------------------------------------
// CMuiuContactService::FoundContactNameL
// ---------------------------------------------------------
EXPORT_C TPtrC CMuiuContactService::FoundContactNameL() const
    {
    if ( iMatchName )
        {
        return *iMatchName;
        }
    return KNullDesC();
    }
    
// ---------------------------------------------------------
// CMuiuContactService::IsVoipSupported
// ---------------------------------------------------------
EXPORT_C TBool CMuiuContactService::IsVoipSupported() const
    {        
    if (    iVoipExtension
        &&  iVoipExtension->IsVoIPSupported( ) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
    
// ---------------------------------------------------------
// CMuiuContactService::MatchDone
// ---------------------------------------------------------
TInt CMuiuContactService::MatchDone( TAny* aCallService )
    {
    CMuiuContactService* caller =
        static_cast<CMuiuContactService*>( aCallService );
    caller->iWait.AsyncStop( );
    return KErrNone;
    }

                                  
// End of File

