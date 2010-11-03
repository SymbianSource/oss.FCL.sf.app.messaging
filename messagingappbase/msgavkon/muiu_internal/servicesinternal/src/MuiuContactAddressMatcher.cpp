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
* Description:   Searches contact using email address as search filter from address books.
*
*/


#include "MuiuContactAddressMatcher.h"
#include <data_caging_path_literals.hrh>    // KDC_RESOURCE_FILES_DIR
#include <muiu_internal.rsg>                // R_MUIU_PHONENUMBER_MATCH_INTEREST
#include <eikenv.h>
#include <coemain.h>
#include <basched.h>                        // KLeaveExit
#include <bautils.h>                        // BaflUtils
#include <contactmatcher.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkStoreContact.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkContactViewFilterBuilder.h>
#include <MVPbkContactFieldTextData.h>
#include <CVPbkFieldTypeRefsList.h>
#include <TVPbkFieldVersitProperty.h>
#include <AiwServiceHandler.h>
#include <finditemmenu.h>
#include <e32base.h>
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>        // CAknInformationNote
#include <StringLoader.h>

#include "MuiuMsvUiServiceUtilities.h"



// Constants 
// Resource file
_LIT( KMuiuInternalResourceFileName, "muiu_internal.rsc" );


// ---------------------------------------------------------------------------
// CMuiuContactAddressMatcher::NewL
// ---------------------------------------------------------------------------
//
CMuiuContactAddressMatcher* CMuiuContactAddressMatcher::NewL(
    CEikonEnv& aEikonEnv )
    {
    CMuiuContactAddressMatcher* self =
        new( ELeave ) CMuiuContactAddressMatcher( aEikonEnv );
    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// CMuiuContactAddressMatcher::CMuiuContactAddressMatcher
// ---------------------------------------------------------------------------
//
CMuiuContactAddressMatcher::CMuiuContactAddressMatcher( 
        CEikonEnv& aEikonEnv ):
    CActive( EPriorityStandard ),
    iEikEnv( aEikonEnv ),
    iResourceOffset( KErrNotFound )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CMuiuContactAddressMatcher::ConstructL
// ---------------------------------------------------------------------------
//
inline void CMuiuContactAddressMatcher::ConstructL()
    {
	iServiceHandler = CAiwServiceHandler::NewL();    

    iContactMatcher = CContactMatcher::NewL( &iEikEnv.FsSession() );
        
    //Changed to open only default stores.
    //Opening of all stores caused
    //frequent jamming.
    iContactMatcher->OpenDefaultMatchStoresL();
    
    iContactLinks = CVPbkContactLinkArray::NewL();
    iChangedLinks = CVPbkContactLinkArray::NewL();
    
    //Idle object is needed because we can't
    //call callback method straight from HandleNotifyL().
    //It causes KErrInUse leave.
    iIdle = CIdle::NewL(CActive::EPriorityIdle);
    
    if ( !iEikEnv.IsResourceAvailableL( R_MUIU_PHONENUMBER_MATCH_INTEREST ) )
        {
        TFileName fileName;
        TParse parse;
        parse.Set( KMuiuInternalResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
        fileName = parse.FullName();
        BaflUtils::NearestLanguageFile( iEikEnv.FsSession(), fileName );
        iResourceOffset = iEikEnv.AddResourceFileL( fileName );
        }
    
    iServiceHandler->AttachL( R_MUIU_PHONENUMBER_MATCH_INTEREST );    
    }


// ---------------------------------------------------------------------------
// CMuiuContactAddressMatcher::~CMuiuContactAddressMatcher
// ---------------------------------------------------------------------------
//
CMuiuContactAddressMatcher::~CMuiuContactAddressMatcher()
    {
    Cancel();
    // delete link arrays first to avoid access violations:
    delete iContactLinks;   
    delete iChangedLinks;    
    // doesn't matter really if detaching fails
    if( iServiceHandler )
        {
        TRAP_IGNORE( iServiceHandler->DetachL( R_MUIU_PHONENUMBER_MATCH_INTEREST ) ); 
        } 
    delete iContactMatcher;
    delete iIdle;   
    delete iServiceHandler;
    if ( iResourceOffset != KErrNotFound )
        {
        iEikEnv.DeleteResourceFile( iResourceOffset );
        }
    } 
    
// ----------------------------------------------------------------------------
// CMuiuContactAddressMatcher::FindContactL
// ----------------------------------------------------------------------------
//
TBool CMuiuContactAddressMatcher::FindContactL(
    const TDesC& aMailAddress,
    TCallBack aCallBack )
    {
    if ( !MsvUiServiceUtilities::IsValidEmailAddressL( aMailAddress ) )
        {
        return  EFalse;
        }

    if ( !IsActive() )         
        {        
        iCallBack = aCallBack;
        // reset old data
        iContactLinks->ResetAndDestroy();
        iChangedLinks->ResetAndDestroy();        
        
        const MVPbkFieldTypeList& fieldTypes = iContactMatcher->FieldTypes();         
        iContactMatcher->MatchDataL( aMailAddress, 
                                     fieldTypes, 
                                     *iContactLinks,
                                     iStatus );  
        SetActive();
        
        // dialog sets iWaitDialog to NULL when it gets deleted
        if (!iWaitDialog )
            {
            iWaitDialog = new( ELeave ) CAknWaitDialog(
                ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ) ), ETrue );
            }
        iWaitDialog->ExecuteLD( R_MUIU_MATCHING_ADDRESS_WAIT_NOTE );
        }
    else
        {
        User::Leave( KErrInUse );
        }
    return ETrue;
    }    

    
