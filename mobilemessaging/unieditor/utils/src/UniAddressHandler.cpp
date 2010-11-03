/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Defines implementation of CUniAddressHandler class methods.
*
*/



// ========== INCLUDE FILES ================================

#include "UniAddressHandler.h"

// MSV
#include <cmsvrecipientlist.h>  
#include <mtclbase.h>

// MMS Engine
#include <mmsgenutils.h>

// Base Editor
#include <MsgAddressControl.h>      // for CMsgAddressControl
#include <MsgRecipientItem.h>       // for CMsgRecipientItem
#include <MsgCheckNames.h>          // for CMsgCheckNames
#include <contactmatcher.h>
#include <MVPbkStoreContact.h>

// MUIU
#include <MuiuMsvUiServiceUtilities.h>
#include <muiumsvuiserviceutilitiesinternal.h>

#include <centralrepository.h>    // link against centralrepository.lib
#include <telconfigcrkeys.h> // for Central Repository keys

// AVKON
#include <AknUtils.h>
#include <aknnotewrappers.h>
#include <akninputblock.h>

// Common Components
#include <StringLoader.h>           // for StringLoader (load and foramt strings from resources)
#include <commonphoneparser.h>      // Common phone number validity checker
#include <data_caging_path_literals.hrh> 

#include <bautils.h>                        // NearestLanguageFile

#include <UniUtils.rsg>           // resources

#include "UniUtils.h"

// ========== EXTERNAL DATA STRUCTURES =====================

// ========== EXTERNAL FUNCTION PROTOTYPES =================

// ========== CONSTANTS ====================================

const TInt KMaxDetailsLength = 64;   // Copy max this many chars to TMsvEntry iDetails
const TInt KDefaultCompareLength = 7;

_LIT( KAddressSeparator, ";" );

// ========== MACROS =======================================

// ========== LOCAL CONSTANTS AND MACROS ===================

// ========== MODULE DATA STRUCTURES =======================

// ========== LOCAL FUNCTION PROTOTYPES ====================

// ========== LOCAL FUNCTIONS ==============================

// ========== MEMBER FUNCTIONS =============================

// ---------------------------------------------------------
// CUniAddressHandler::CUniAddressHandler
//
// C++ constructor.
// ---------------------------------------------------------
//
CUniAddressHandler::CUniAddressHandler( CBaseMtm& aMtm,
                                        CMsgAddressControl& aControl,
                                        CCoeEnv& aCoeEnv ) :
    iMtm( aMtm ),
    iControl( aControl ),
    iCoeEnv( aCoeEnv ),
    iValidAddressType( CMsgCheckNames::EMsgTypeMms ),
    iResourceLoader( iCoeEnv )
    {
    }

// ---------------------------------------------------------
// CUniAddressHandler::ConstructL
// ---------------------------------------------------------
//
void CUniAddressHandler::ConstructL()
    {
    if ( !iCoeEnv.IsResourceAvailableL( R_UNUT_INFO_FAIL_RECIPIENT_NO_ALIAS ) )
        {
        TParse parse;
        User::LeaveIfError( parse.Set( KUniUtilsResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL ) );
        TFileName fileName( parse.FullName() );
        iResourceLoader.OpenL( fileName );                 
        }
        iCheckNames = CMsgCheckNames::NewL();
    }

