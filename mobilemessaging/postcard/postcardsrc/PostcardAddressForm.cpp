/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*           Methods for Postcard Address form.
*
*/



// INCLUDE FILES

#include <eikspane.h>               // CEikStatusPane
#include <eikmenub.h>
#include <eiklabel.h>               // for CEikLabel

#include <avkon.hrh>
#include <aknnavi.h>                // CAknNavigationControlContainer
#include <aknnavide.h>              // CAknNavigationDecorator
#include <AknIndicatorContainer.h>	// for CAknIndicatorContainer
#include <aknEditStateIndicator.h>	// for MAknEditStateIndicator
#include <akntitle.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h>
#include <AknsUtils.h>
#include <aknlists.h>

#include <featmgr.h>
#include <StringLoader.h>
#include <csxhelp/postcard.hlp.hrh>

#include <cntitem.h>
#include <cntfldst.h>
#include <mmsmsventry.h>

#include <Postcard.rsg>
#include "Postcard.hrh"
#include "PostcardLaf.h"
#include "PostcardDocument.h"
#include "PostcardAppUi.h"
#include "PostcardAddressForm.h"
#include "PostcardUtils.h"
#include "PostcardRecipientWrapper.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
//  Two-phased constructor
// ---------------------------------------------------------
CPostcardAddressForm* CPostcardAddressForm::NewL( CPostcardAppUi& aAppUi, CPostcardDocument& aDocument, CContactCard& aContactItem, TInt& aRetValue, TInt aFirstActive, TKeyEvent aKeyEvent  )
    {
    CPostcardAddressForm* self = 
        new (ELeave) CPostcardAddressForm( aAppUi, aDocument, aContactItem, aRetValue, aFirstActive, aKeyEvent );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( );
    return self;
    }

// ---------------------------------------------------------
//  Default destructor
// ---------------------------------------------------------
CPostcardAddressForm::~CPostcardAddressForm()
    {
    delete iContact;

    // Remove the navidecorator from status pane
    CEikStatusPane *sp = iEikonEnv->AppUiFactory( )->StatusPane( );
    TRAP_IGNORE( static_cast<CAknNavigationControlContainer*>
            ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) )->Pop( iNaviDecorator );
    ); // TRAP_IGNORE
    delete iNaviDecorator;
    }

// ---------------------------------------------------------
// CPostcardAddressForm
// ---------------------------------------------------------
CPostcardAddressForm::CPostcardAddressForm
	    	( 	CPostcardAppUi& aAppUi, 
	    		CPostcardDocument& aDocument,
	    		CContactCard& aContactItem,
	    		TInt& aRetValue,
	    		TInt aFirstActive,
	    		TKeyEvent aKeyEvent ) :
    iContactItem( aContactItem ),
	iAppUi( aAppUi ),
    iDocument( aDocument ), 
    iNaviDecorator( NULL ),
    iKeyEvent( aKeyEvent ),
    iRetValue( aRetValue ),
    iPreviousControl( NULL ),
    iFirstActive( aFirstActive )
    {
    }


// ---------------------------------------------------------
// ExecuteLD()
// ---------------------------------------------------------
TInt CPostcardAddressForm::ExecLD( )
	{	
    PrepareLC( R_POSTCARD_ADDRESS_DIALOG );
    return CAknForm::RunLD( );
	}	

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
void CPostcardAddressForm::ConstructL( )
    {

    // Feature manager
    FeatureManager::InitializeLibL( );

    if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
        {
        iAddrFlags |= EPostcardAddressHelpSupported;
        }

    FeatureManager::UnInitializeLib( );

    CleanNavipane();
    
    // Call the base class' two-phased constructor
    CAknDialog::ConstructL( R_POSTCARD_ADDRESS_MENUBAR ); 
    }