//----------------------------------------------------------------------------
// CMuiuContactAddressMatcher::GetAddressL()
// Returns NULL pointer if no data is found
//----------------------------------------------------------------------------
HBufC* CMuiuContactAddressMatcher::GetAddressL() const
    {
    ASSERT( !IsActive() );
    
    HBufC* text( NULL );
    if ( iChangedLinks->Count() > 0 )
        {
        ASSERT( iChangedLinks->Count() == 1 ); // single item fetch -> 1
        MVPbkStoreContact* storeContact = NULL;
        MVPbkStoreContactField* selectedField = NULL;        
        const MVPbkContactLink& link = iChangedLinks->At( 0 );
        iContactMatcher->GetStoreContactL( link, &storeContact ); 
        storeContact->PushL();   
        MVPbkStoreContactFieldCollection& fieldCollection =
            storeContact->Fields();
        selectedField = fieldCollection.RetrieveField( link ); 
        if ( selectedField )
            {
            // Set field data
            const MVPbkContactFieldTextData* textData =
                &MVPbkContactFieldTextData::Cast( selectedField->FieldData() );
            text = textData->Text().AllocL();
            }
        CleanupStack::PopAndDestroy( storeContact );
        }        
    iChangedLinks->ResetAndDestroy();        
    return text;
    }
//----------------------------------------------------------------------------
// CMuiuContactAddressMatcher::GetNameL()
// Returns NULL pointer if no data is found
//----------------------------------------------------------------------------
HBufC* CMuiuContactAddressMatcher::GetNameL() const
    {
    MVPbkStoreContact* storeContact = NULL;
    HBufC* name( NULL );
    
    ASSERT(iContactMatcher); // should exist
    
    if(iContactLinks->Count() > 0)
        {
        //We should have only one contact link.
        iContactMatcher->GetStoreContactL( iContactLinks->At(0), &storeContact ); 
        storeContact->PushL();   
            
        MVPbkStoreContactFieldCollection& fieldCollection =
            storeContact->Fields();
       
        name = iContactMatcher->GetNameL(fieldCollection);
        
        CleanupStack::PopAndDestroy( storeContact );
        }
    iContactLinks->ResetAndDestroy();    
    return name;
    }       
    