// ---------------------------------------------------------
// CUniAddressHandler::NewL
//
// Factory method.
// ---------------------------------------------------------
//
EXPORT_C CUniAddressHandler* CUniAddressHandler::NewL( CBaseMtm& aMtm,
                                                       CMsgAddressControl& aControl,
                                                       CCoeEnv& aCoeEnv )
    {
    CUniAddressHandler* self = new ( ELeave ) CUniAddressHandler( aMtm, aControl, aCoeEnv );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CUniAddressHandler::~CUniAddressHandler
// ---------------------------------------------------------
//
CUniAddressHandler::~CUniAddressHandler()
    {
    iResourceLoader.Close();
    
    delete iCheckNames;
    delete iInvalidRecipients;
    }

// ---------------------------------------------------------
// CUniAddressHandler::RemoveAddressesFromMtmL
//
// Resets address list in MTM
// ---------------------------------------------------------
//
EXPORT_C void CUniAddressHandler::RemoveAddressesFromMtmL( TMsvRecipientTypeValues aRecipientType)
    {
    const CMsvRecipientList& addresses = iMtm.AddresseeList();

    for ( TInt i = iMtm.AddresseeList().Count(); --i >=  0; )
        {
        if ( addresses.Type( i ) == aRecipientType )
            {
            iMtm.RemoveAddressee( i );
            }
        }
    }

// ---------------------------------------------------------
// CUniAddressHandler::CopyAddressesToMtmL
//
// Resets address list in MTM and insert addresses from 
// address control to MTM
// ---------------------------------------------------------
//
EXPORT_C void CUniAddressHandler::CopyAddressesToMtmL( TMsvRecipientTypeValues aRecipientType )
    {
    RemoveAddressesFromMtmL( aRecipientType );
    AppendAddressesToMtmL( aRecipientType );
    }

// ---------------------------------------------------------
// CUniAddressHandler::AppendAddressesToMtmL
//
// Inserts addresses from control to MTM.
// ---------------------------------------------------------
//
EXPORT_C void CUniAddressHandler::AppendAddressesToMtmL( TMsvRecipientTypeValues aRecipientType)

    {
    const TInt KMaxContactLength = 100;
    const CMsgRecipientArray* recipients = iControl.GetRecipientsL(); // Not owned
    
    HBufC* contactname = HBufC::NewL( KMaxContactLength );
    CleanupStack::PushL( contactname ); 
    TPtr aliPtr ( contactname->Des() );
    TInt addrCnt = recipients->Count();
    
    for ( TInt i = 0; i < addrCnt ; i++ )
        {
        CMsgRecipientItem* addrItem = recipients->At( i );

        TPtr realAddress = addrItem->Address()->Des();
        TPtr alias = addrItem->Name()->Des();

        // Check that neither string contains illegal characters.
        // If they does strip illegal chars away and save
        RemoveIllegalChars( realAddress );
        
        if ( alias.Length() > 0 )
            {
            RemoveIllegalChars( alias );
            iMtm.AddAddresseeL( aRecipientType, realAddress, alias );
            }
        else
            {               
            iCheckNames->GetAliasL(realAddress,aliPtr,KMaxContactLength);             
            if(contactname->Des().Length() > 0 )
                {
                RemoveIllegalChars( aliPtr );
                }            
            iMtm.AddAddresseeL( aRecipientType, realAddress, aliPtr );
            }                              
        }
        CleanupStack::PopAndDestroy( contactname );           
    }

// ---------------------------------------------------------
// CUniAddressHandler::CopyAddressesFromMtmL
//
// Inserts addresses from MTM to address control. aAddInvalid
// parameter is used to determine whether invalid addresses
// should be added to the control or to special list that can
// be used later on to show invalid address notes.
// ---------------------------------------------------------
//
EXPORT_C void CUniAddressHandler::CopyAddressesFromMtmL( TMsvRecipientTypeValues aRecipientType,
                                                         TBool aAddInvalid )
    {
    const CMsvRecipientList& addresses = iMtm.AddresseeList();

    CMsgRecipientList* recipients = CMsgRecipientList::NewL();
    CleanupStack::PushL( recipients );
        
    TInt addrCnt = addresses.Count();        
    for ( TInt i = 0; i < addrCnt; i++ )
        {          
        if ( addresses.Type( i ) == aRecipientType )
            {
            TPtrC address = TMmsGenUtils::PureAddress( addresses[i] );
            if ( address.Length() > 0 )
                {
                TPtrC alias   = TMmsGenUtils::Alias( addresses[i] );
                if ( address.Compare( alias ) == 0 )
                    {
                    // Both alias and address are the same -> drop the alias as this is most likely reply to 
                    // SMS and SMS client MTM sets both alias and address to be the same.
                    alias.Set( KNullDesC );
                    }
                
                CMsgRecipientItem* item = CMsgRecipientItem::NewLC( alias, address );
                item->SetVerified( alias.Length() > 0 ? ETrue : EFalse );
                
                HBufC16* convertBuffer = NumberConversionLC( address, EFalse );
                
            	if ( aAddInvalid || CheckSingleAddressL( *convertBuffer ) )
                    {    
                    recipients->AppendL( item );
                    }
                else
                    {
                    if ( !iInvalidRecipients )
                        {
                        iInvalidRecipients = CMsgRecipientList::NewL();
                        }
                    iInvalidRecipients->AppendL( item );
                    }
                
                CleanupStack::PopAndDestroy( convertBuffer );    
                CleanupStack::Pop( item );
                }
            }
        }

    if ( recipients->Count() > 0 )
        {
        iControl.AddRecipientsL( *recipients );
        iControl.SetModified( EFalse );
        }
        
    CleanupStack::PopAndDestroy( recipients );
    }

// ---------------------------------------------------------
// CUniAddressHandler::VerifyAddressesL
//
// Walks thru addresses in addresscontrol and counts errors. 
// If unverified address is valid, marks it validated (verified now). 
// Stops to first error.
// ---------------------------------------------------------
//
EXPORT_C TBool CUniAddressHandler::VerifyAddressesL( TBool& aModified )
    {
    CAknInputBlock::NewLC();  

    aModified = EFalse;

    CMsgRecipientArray* recipients = iControl.GetRecipientsL(); // Not owned

    TInt addrCnt = recipients->Count();
    
    // First verify unverified addresses in control.
    for ( TInt i = 0; i < addrCnt; i++ )
        {
        CMsgRecipientItem* addrItem = recipients->At( i );

        if ( !addrItem->IsVerified() || 
             !addrItem->IsValidated() )
            {
            // Must check
            // Just check the addresses here. Don't modify them yet.
            if ( CheckSingleAddressL( *( addrItem->Address() ) ) )
                {
                addrItem->SetValidated( ETrue );
                }
            else 
                {
                // Address not OK, but maybe it's an alias from Phonebook...
                TBool addressOK = EFalse;
                
                // Search for matches
                if ( !iCheckNames )
                    {
                    iCheckNames = CMsgCheckNames::NewL();
                    }
                    
                if ( iCheckNames->FindAndCheckByNameL( *addrItem->Address(), 
                                                       iValidAddressType, 
                                                       *recipients, 
                                                       i ) ) 
                    { 
                    //  Determine how many new entries
                    addressOK = ETrue;
                    aModified = ETrue;

                    TInt newEntries = recipients->Count() - addrCnt;
                    TInt checkedEntry = i;
                    i = i + newEntries;
                    
                    // Check that found entry's/entries number is valid
                    while ( checkedEntry <= i )
                        {
                        addrItem = recipients->At( checkedEntry );
                        if ( CheckSingleAddressL( *( addrItem->Address() ) ) )
                            {
                            addrItem->SetValidated( ETrue );
                            addrItem->SetVerified( ETrue );
                            }
                        else 
                            { 
                            // Address not OK
                            addressOK = EFalse;
                            break;
                            }
                            
                        checkedEntry++;
                        }
                    }
                    
                if ( !addressOK )
                    {
                    // Save the address & alias name. Those will be
                    // lost if slide is changed.
                    HBufC* address = addrItem->Address()->AllocLC();
                    HBufC* name = addrItem->Name()->AllocLC();
                    
                    iControl.RefreshL( *recipients ); 
                    
                    // Address is not OK.
                    ShowAddressInfoNoteL( *address, *name );
                    
                    CleanupStack::PopAndDestroy( 3 ); //address, name, CAknInputBlock
                    
                    iControl.HighlightUnvalidatedStringL(); 
                    return EFalse;
                    }
                }
            }
        }


    // Addresses are all ok. Now parse not allowed chars away before giving it to MTM.
    addrCnt = recipients->Count();
    for ( TInt ii = 0; ii < addrCnt; ii++ )
        {
        // Note: This is just to parse spaces etc away from phonenumbers.
        //       Ignore EFalse returned for email addresses. 
        TPtr tempPtr = recipients->At( ii )->Address()->Des();
        CommonPhoneParser::ParsePhoneNumber( tempPtr, 
                                             CommonPhoneParser::ESMSNumber );
        }

    iControl.RefreshL( *recipients ); // returns always KErrNone!
    
    CleanupStack::PopAndDestroy(); // CAknInputBlock
    return ETrue;
    }

// ---------------------------------------------------------
// CUniAddressHandler::RemoveDuplicateAddressesL
//
// Should be called when sending and after 
// all addresses are validated i.e. no more error corrections
// from user i.e. user is not going to edit addr control 
// after this because Addresscontrol is not updated to show 
// removed entries.
// ---------------------------------------------------------
//
EXPORT_C TBool CUniAddressHandler::RemoveDuplicateAddressesL( 
    CArrayPtrFlat<CMsgAddressControl>& aAddressControls )
    {
    TBool retVal = EFalse;
    
    TInt controlCount = aAddressControls.Count();
    if ( controlCount == 0 )
        {
        return retVal;
        }

    // Initialize internal recipient info array.
    // Needed for tracking modifications of the recipient lists.
    // Possible modifications are "committed" back to address
    // controls only once after all the checks have been performed.
    CArrayFixFlat<TRecipientsInfo>* recipientInfos = 
                                    new ( ELeave ) CArrayFixFlat<TRecipientsInfo>( controlCount );
    CleanupStack::PushL( recipientInfos );
    
    for ( TInt init = 0; init < controlCount; init++ )
        {
        TRecipientsInfo info;
        info.iRecipients = aAddressControls.At( init )->GetRecipientsL();
        info.iOriginalCount = info.iRecipients->Count();
        info.iModified = EFalse;
        recipientInfos->AppendL( info );
        }

    TInt compareLength( KDefaultCompareLength );
    
    CRepository* repository = CRepository::NewL( KCRUidTelConfiguration );
    
    repository->Get( KTelMatchDigits, compareLength );
    
    delete repository;   
   
    CMsgRecipientItem* addrItem1 = NULL;
    CMsgRecipientItem* addrItem2 = NULL;
    CMsgRecipientArray* recipients1 = NULL;
    CMsgRecipientArray* recipients2 = NULL;
    
    for ( TInt i = 0; i < controlCount; i++ )
        {
        recipients1 = recipientInfos->At( i ).iRecipients;
        
        for ( TInt j = 0; j < recipients1->Count(); j++ )
            {
            addrItem1 = recipients1->At( j );
            
            for ( TInt k = i; k < controlCount; k++ )
                {
                recipients2 = recipientInfos->At( k ).iRecipients;
                TInt l( 0 );
                if ( recipients2 == recipients1 )
                    {
                    // No need to check earlier items of "recipients1"
                    l = j + 1;
                    }
                for ( ; l < recipients2->Count(); l++ )
                    {
                    addrItem2 = recipients2->At( l );

                    if ( addrItem1->Address() && addrItem2->Address() )
                        {
                        // Check for equal phone numbers
                        TBool equal = MsvUiServiceUtilitiesInternal::ComparePhoneNumberL( *( addrItem1->Address() ), 
                                                                                          *( addrItem2->Address() ), 
                                                                                          compareLength );
                        if ( !equal )
                            {
                            // Check for equal mail addresses
                            equal = addrItem1->Address()->CompareF( *(addrItem2->Address() ) ) == 0;
                            }
                        
                        // If email address check or phone number check returns that addresses are 
                        // equal then remove either one of them.
                        if ( equal )
                            {
                            if( addrItem1->Name()->Length() && addrItem2->Name()->Length() )
                                { // Both recipients have alias
                                if ( addrItem1->Name()->Compare( *(addrItem2->Name()) ) != 0 )
                                    {
                                    // if aliases don't match clear alias.
                                    addrItem1->SetNameL( KNullDesC() );
                                    addrItem1->SetVerified( EFalse );
                                    recipientInfos->At( i ).iModified = ETrue;
                                    }
                                // if they match -> leave it as it is
                                }
                            else if( addrItem1->Name()->Length() || addrItem2->Name()->Length() )
                                { // only one of them has an alias
                                if( addrItem2->Name()->Length() )
                                    { // addrItem2 has it
                                    addrItem1->SetNameL( *addrItem2->Name( ) );
                                    addrItem1->SetVerified( ETrue );
                                    recipientInfos->At( i ).iModified = ETrue;
                                    }
                                // else addrItem1 has it so it's already set in the Name field
                                }
                            recipients2->Delete( l );
                            delete addrItem2;
                            addrItem2 = NULL;
                            l--;
                            retVal = ETrue;
                            }
                        }
                    }
                }
            }
        }

    // Check whether recipients have changed and refresh if needed.
    for ( TInt final = 0; final < controlCount; final++ )
        {
        if ( recipientInfos->At( final ).iModified ||
            ( recipientInfos->At( final ).iOriginalCount !=
              recipientInfos->At( final ).iRecipients->Count() ) )
            {
            aAddressControls.At( final )->RefreshL( *recipientInfos->At( final ).iRecipients );
            }
        }
    CleanupStack::PopAndDestroy( recipientInfos );
    return retVal;
    }

// ----------------------------------------------------
// CUniAddressHandler::AddRecipientL
//
// Performs address(es) fetch operation.
// ----------------------------------------------------
//
EXPORT_C TBool CUniAddressHandler::AddRecipientL( TBool& aInvalid )
    {
    TBool addressesAdded( EFalse );
    aInvalid = EFalse;
    
    CMsgRecipientList* recipientList = CMsgRecipientList::NewL();
    CleanupStack::PushL( recipientList );

    CMsgRecipientArray* recipients = new ( ELeave ) CMsgRecipientArray( 10 );
    CleanupStack::PushL( TCleanupItem( CleanupRecipientArray, recipients ) );
    
    // Multiple entry fetch to get the contact    
    if ( !iCheckNames )
        {
        iCheckNames = CMsgCheckNames::NewL();
        }
        
    iCheckNames->FetchRecipientsL( *recipients, iValidAddressType );
    
    // Contacts now fetched, verify that valid address is given for every contact
    while ( recipients->Count() > 0 )
        {
        CMsgRecipientItem* recipient = recipients->At( 0 );
        
        TPtrC namePtr = recipient->Name()->Des();
        TPtrC addressPtr = recipient->Address()->Des();

        // Don't parse away chars here so this is consistent with 
        // addresses that user writes "-()" are saved to draft
        // but removed when sending
        if ( CheckSingleAddressL( addressPtr ) )
            {
            //  add it to the list of valid addresses
            recipient->SetValidated( ETrue );
            recipient->SetVerified( ( namePtr.Length() > 0 ? ETrue : EFalse ) );
            recipientList->AppendL( recipient );
            }
        else
            {
            aInvalid = ETrue;
            ShowAddressInfoNoteL( addressPtr, namePtr );
            delete recipient;
            }
        
        recipients->Delete( 0 );
        }
    
    if ( recipientList->Count() > 0 )
        {
        iControl.AddRecipientsL( *recipientList );
        addressesAdded = ETrue;
        }
 
    CleanupStack::PopAndDestroy( 2, recipientList );//recipients
    
    return addressesAdded;
    }
    
// ---------------------------------------------------------
// CUniAddressHandler::CheckSingleAddressL
// ---------------------------------------------------------
//
EXPORT_C TBool CUniAddressHandler::CheckSingleAddressL( const TDesC& aAddress )
    {
    if ( iValidAddressType != CMsgCheckNames::EMsgTypeMail &&
         CommonPhoneParser::IsValidPhoneNumber( aAddress, CommonPhoneParser::ESMSNumber ) )
        {
        return ETrue;
        }
        
    if ( iValidAddressType != CMsgCheckNames::EMsgTypeSms && 
         MsvUiServiceUtilities::IsValidEmailAddressL( aAddress ) )
        {
        return ETrue;
        }
        
    return EFalse;
    }

// ---------------------------------------------------------
// CUniAddressHandler::MakeDetailsL
// ---------------------------------------------------------
//
EXPORT_C void CUniAddressHandler::MakeDetailsL( TDes& aDetails )
    {
    // This very same code can be found in MMSPlugin. 
    // They should be put in common location some day...
    const CMsvRecipientList& addresses = iMtm.AddresseeList();
    TInt addrCnt = addresses.Count();

    TPtrC stringToAdd;
    for ( TInt i = 0; i < addrCnt; i++)
        {
        // Only address is converted to western. 
        // There may numbers in contact name - they must not be converted 
        TPtrC alias = TMmsGenUtils::Alias( addresses[i] );
        HBufC* addressBuf = NULL;
        
        if ( alias.Length() != 0 )
            {
            stringToAdd.Set( alias );
            }
        else
            {
            TPtrC address = TMmsGenUtils::PureAddress( addresses[i] );
            addressBuf = HBufC::NewLC( address.Length() );
            TPtr addressPtr = addressBuf->Des();
            addressPtr.Copy( address );
            stringToAdd.Set( addressPtr );

            // Internal data structures always holds the address data in western format.
            // UI is responsible of doing language specific conversions.    
            AknTextUtils::ConvertDigitsTo( addressPtr, EDigitTypeWestern );      
            }
        
        if ( ( aDetails.Length() != 0 ) &&   // Not a first address
             ( aDetails.Length() + KAddressSeparator().Length() < KMaxDetailsLength ) )
            {
            // Add separator
            aDetails.Append( KAddressSeparator() );
            }

        if ( aDetails.Length() + stringToAdd.Length() < KMaxDetailsLength ) 
            {
            // whole string fits. Add it.
            aDetails.Append( stringToAdd );
            if ( addressBuf )
                {
                CleanupStack::PopAndDestroy( addressBuf );
                }
            }
        else
            {
            // Only part of the string fits
            TInt charsToAdd = KMaxDetailsLength - aDetails.Length();

            if ( charsToAdd <= 0 )
                {
                // Cannot add any more chars 
                break;
                }

            if ( charsToAdd >= stringToAdd.Length() )
                {
                // Guarantee that charsToAdd is not larger that stringToAdd lenght 
                charsToAdd = stringToAdd.Length();
                }

            aDetails.Append( stringToAdd.Left( charsToAdd ) );
            if ( addressBuf )
                {
                CleanupStack::PopAndDestroy( addressBuf );
                }
            break;
            }
        }
    }

// ----------------------------------------------------
// CUniAddressHandler::ShowInvalidRecipientInfoNotesL
// ----------------------------------------------------
//
EXPORT_C void CUniAddressHandler::ShowInvalidRecipientInfoNotesL()
    {
    if ( iInvalidRecipients )
        {
        for ( TInt current = 0; current < iInvalidRecipients->Count(); current++ )
            {
            ShowAddressInfoNoteL( *iInvalidRecipients->At( current )->Address(),
                                  *iInvalidRecipients->At( current )->Name() );
            }
            
        delete iInvalidRecipients;
        iInvalidRecipients = NULL;
        }
    }

// ---------------------------------------------------------
// CUniAddressHandler::ShowAddressInfoNoteL
// ---------------------------------------------------------
//
void CUniAddressHandler::ShowAddressInfoNoteL( const TDesC& aAddress, const TDesC& aAlias )
    {
    TInt cleanupCount = 0;
    HBufC* string = NULL;

    HBufC* convertedAddress = NumberConversionLC( aAddress, ETrue );
    cleanupCount++;

    if ( aAlias.Length() == 0 )
        {
        string = StringLoader::LoadLC( R_UNUT_INFO_FAIL_RECIPIENT_NO_ALIAS,
                                       *convertedAddress,
                                       &iCoeEnv );
        cleanupCount++;
        }
    else
        {
        CDesCArrayFlat* stringArr = new ( ELeave ) CDesCArrayFlat( 2 );
        CleanupStack::PushL( stringArr );
        cleanupCount++;
        
        stringArr->AppendL( aAlias );    //First string
        stringArr->AppendL( *convertedAddress );  //Second string
        string = StringLoader::LoadLC( R_UNUT_INFO_FAIL_RECIPIENT,
                                       *stringArr,
                                       &iCoeEnv );
        cleanupCount++;
        }
    
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *string );
    
    CleanupStack::PopAndDestroy( cleanupCount );  // string, (stringArr), convertedAddress
    }