// ---------------------------------------------------------
// OfferKeyEventL
// ---------------------------------------------------------
TKeyResponse CPostcardAddressForm::OfferKeyEventL
        (const TKeyEvent& aKeyEvent, 
        TEventCode aType)
    {
    // Only interested in standard key events
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

    // If a menu is showing offer key events to it.
    if ( CAknForm::MenuShowing( ) )
        {
        return CAknForm::OfferKeyEventL( aKeyEvent, aType );
        }

    TUint code = aKeyEvent.iCode;

    TKeyResponse resp=EKeyWasNotConsumed;

    switch ( code )
        {
        case EKeyOK:
            {
            DoSelectionKeyL( );
            resp=EKeyWasConsumed;
            break;
            }

        case EKeyBackspace:
        case EKeyDelete:
            {
            resp = CAknForm::OfferKeyEventL( aKeyEvent, aType );
            break;
            }
        
        case EKeyYes:
            {
        	if( iDocument.MessageType( ) != EPostcardSent ) 
        	    { // This is draft message
       			if( IsEmpty( ) )
       			    {
       			    DoAddRecipientL( );
       			    }
       			else
       			    {
       			    SendL();
       			    }
       			resp=EKeyWasConsumed;   
   			    break;
        	    }
        	// If message is sent -> intentionally let it fall thru
            }
            //lint -fallthrough
        case EKeyEscape:
            // Update recipient to entry
            UpdateRecipientToEntryL();
            // Escape dismisses (destroys) the dialog
            return CAknForm::OfferKeyEventL( aKeyEvent, aType );

        case EKeyApplication:   
        case EKeyPhoneEnd:
        case EKeyPowerOff:
        case EKeyGripOpen:
        case EKeyGripClose:
        case EKeySide:
        default:
            resp=CAknForm::OfferKeyEventL( aKeyEvent, aType );
            CheckTextExceedingL();
            break;
 
        }

    return resp;
    }

// ---------------------------------------------------------
// UpdateRecipientToEntryL
// ---------------------------------------------------------
void CPostcardAddressForm::UpdateRecipientToEntryL()
    {
    if( iDocument.MessageType( ) != EPostcardSent )
		{
	    // Create an empty (ie. no fields) contact card
	    CContactCard* card = CContactCard::NewLC();
		
	    for(TInt loop = EPostcardAddressName; loop<=EPostcardAddressCountry; loop++ )
	    	{
			CEikEdwin* edwin = static_cast< CEikEdwin* >(ControlOrNull( loop ) );    	
	    	if( edwin && edwin->TextLength( ) > 0 )
	    		{ // The field is not empty: lets add field with right label
                TPostcardUtils::AddContactFieldL( *card,
                    TPostcardUtils::ContactItemNameFromId( loop ),
                    edwin->Text()->Read( 0, edwin->TextLength() ) );
    			}
    		}
		CleanupStack::Pop( card );
		// Update AppUi's contact.
		iAppUi.SetRecipientL( *card );
        // Recipient changed, so we can change field to modified.
        iDocument.SetChanged( EPostcardRecipient );
		}
    }

// ---------------------------------------------------------
// PreLayoutDynInitL
// ---------------------------------------------------------
void CPostcardAddressForm::PreLayoutDynInitL()
    {
    SetEditableL(ETrue);
    }