// ----------------------------------------------------------------------------
// CMuiuContactAddressMatcher::DoCancel()
// From CActive
// ----------------------------------------------------------------------------
//
void CMuiuContactAddressMatcher::DoCancel()
    {
    if ( iContactMatcher )
        {
        iContactMatcher->CancelOperation();
        }
    }
    
// ----------------------------------------------------------------------------
// CMuiuContactAddressMatcher::RunL()
// From CActive
// ----------------------------------------------------------------------------
//
void CMuiuContactAddressMatcher::RunL()
    {        
    // We need to delete excess matches, otherwise Fetch AIW may
    // continue with next match when user presses Cancel.
    DeleteExcessMatchesL( *iContactLinks );
    if( iContactLinks->Count() == 0)
        {
        HBufC* string = StringLoader::LoadLC( R_MUIU_QTN_PHOB_NOTE_NO_NUMBER, &iEikEnv );
        CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
        note->ExecuteLD( *string );
        CleanupStack::PopAndDestroy( string ); 
        // Deleting wait dialog in the beginning of the function causes
        // info note to flicker
        if ( iWaitDialog )
            {
            iWaitDialog->ProcessFinishedL(); // deletes the dialog
            }
        
        //We can't call iCallBack straight away,
        //because it triggers events that cause KErrInUse leave.
        iIdle->Start(iCallBack);
        }
    else
        {
        if ( iWaitDialog )
            {
            iWaitDialog->ProcessFinishedL(); // deletes the dialog
            }
        CVPbkFieldTypeSelector* filter = CreateFilterLC( );
        TAiwSingleItemSelectionDataV3 selectionData = SelectionData( *filter);        
        ExecuteSingleItemFetchL( selectionData, *iContactLinks );    
        CleanupStack::PopAndDestroy( filter );
        }
    }
	
// ----------------------------------------------------------------------------
// CMuiuContactAddressMatcher::RunError
// From CActive
// ----------------------------------------------------------------------------
//
TInt CMuiuContactAddressMatcher::RunError( TInt aError )
    {      
    if( aError == KLeaveExit )
        {
        return aError;    
        }
    else
        {
        return KErrNone;
        }
    }

// ----------------------------------------------------------------------------
// CMuiuContactAddressMatcher::HandleNotifyL
// From MAiwNotifyCallback
// ----------------------------------------------------------------------------
//
TInt CMuiuContactAddressMatcher::HandleNotifyL(
    TInt /*aCmdId*/,
    TInt aEventId,
    CAiwGenericParamList& aEventParamList,
    const CAiwGenericParamList& /*aInParamList*/ )
    {
    TInt retVal( KErrNone );
    if ( aEventId == KAiwEventCompleted )
        {
        TInt index( 0 );
        const TAiwGenericParam* param =
            aEventParamList.FindFirst( index, EGenericParamContactLinkArray );
        if ( param )
            {
            TPtrC8 contactLinks = param->Value().AsData();
            if ( contactLinks.Length() > 0 )
                {
                CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC(
                    contactLinks, iContactMatcher->GetContactStoresL() );
                CleanupStack::Pop();//links
                iChangedLinks->ResetAndDestroy();
                delete iChangedLinks;
                iChangedLinks = links;
                }
            else
                {
                retVal = KErrArgument;    
                }
            } 
        }
    //Notify waiter always, even if error.
    //We can't call iCallBack straight away,
    //because it triggers events that cause KErrInUse leave.
    iIdle->Start(iCallBack);
    return retVal;
    }

