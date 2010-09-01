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
* Description:  Mail viewer contact finder.
*
*/

#include "cmsgmailviewercontactmatcher.h"
#include "MailLog.h"
#include <MsgMailViewer.rsg>
#include <coemain.h>
#include <basched.h> // KLeaveExit
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

// Constants 
    
// ---------------------------------------------------------------------------
// CMsgMailViewerContactMatcher
// ---------------------------------------------------------------------------
//
CMsgMailViewerContactMatcher::CMsgMailViewerContactMatcher(
    CAiwServiceHandler& aServiceHandler )
    : CActive( EPriorityStandard ),
    iServiceHandler( aServiceHandler )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
inline void CMsgMailViewerContactMatcher::ConstructL()
    {
    LOG("CMsgMailViewerContactMatcher::ConstructL()");
    iContactMatcher = CContactMatcher::NewL( 
        &CCoeEnv::Static()->FsSession() );
        
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
    
    iServiceHandler.AttachL( R_PHONENUMBER_MATCH_INTEREST );    
    LOG("CMsgMailViewerContactMatcher::ConstructL() -> END");
    }

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CMsgMailViewerContactMatcher* CMsgMailViewerContactMatcher::NewL(
    CAiwServiceHandler& aServiceHandler )
    {
    CMsgMailViewerContactMatcher* self =
        new( ELeave ) CMsgMailViewerContactMatcher( aServiceHandler );
    CleanupStack::PushL( self );
    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ~CMsgMailViewerContactMatcher
// ---------------------------------------------------------------------------
//
CMsgMailViewerContactMatcher::~CMsgMailViewerContactMatcher()
    {
    Cancel();
    // delete link arrays first to avoid access violations:
    delete iContactLinks;   
    delete iChangedLinks;    
    // doesn't matter really if detaching fails
    TRAP_IGNORE( iServiceHandler.DetachL( R_PHONENUMBER_MATCH_INTEREST ) );  
    delete iContactMatcher;
    delete iIdle;   
    } 
    
// ----------------------------------------------------------------------------
// FindContactL
// ----------------------------------------------------------------------------
//
void CMsgMailViewerContactMatcher::FindContactL(
    const TDesC& aMailAddress,
    TPurpose aPurpose,
    TCallBack aCallBack )
    {
    LOG1("CMsgMailViewerContactMatcher::FindContactL: %S", &aMailAddress);    
    if ( !IsActive() )         
        {        
        iPurpose = aPurpose;
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
        
        ASSERT( !iWaitDialog ); // should not exist!
        // dialog sets iWaitDialog to NULL when it gets deleted
        iWaitDialog = new( ELeave ) CAknWaitDialog(
            ( REINTERPRET_CAST( CEikDialog**, &iWaitDialog ) ));
		// safe to call LD even as a member variable, since sets itself to NULL when deleting
        iWaitDialog->ExecuteLD( R_VIEWER_WAIT_MATCHING_NOTE ); // CSI: 50 # see comment above
        }
    }    
    
    
//----------------------------------------------------------------------------
// GetDataL()
// Returns NULL pointer if no data is found
//----------------------------------------------------------------------------
HBufC* CMsgMailViewerContactMatcher::GetDataL() const
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
    return text;
    }
//----------------------------------------------------------------------------
// GetNameL()
// Returns NULL pointer if no data is found
//----------------------------------------------------------------------------
HBufC* CMsgMailViewerContactMatcher::GetNameL() const
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
    
    
    return name;
    }       
    
// ----------------------------------------------------------------------------
// From CActive
// ----------------------------------------------------------------------------
//
void CMsgMailViewerContactMatcher::DoCancel()
    {
    if ( iContactMatcher )
        {
        iContactMatcher->CancelOperation();
        }
    }
    
// ----------------------------------------------------------------------------
// From CActive
// ----------------------------------------------------------------------------
//
void CMsgMailViewerContactMatcher::RunL()
    {
    LOG1("CMsgMailViewerContactMatcher::RunL: %d", iStatus.Int());
    
    ASSERT( iWaitDialog ); // should exist!
    iWaitDialog->ProcessFinishedL(); // deletes the dialog
    
    if(iContactLinks->Count() == 0)
        {
        //No matching contacts found -> no need to do fetch.
        
        //We can't call iCallBack straight away,
        //because it triggers events that cause KErrInUse leave.
        iIdle->Start(iCallBack); // CSI: 10 # 
        }
    else
        {
        // We need to delete excess matches, otherwise Fetch AIW may
        // continue with next match when user presses Cancel.
        DeleteExcessMatches( *iContactLinks );
        CVPbkFieldTypeSelector* filter = CreateFilterLC( iPurpose );
        TAiwSingleItemSelectionDataV3 selectionData = SelectionData( iPurpose,
                                                                     *filter);        
        ExecuteSingleItemFetchL( selectionData, *iContactLinks );    
        CleanupStack::PopAndDestroy( filter );
        }
    }
	