// ---------------------------------------------------------
// PostLayoutDynInitL
// ---------------------------------------------------------
void CPostcardAddressForm::PostLayoutDynInitL()
    {

	// Read the mandatory fields
	TInt mandatories = iAppUi.MandatoryAddressFields( );

    for(TInt loop = EPostcardAddressName; loop<=EPostcardAddressCountry; loop++ )
    	{
		CEikEdwin* edwin = static_cast< CEikEdwin* >(ControlOrNull( loop ) );
    	if( edwin )
    		{
            TInt max = iAppUi.MaxTextLength(loop);
    		edwin->SetMaxLength( max );
    		if( max < 1 )
    			{ // If max is less than 1 -> make it invisible
	    		DeleteLine( loop, ETrue );
    			}
    		else
    			{
    			if( loop == EPostcardAddressZip )
    				{
    				edwin->SetAknEditorInputMode( EAknEditorNumericInputMode );
    				}
                // Field is visible, let's see if it's mandatory too
                TInt bit = 1 << ( loop - EPostcardAddressName );
                if ( bit & mandatories )
        			{ // This control is mandatory so lets add asterisk
                    CEikLabel* label = ControlCaption( loop );
                    HBufC* mandLabel = HBufC::NewLC( label->Text()->Length() + 2 );                    
                    TPtr labelPtr = mandLabel->Des( );
                    labelPtr.Zero( );
                    labelPtr.Append( _L("* ") );
                    labelPtr.Append( *label->Text( ) );
                    SetControlCaptionL( loop, *mandLabel );
                    CleanupStack::PopAndDestroy( mandLabel ); // mandLabel 
        			}
    			}
    		}
    	}

    UpdateFieldsL( iContactItem );
    
	if( iDocument.MessageType( ) == EPostcardSent ) 
	    { // Set all fields read-only
        for(TInt loop = EPostcardAddressName; loop<=EPostcardAddressCountry; loop++ )
        	{
    		CEikEdwin* edwin = static_cast< CEikEdwin* >(ControlOrNull( loop ) );    	
    		if( edwin )
    			{
    			edwin->AddFlagToUserFlags( CEikEdwin::EReadOnly );
    			edwin->AddFlagToUserFlags( CEikEdwin::EAvkonDisableCursor );
    			}
        	}    
	    }
    
    SetInitialCurrentLine( );
    SetEditableL(ETrue);

    SetMiddleSoftkeyL();
    }

// ---------------------------------------------------------
// SetInitialCurrentLine
// ---------------------------------------------------------
void CPostcardAddressForm::SetInitialCurrentLine()
    {
    // In case something else than Name field is focused
    // -> it means we have already been here so there's no need to call this again
    TInt focused = IdOfFocusControl( );
	TInt a = EPostcardAddressName;
	if( focused == a )
		{
	    CAknForm::SetInitialCurrentLine();		
		}
    }

// ---------------------------------------------------------
// HandleResourceChange
// ---------------------------------------------------------
void CPostcardAddressForm::HandleResourceChange(TInt aType )
    {
    CAknForm::HandleResourceChange( aType );
    }

// ---------------------------------------------------------
// CPostcardAddressDialog::ProcessCommandL
// ---------------------------------------------------------
void CPostcardAddressForm::ProcessCommandL( TInt aCommandId )
    {
    HideMenu( );

    switch ( aCommandId )
        {
        case EPostcardCmdSend:
            SendL( );
            break;

        case EPostcardCmdForward:
            ForwardMessageL( );
            break;

       	case EPostcardCmdMessageInfo:
			MessageInfoL( );
        	break;

       	case EPostcardCmdAddRecipient:
       	case EPostcardCmdReplaceRecipient:
       	    // These MSK commands are not active in viewer mode
       		DoAddRecipientL( );
            SetMiddleSoftkeyL();
        	break;
        	
        case EMsgDispSizeAutomatic:
        case EMsgDispSizeLarge:
        case EMsgDispSizeNormal:        	
        case EMsgDispSizeSmall:
            iAppUi.HandleLocalZoomChangeL( (TMsgCommonCommands)aCommandId );
            break;  
        case EAknCmdHelp: 
            LaunchHelpL( );
            break;

        case EAknCmdExit:
        case EEikCmdExit:
            if( OkToExitL( aCommandId ) )
                {
            	iRetValue = EPostcardDialogExit;	
                TryExitL( EAknSoftkeyDone );
                }
            break;

        default :
            CAknForm::ProcessCommandL( aCommandId );
            break;
        }
    }