//----------------------------------------------------------------------------
//  CMuiuContactAddressMatcher::DeleteExcessMatches()
//----------------------------------------------------------------------------
void CMuiuContactAddressMatcher::DeleteExcessMatchesL(
    CVPbkContactLinkArray& aLinks ) const
    {
    // It may be that first contact has not phone number but others has.
    TInt count = aLinks.Count();
    // Search for first contact that has phone number
    for (TInt i=0 ; i < count ; i++ )
        {
        MVPbkStoreContact* storeContact = NULL;
        const MVPbkContactLink& link = aLinks.At( i );
        iContactMatcher->GetStoreContactL( link, &storeContact ); 
        storeContact->PushL();   
        if (    storeContact
            &&  iContactMatcher->ContactHasFieldOfTypeL( EAiwPhoneNumberSelect, *storeContact ) == KErrNotFound )
            {
            aLinks.Delete( i-- );
            count = aLinks.Count();
            }
        else if ( !storeContact )
            {
            // is this possible?
            aLinks.Delete( i-- );
            count = aLinks.Count();
            }
        else
            {
            CleanupStack::PopAndDestroy( storeContact );
            break;
            }
        CleanupStack::PopAndDestroy( storeContact );
        }
     
    TInt k( aLinks.Count() - 1 );
    // delete all but link at index 0
    while ( k > 0 )
        {
        aLinks.Delete( k-- );
        }
    }

//----------------------------------------------------------------------------
//  CMuiuContactAddressMatcher::CreateFilterLC()
//----------------------------------------------------------------------------
CVPbkFieldTypeSelector* CMuiuContactAddressMatcher::CreateFilterLC( ) const
    {
    CVPbkContactManager& contactMgr = iContactMatcher->GetContactManager();
    CVPbkFieldTypeSelector* contactViewFilter =
        CVPbkFieldTypeSelector::NewL( contactMgr.FieldTypes() );
    CleanupStack::PushL( contactViewFilter );

    // Append the filter object with suitable criteria
    VPbkContactViewFilterBuilder::BuildContactViewFilterL(
        *contactViewFilter, EVPbkContactViewFilterPhoneNumber, contactMgr );
    
    //VPbkContactViewFilterBuilder::BuildContactViewFilterL(
    //    *contactViewFilter, EVPbkContactViewFilterEmail, contactMgr );
        
    return contactViewFilter;
    }

//----------------------------------------------------------------------------
//  CMuiuContactAddressMatcher::SelectionData()
//----------------------------------------------------------------------------
TAiwSingleItemSelectionDataV3
CMuiuContactAddressMatcher::SelectionData(
    CVPbkFieldTypeSelector& aFilter ) const
    {
    TAiwAddressSelectType addressSelectType( EAiwAllItemsSelect );
    addressSelectType = EAiwPhoneNumberSelect;

    TAiwSingleItemSelectionDataV3 selData;
    selData.SetAddressSelectType( addressSelectType )
           .SetFetchFilter( &aFilter );
    return selData;
    }

//----------------------------------------------------------------------------
//  CMuiuContactAddressMatcher::ExecuteSingleItemFetchL()
//----------------------------------------------------------------------------
void CMuiuContactAddressMatcher::ExecuteSingleItemFetchL( 
    TAiwSingleItemSelectionDataV3 aData,
    const CVPbkContactLinkArray& aLinks )
    {
    CAiwGenericParamList& inParamList = iServiceHandler->InParamListL();    
    inParamList.AppendL(
        TAiwGenericParam( EGenericParamContactSelectionData,
        TAiwVariant( TAiwSingleItemSelectionDataV3Pckg( aData ) ) ) );

    //Contacts that matched search
    HBufC8* packedLinks = aLinks.PackLC();
        
    inParamList.AppendL( TAiwGenericParam( EGenericParamContactLinkArray,
                                           TAiwVariant( *packedLinks ) ) );

    iServiceHandler->ExecuteServiceCmdL( KAiwCmdSelect, 
                                        inParamList, 
                                        iServiceHandler->OutParamListL(),
                                        0,
                                        this );
    CleanupStack::PopAndDestroy( packedLinks ); 
    }

// End of File