// ---------------------------------------------------------
// CUniAddressHandler::RemoveIllegalChars
// ---------------------------------------------------------

TBool CUniAddressHandler::RemoveIllegalChars( TDes& aString )
    {
    TBool ret( EFalse );
    for ( TInt i = 0; i < aString.Length(); i++ )
        {
        if ( !IsValidChar( (TChar) aString[ i ] ) ) 
            {
            aString.Delete( i, 1 );
            i--;
            ret = ETrue;
            }
        }
    return ret;
    }

// ---------------------------------------------------------
// CUniAddressHandler::IsValidChar
//
// These characters are used by system on it's internal data structure
// and cannot be because of that used on address or alias string
// ---------------------------------------------------------
//
TBool CUniAddressHandler::IsValidChar( const TChar& aChar )
    {
    // Don't allow "<" (60), ">" (62)
    return ( aChar != '<' && aChar != '>' );
    }

// ----------------------------------------------------
// CUniAddressHandler::NumberConversionLC
// ----------------------------------------------------
//
HBufC* CUniAddressHandler::NumberConversionLC( const TDesC& aOrigNumber, TBool aDirection )
    {
    HBufC* addressCopy = aOrigNumber.AllocLC();
    TPtr tempTPtr = addressCopy->Des();
    if ( aDirection )
        {
        AknTextUtils::DisplayTextLanguageSpecificNumberConversion( tempTPtr );
        }
    else
        {
        AknTextUtils::ConvertDigitsTo( tempTPtr, EDigitTypeWestern );
        }

    return addressCopy;
    }

// ----------------------------------------------------
// CUniAddressHandler::NumberConversionLC
// ----------------------------------------------------
//
void CUniAddressHandler::CleanupRecipientArray( TAny* aArray )
    {
    CMsgRecipientArray* recipientArray = static_cast<CMsgRecipientArray*>( aArray );
    
    recipientArray->ResetAndDestroy();
    delete recipientArray;
    }


// ========== OTHER EXPORTED FUNCTIONS =====================

//  End of File