// ---------------------------------------------------------
// DynInitMenuPaneL
// ---------------------------------------------------------
void CPostcardAddressForm::DynInitMenuPaneL
        (TInt aResourceId, 
        CEikMenuPane* aMenuPane)
    {
    switch ( aResourceId )
        {
        case R_POSTCARD_ADDRESS_OPTIONS:
            DynInitMainMenuL( aMenuPane );
            break;
        case R_POSTCARD_ZOOM_SUBMENU:
            {
            TInt zoomLevel = KErrGeneral;
            iAppUi.GetZoomLevelL( zoomLevel );
            switch ( zoomLevel )
                {
                case EAknUiZoomAutomatic:
                    aMenuPane->SetItemButtonState( EMsgDispSizeAutomatic,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomNormal:
                    aMenuPane->SetItemButtonState( EMsgDispSizeNormal,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomSmall:
                    aMenuPane->SetItemButtonState( EMsgDispSizeSmall,
                                     EEikMenuItemSymbolOn );
                    break;
                case EAknUiZoomLarge:
                    aMenuPane->SetItemButtonState( EMsgDispSizeLarge,
                                     EEikMenuItemSymbolOn );
                    break;
                default:
                    break;
                }
            }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// DynInitMainMenuL
// ---------------------------------------------------------
void CPostcardAddressForm::DynInitMainMenuL( CEikMenuPane* aMenuPane )
    {
    aMenuPane->SetItemDimmed( EPostcardCmdSend, 
        IsMainMenuOptionInvisible( EPostcardCmdSend ));
    aMenuPane->SetItemDimmed( EPostcardCmdAddRecipient, 
        IsMainMenuOptionInvisible( EPostcardCmdAddRecipient ));
    aMenuPane->SetItemDimmed( EPostcardCmdReplaceRecipient, 
        IsMainMenuOptionInvisible( EPostcardCmdReplaceRecipient ));
    aMenuPane->SetItemDimmed( EPostcardCmdForward, 
        IsMainMenuOptionInvisible( EPostcardCmdForward ));
    aMenuPane->SetItemDimmed( EPostcardCmdMessageInfo, 
        IsMainMenuOptionInvisible( EPostcardCmdMessageInfo ));
    aMenuPane->SetItemDimmed( EAknCmdHelp, iAddrFlags & EPostcardAddressHelpSupported?EFalse:ETrue ); 
    aMenuPane->SetItemDimmed( EAknCmdExit, EFalse ); 
    }

// ---------------------------------------------------------
// IsMainMenuOptionInvisible
// ---------------------------------------------------------
TBool CPostcardAddressForm::IsMainMenuOptionInvisible( TInt aMenuOptionId )
    {
    TBool retVal = ETrue;
    TPostcardMsgType msgType = iDocument.MessageType( );

    switch ( aMenuOptionId )
        {

        case EPostcardCmdSend:
       		if( msgType != EPostcardSent ) 
       			{
       			if( !IsEmpty( ) )
       			    {
    	            retVal = EFalse;   			    
       			    }
       			}
            break;
       	case EPostcardCmdAddRecipient:
       		if( msgType != EPostcardSent ) 
       			{
       			if( IsEmpty( ) )
       				{
		       		retVal = EFalse;   				       					
       				}
       			}
        	break;
       	case EPostcardCmdReplaceRecipient:
       		if( msgType != EPostcardSent ) 
       			{
       			if( !IsEmpty( ) )
       				{
		       		retVal = EFalse;   				       					
       				}
       			}
        	break;
        case EPostcardCmdForward:
            if( msgType == EPostcardSent ) 
                {
                retVal = EFalse;                
                }
            break;
       	case EPostcardCmdMessageInfo:
       	    {
			retVal = EFalse;
        	break;        	
       	    }
        case EAknCmdHelp: 
        	if( iAddrFlags & EPostcardAddressHelpSupported )
        		{
	        	retVal = EFalse;
        		}
            break;

        case EAknCmdExit:
        	retVal = EFalse;
            break;
        	
        default:
            break;
        }

    return retVal;
    }

// ---------------------------------------------------------
// DoSelectionKeyL
// ---------------------------------------------------------
void CPostcardAddressForm::DoSelectionKeyL( )
    {
    TPostcardMsgType msgType = iDocument.MessageType( );

	if( msgType != EPostcardSent ) 
    	{
		DoAddRecipientL( );
    	}
    }

// ---------------------------------------------------------
// OkToExitL(TInt aButtonId)
// called by framework when the OK button is pressed
// ---------------------------------------------------------
TBool CPostcardAddressForm::OkToExitL( TInt aButtonId )
    {
    TBool okToExit = CAknForm::OkToExitL( aButtonId );
    if( aButtonId == EAknSoftkeyDone || aButtonId == EPostcardCmdSend )
    	{
    	UpdateRecipientToEntryL();
    	return ETrue;
    	}
    if( aButtonId == EAknCmdExit )
        {
        return ETrue;
        }
    if( aButtonId == EAknSoftkeyEdit || aButtonId == EPostcardCmdAdd )
        {
        DoAddRecipientL();
        CheckTextExceedingL( );
        SetMiddleSoftkeyL();
        return EFalse;
        }
    if( !okToExit )
    	{
    	return okToExit;
    	}
    if( aButtonId == EAknSoftkeyBack ||
    	aButtonId == EAknSoftkeyExit || 
    	aButtonId == EAknSoftkeyDone ||
    	aButtonId == EAknCmdExit ||
    	aButtonId == EEikCmdExit )
    	{
    	return okToExit;
    	}
    return EFalse;
    }

// ---------------------------------------------------------
// ActivateL()
// ---------------------------------------------------------
void CPostcardAddressForm::ActivateL( )
    {
    SetFocus( ETrue );
    TInt focused = IdOfFocusControl( );
	CEikEdwin* edwin = static_cast< CEikEdwin* >(Control( focused ) );
	edwin->AddEdwinObserverL( this );
    CCoeControl::ActivateL( );

	// After activating -> lets set the max. lengths and hide if needed

	TryChangeFocusToL( iFirstActive==KErrNotFound?EPostcardAddressName:iFirstActive );

    if( iFirstActive !=  KErrNotFound )
		{ // Lets show a note according to iFirstActive
		TInt resourceId;
		switch( iFirstActive )
			{
			case EPostcardAddressName:
				resourceId = R_POSTCARD_ADDRESS_NAME;
				break;
			case EPostcardAddressInfo:
				resourceId = R_POSTCARD_ADDRESS_INFO;
				break;
			case EPostcardAddressStreet:
				resourceId = R_POSTCARD_ADDRESS_STREET;
				break;
			case EPostcardAddressZip:
				resourceId = R_POSTCARD_ADDRESS_ZIP;
				break;
			case EPostcardAddressCity:
				resourceId = R_POSTCARD_ADDRESS_CITY;
				break;
			case EPostcardAddressState:
				resourceId = R_POSTCARD_ADDRESS_STATE;
				break;
			default:
				resourceId = R_POSTCARD_ADDRESS_COUNTRY;
				break;			
			}

		CEikEdwin* edwin2 = static_cast< CEikEdwin* >(ControlOrNull( iFirstActive ) );    	
    	if( edwin2 && edwin2->TextLength( ) )
    	    { // It's too long
            if( edwin2->TextLength( ) > edwin2->MaxLength( ) + 1 )
                { // Exceeded by many
                TInt howMany = edwin2->TextLength( ) - edwin2->MaxLength( );
		        HBufC* warningString = 
		            StringLoader::LoadLC( R_POSTCARD_ADDRESS_EXCEEDED_MANY, howMany, iCoeEnv );
				iAppUi.ShowConfirmationQueryL( *warningString, ETrue );
		        CleanupStack::PopAndDestroy( warningString );
                }
            else
                { // Exceeded by one
        		iAppUi.ShowConfirmationQueryL( R_POSTCARD_ADDRESS_EXCEEDED, ETrue );
                }
    	    iAddrFlags |= EPostcardAddressTextExceeded;
    	    }
    	else
    	    { // It's empty
    	    HBufC* item = StringLoader::LoadLC( resourceId, iCoeEnv );
    	    CDesCArray* stringArray = new ( ELeave ) CDesCArrayFlat( 2 );
    	    CleanupStack::PushL( stringArray );

    		stringArray->AppendL( *item );
    		stringArray->AppendL( *item );

    		HBufC* note = StringLoader::LoadLC( R_POSTCARD_SEND_FIELD_EMPTY, *stringArray, iCoeEnv );

    		iAppUi.ShowInformationNoteL( note->Des( ), EFalse );
    		CleanupStack::PopAndDestroy( 3, item ); // item, stringarray, note
    	    }
		}

	if( iKeyEvent.iCode != 0 )
		{
		TEventCode type = EEventKey;
        iCoeEnv->SyncNotifyFocusObserversOfChangeInFocus();
        iCoeEnv->SimulateKeyEventL( iKeyEvent, type );						
		}
    }

// ---------------------------------------------------------
// HandleControlStateChangeL
// ---------------------------------------------------------
void CPostcardAddressForm::HandleControlStateChangeL( TInt /*aControlId*/ )
    {

    }

// ---------------------------------------------------------
// HandleEdwinEventL
// ---------------------------------------------------------
void CPostcardAddressForm::HandleEdwinEventL(CEikEdwin* /*aEdwin*/, TEdwinEvent /*aEventType*/ )
	{
	// The functionality has been removed to HandleControlEventL
	}

// ---------------------------------------------------------
// LineChangedL
// ---------------------------------------------------------
void CPostcardAddressForm::LineChangedL(TInt /*aControlId*/)
	{
    iAddrFlags &= ~EPostcardAddressTextExceeded;
	}

// ---------------------------------------------------------
// SendL
// ---------------------------------------------------------
void CPostcardAddressForm::SendL( )
	{
	iRetValue = EPostcardDialogSend;	
    TryExitL( EPostcardCmdSend );		
	}

// ---------------------------------------------------------
// ForwardMessageL
// ---------------------------------------------------------
void CPostcardAddressForm::ForwardMessageL( )
    {
    iRetValue = EPostcardDialogForward;    
    TryExitL( EAknSoftkeyDone );        
    }

// ---------------------------------------------------------
// MessageInfoL
// ---------------------------------------------------------
void CPostcardAddressForm::MessageInfoL( )
	{
    TPostcardMsgType msgType = iDocument.MessageType( );
	if( msgType != EPostcardSent ) 
        { // We are dealing with unsent message -> save recipient details for now
		CEikEdwin* edwin = static_cast< CEikEdwin* >(ControlOrNull( EPostcardAddressName ) );    	
    	if( edwin )
    	    {
            TMmsMsvEntry tEntry = static_cast<TMmsMsvEntry>( iDocument.Entry() );
            tEntry.iDate.UniversalTime();
            tEntry.iDetails.Set(edwin->Text( )->Read( 0, edwin->TextLength( ) ) );
        	tEntry.iBioType = KUidMsgSubTypeMmsPostcard.iUid;
            iDocument.CurrentEntry().ChangeL( tEntry );
    	    }        
        }	
	iAppUi.DoShowMessageInfoL( );
	}

// ---------------------------------------------------------
// LaunchHelpL
// ---------------------------------------------------------
void CPostcardAddressForm::LaunchHelpL( )
	{
	if( iDocument.MessageType( ) == EPostcardSent ) 
	    {
        iAppUi.LaunchHelpL( KPOSTCARD_HLP_ADDRESS_VIEW() );
	    }
    else
        {
        iAppUi.LaunchHelpL( KMMS_POSTCARD_HLP_ADDRESS_EDIT() );    
        }	
	}

// ---------------------------------------------------------
// CheckTextExceedingL
// ---------------------------------------------------------
void CPostcardAddressForm::CheckTextExceedingL( )
	{
	if( iDocument.MessageType( ) == EPostcardSent ) 
	    { // we are just viewing the fields so no reason to show warnings
	    return;
	    }
    TInt focused = IdOfFocusControl( );
	CEikEdwin* edwin = static_cast< CEikEdwin* >(Control( focused ) );
	if ( !edwin )
		{ // just in case
		return;
		}
	TInt maxLength = edwin->MaxLength( );
	
	if( edwin->TextLength( ) >= maxLength )
		{
		TInt howMany = edwin->TextLength( ) - maxLength;
		switch( howMany )
			{
			case 0:
				{
                // No note shown anymore.
				break;
				}
			case 1:
				{
				if( !( iAddrFlags & EPostcardAddressTextExceeded ) )
				    {
    				iAppUi.ShowConfirmationQueryL( R_POSTCARD_ADDRESS_EXCEEDED, ETrue );
				    }
				break;					
				}
			default:
				{
				if( !( iAddrFlags & EPostcardAddressTextExceeded ) )
				    {
    		        HBufC* warningString = 
    		            StringLoader::LoadLC( R_POSTCARD_ADDRESS_EXCEEDED_MANY, howMany, iCoeEnv );
    				iAppUi.ShowConfirmationQueryL( *warningString, ETrue );
    		        CleanupStack::PopAndDestroy( warningString );
				    }
				break;					
				}
			}
	    iAddrFlags |= EPostcardAddressTextExceeded;
		}
    else
        {
        iAddrFlags &= ~EPostcardAddressTextExceeded;
        }
	}

// ---------------------------------------------------------
// DoAddRecipientL
// ---------------------------------------------------------
void CPostcardAddressForm::DoAddRecipientL( )
    {
    if ( !iContact )
        {
        iContact = CPostcardContact::NewL( iCoeEnv->FsSession() );
        }
    iContact->FetchContactL();

    if ( iContact->IsValid() )
        {
        // Reset the flag, so that user is notified if selected recipient
        // has too long address fields.
        iAddrFlags &= ~EPostcardAddressTextExceeded;
        
        CPostcardRecipientWrapper* recWrapper = 
            CPostcardRecipientWrapper::NewL( iDocument, iAppUi, *iContact );
                CleanupStack::PushL( recWrapper );

        // Location asking is done in recipientWrapper
        CPostcardContact::TLocation location;
        TBool hasLocation = recWrapper->AskLocationL( location );
        CleanupStack::PopAndDestroy( recWrapper );

        if ( hasLocation )
            {
            UpdateFieldsL( location );
            RefreshDisplay(); //	Refresh the dislay
            }
        }
	}

// ---------------------------------------------------------
// UpdateFieldsL
// ---------------------------------------------------------
void CPostcardAddressForm::UpdateFieldsL( CContactItem& aContact )
	{
	CContactItemFieldSet& fieldSet =  aContact.CardFields();    

	for( TInt i = 0; i < fieldSet.Count( ); i++ )
		{
		CContactItemField& field = fieldSet[ i ];
		if( field.StorageType( ) == KStorageTypeText )
			{
			TInt destinationId = TPostcardUtils::IdFromContactItemName( field.Label( ) );
			CEikEdwin* edwin = static_cast< CEikEdwin* >(ControlOrNull( destinationId ) );    	
    		if( edwin )
    			{
			    HBufC* text = field.TextStorage()->Text().AllocLC( );
    		    edwin->SetTextL( text );
    		    edwin->SetCursorPosL( text->Length( ) - 1, EFalse );
			    CleanupStack::PopAndDestroy( text );
    			}
			}
		}
	}

// ---------------------------------------------------------
// UpdateFieldsL
// ---------------------------------------------------------
void CPostcardAddressForm::UpdateFieldsL(
    CPostcardContact::TLocation aLocation )
    {
    // Update address fields
    for( TInt i = EPostcardAddressInfo; i <= EPostcardAddressCountry; i++ )
        {
        CEikEdwin* edwin = static_cast<CEikEdwin*>( ControlOrNull( i ) );
        if ( edwin )
            {
            HBufC* text = iContact->GetAddressFieldLC( aLocation,
                CPostcardContact::ControlIdToAddrField( i ) );
            edwin->SetTextL(text->Length() ? text:NULL);
            CleanupStack::PopAndDestroy( text );  // text
            }
        }

    // Update the name
	CEikEdwin* edwin =
	    static_cast<CEikEdwin*>( ControlOrNull( EPostcardAddressName ) );
	if ( edwin )
	    {
	    HBufC* contactName = iContact->GetNameLC();
        edwin->SetTextL( contactName->Length() ? contactName:NULL );
        CleanupStack::PopAndDestroy( contactName );  // contactName
        }
    }
		
// ---------------------------------------------------------
// IsEmpty
// ---------------------------------------------------------
TBool CPostcardAddressForm::IsEmpty( )
	{
    for(TInt loop = EPostcardAddressName; loop<=EPostcardAddressCountry; loop++ )
    	{
		CEikEdwin* edwin = static_cast< CEikEdwin* >(ControlOrNull( loop ) );    	
		if( edwin && edwin->TextLength( ) > 0 )
			{
			return EFalse;
			}
    	}
    return ETrue;
	}

// ---------------------------------------------------------
// HandleControlEventL
// ---------------------------------------------------------
void CPostcardAddressForm::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent aEventType)
    {
    if( aEventType == EEventStateChanged )
        {

        // Update middle softkey
        SetMiddleSoftkeyL();
        }
    }

// ---------------------------------------------------------
// CleanNavipane
// ---------------------------------------------------------
void CPostcardAddressForm::CleanNavipane()
    {
    // Clean navigation pane, otherwise it can contain e.g.
    // character counter from greeting text editor, when user
    // tries to send a postcard without recipient's address.
    MAknEditingStateIndicator* editIndi = 
        static_cast< CAknEnv* >( iEikonEnv->Extension( ) )->EditingStateIndicator();
	if( editIndi )
		{
        CAknIndicatorContainer* indiContainer = editIndi->IndicatorContainer( );
        if ( indiContainer )
            {
            indiContainer->SetIndicatorState(
	                TUid::Uid( EAknNaviPaneEditorIndicatorMessageLength ),  
	                EAknIndicatorStateOff );
            }
		}    
    }


// ---------------------------------------------------------
// RefreshDisplay
// ---------------------------------------------------------
void CPostcardAddressForm::RefreshDisplay( )
    {
    for(TInt loop = EPostcardAddressName; loop<=EPostcardAddressCountry; loop++ )
    	{
		CEikEdwin* edwin = static_cast< CEikEdwin* >(ControlOrNull( loop ) );    	
		if( edwin )
			{
			if( edwin->IsVisible( ) )
			    {
		        edwin->DrawNow();	    
			    }
			}
    	}
    }

// ---------------------------------------------------------
// SetMiddleSoftkeyL
// ---------------------------------------------------------
void CPostcardAddressForm::SetMiddleSoftkeyL()
    {
    TInt resourceId = R_POSTCARD_MSK_BUTTON_EMPTY;

    // Applying changes of the cr 403-1527 always from 5.0 onwards    
    if ( iDocument.MessageType( ) != EPostcardSent )
        {
        if( IsEmpty() )
            {
            resourceId = R_POSTCARD_MSK_BUTTON_ADD;
            }
        else if( !IsEmpty() && IdOfFocusControl() == EPostcardAddressName )
            {
            resourceId = R_POSTCARD_MSK_BUTTON_REPLACE;
            }
        }
   
    if ( resourceId != iMskResource)
        {
        const TInt KMskPosition = 3;
        CEikButtonGroupContainer& cba = ButtonGroupContainer();
        cba.SetCommandL( KMskPosition, resourceId );
        cba.DrawDeferred();
        iMskResource = resourceId;
        }
    }
        
// End of File