// ----------------------------------------------------------------------------
// From CActive
// ----------------------------------------------------------------------------
//
TInt CMsgMailViewerContactMatcher::RunError( TInt aError )
    {
    LOG1("CMsgMailViewerContactMatcher::RunError: %d", aError);
      
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
// From MAiwNotifyCallback
// ----------------------------------------------------------------------------
//
TInt CMsgMailViewerContactMatcher::HandleNotifyL( // CSI: 40 # We must return 
												  // the integer value although this 
												  // is a leaving method.
    TInt LOGPARAM_ONLY( aCmdId ),
    TInt aEventId,
    CAiwGenericParamList& aEventParamList,
    const CAiwGenericParamList& /*aInParamList*/ )
    {
    LOG1("CMsgMailViewerContactMatcher::HandleNotifyL: %d", aCmdId);
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
                LOG1("CMsgMailViewerContactMatcher::HandleNotifyL: LA %d", 
                     iChangedLinks->Count());
                
                //We can't call iCallBack straight away,
                //because it triggers events that cause KErrInUse leave.
                iIdle->Start(iCallBack); // CSI: 10 # 
                }
            else
                {
                retVal = KErrArgument;    
                }
            } 
        }
    return retVal;
    }

//----------------------------------------------------------------------------
//  DeleteExcessMatches()
//----------------------------------------------------------------------------
void CMsgMailViewerContactMatcher::DeleteExcessMatches(
    CVPbkContactLinkArray& aLinks ) const
    {
    TInt i( aLinks.Count() - 1 );
    // delete all but link at index 0
    while ( i > 0 )
        {
        aLinks.Delete( i-- );
        }
    }

//----------------------------------------------------------------------------
//  CreateFilterLC()
//----------------------------------------------------------------------------
CVPbkFieldTypeSelector* CMsgMailViewerContactMatcher::CreateFilterLC(
    TPurpose aPurpose ) const
    {
    CVPbkContactManager& contactMgr = iContactMatcher->GetContactManager();
    CVPbkFieldTypeSelector* contactViewFilter =
        CVPbkFieldTypeSelector::NewL( contactMgr.FieldTypes() );
    CleanupStack::PushL( contactViewFilter );

    ASSERT( aPurpose != EPurposeNone );

    if ( aPurpose == ECallToContact ||  aPurpose == EMsgToContact )
        {        
        // Append the filter object with suitable criteria
        VPbkContactViewFilterBuilder::BuildContactViewFilterL(
            *contactViewFilter, EVPbkContactViewFilterPhoneNumber, contactMgr );
        }
        
    // if message, then add also email addresses in addition to phone numbers
    if ( aPurpose == EMsgToContact )
        {        
        VPbkContactViewFilterBuilder::BuildContactViewFilterL(
            *contactViewFilter, EVPbkContactViewFilterEmail, contactMgr );
        }
        
    return contactViewFilter;
    }

//----------------------------------------------------------------------------
//  SelectionData()
//----------------------------------------------------------------------------
TAiwSingleItemSelectionDataV3
CMsgMailViewerContactMatcher::SelectionData(
    TPurpose aPurpose,
    CVPbkFieldTypeSelector& aFilter ) const
    {
    TAiwAddressSelectType addressSelectType( EAiwAllItemsSelect );
    if ( aPurpose == ECallToContact )
        {
        addressSelectType = EAiwPhoneNumberSelect;
        }
    else 
        {        
        ASSERT( aPurpose == EMsgToContact );
        addressSelectType = EAiwMMSSelect;
        }
    TAiwSingleItemSelectionDataV3 selData;
    selData.SetAddressSelectType( addressSelectType )
           .SetFetchFilter( &aFilter );
    return selData;
    }

//----------------------------------------------------------------------------
//  ExecuteSingleItemFetchL()
//----------------------------------------------------------------------------
void CMsgMailViewerContactMatcher::ExecuteSingleItemFetchL( 
    TAiwSingleItemSelectionDataV3 aData,
    const CVPbkContactLinkArray& aLinks )
    {
    CAiwGenericParamList& inParamList = iServiceHandler.InParamListL();    
    inParamList.AppendL(
        TAiwGenericParam( EGenericParamContactSelectionData,
        TAiwVariant( TAiwSingleItemSelectionDataV3Pckg( aData ) ) ) );

    //Contacts that matched search
    HBufC8* packedLinks = aLinks.PackLC();
        
    inParamList.AppendL( TAiwGenericParam( EGenericParamContactLinkArray,
                                           TAiwVariant( *packedLinks ) ) );

    iServiceHandler.ExecuteServiceCmdL( KAiwCmdSelect, 
                                        inParamList, 
                                        iServiceHandler.OutParamListL(),
                                        0,
                                        this );
    CleanupStack::PopAndDestroy( packedLinks ); 
    }

// End of File

