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
*     This contains all the state specific logic of the bio-control.
*
*/



// INCLUDE FILES
#include "WmlState.h"                   // Own header
#include "WmlItemEngine.h"
#include "WmlBioControl.h"              // CWmlBioControl
#include "WmlBioControl.pan"              // Panic
#include "WmlSubItemBase.h"             // CWmlSubItemBase

#include <StringLoader.h>               // StringLoader
#include <msgnamevalue.h>               // CMsgNameValue
#include <msgbiocontrolObserver.h>
#include <CRichBio.h>                   // CRichBio
#include <WMLBC.rsg>                    // resouce identifiers
#include <bldvariant.hrh>
#include <csxhelp/smart.hlp.hrh>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWmlStateFactory::NewL()
// ---------------------------------------------------------
CWmlStateFactory* CWmlStateFactory::NewL( CWmlBioControl& aBioControl )
    {
    CWmlStateFactory* self = new ( ELeave ) CWmlStateFactory;
    CleanupStack::PushL( self );
    self->ConstructL( aBioControl );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWmlStateFactory::ConstructL()
// ---------------------------------------------------------
void CWmlStateFactory::ConstructL( CWmlBioControl& aBioControl )
    {
    iBioControl = &aBioControl;
    iStateSingle = new (ELeave) CWmlStateSingle( this, aBioControl );
    }

// ---------------------------------------------------------
// CWmlStateFactory::~CWmlStateFactory()
// ---------------------------------------------------------
CWmlStateFactory::~CWmlStateFactory()
    {
    delete iStateSingle;
    }

// ---------------------------------------------------------
// CWmlStateFactory::SetInitialStateL()
// ---------------------------------------------------------
void CWmlStateFactory::SetInitialStateL( 
    const TInt aBMCount )
    {
    // Pre-condition is that state object is NULL
    __ASSERT_DEBUG(iState == NULL, 
        Panic(EWmlBCStateObjectAlreadyCreated));

    if ( aBMCount == 0 )
        {
        // nothing to show... invalid message.
        User::Leave( KErrCorrupt );
        }

    TWmlState state( EWmlBCInitial );

    if ( aBMCount == 1 )
    	{
        state = EWmlBCViewerBookmarkOne;
        }
    else
    	{
        // invalid message
        User::Leave( KErrNotSupported );
        }

    ChangeStateL( state );
    // Post condition is that we have state object.
    __ASSERT_DEBUG(iState, 
        Panic(EWmlBCStateObjectNotCreated));
    }

// ---------------------------------------------------------
// CWmlStateFactory::ChangeStateL()
// ---------------------------------------------------------
void CWmlStateFactory::ChangeStateL( TWmlState aState )
    {
    iStateEnum = aState;
    // Set the correct state object.
    switch ( aState )
        {
        // Single mode
        case EWmlBCViewerBookmarkOne:
            {
            iState = iStateSingle;
            break;
            }
        // Illegal transitions
        default:
            {
            Panic( EIllegalStateTransition );
            break;
            }
        }
    iState->EnterL( aState );
    }

// ---------------------------------------------------------
// CWmlStateFactory::State()
// ---------------------------------------------------------
MWmlState& CWmlStateFactory::State() const
    {
    return *iState;
    }

// ---------------------------------------------------------
// CWmlState::CWmlState()
// ---------------------------------------------------------
CWmlState::CWmlState( CWmlStateFactory* aStateFactory, 
    CWmlBioControl& aBioControl) : 
    iStateFactory( aStateFactory ),
    iBioControl( &aBioControl )
    {
    // Nothing more here...
    }

// ---------------------------------------------------------
// CWmlStateSingle::CWmlStateSingle()
// ---------------------------------------------------------
CWmlStateSingle::CWmlStateSingle( CWmlStateFactory* aStateFactory,
    CWmlBioControl& aBioControl) : 
    CWmlState( aStateFactory, aBioControl )
    {
    // Nothing more here...
    }

// ---------------------------------------------------------
// CWmlStateSingle::~CWmlStateSingle()
// ---------------------------------------------------------
CWmlStateSingle::~CWmlStateSingle()
    {
    // This owns nothing...
    }

// ---------------------------------------------------------
// CWmlStateSingle::SetAndGetSizeL()
// ---------------------------------------------------------
void CWmlStateSingle::SetAndGetSizeL( TSize& aSize )
    {
    iBioControl->iRichBio->SetAndGetSizeL( aSize );
    }

// ---------------------------------------------------------
// CWmlStateSingle::HandleBioCommandL()
// ---------------------------------------------------------
TBool CWmlStateSingle::HandleBioCommandL( TInt aCommand )
    {
    TBool retValue(EFalse);
    aCommand -= iBioControl->iBioControlObserver.FirstFreeCommand();
    switch ( aCommand )
        {
        case EAddBookmark:
            {
            iBioControl->iItemEngine->SaveCurrentItemL();
            retValue = ETrue;
            break;
            }
        default:
            {
            break;
            }
        }
    return retValue;
    }

// ---------------------------------------------------------
// CWmlStateSingle::CurrentLineRect()
// ---------------------------------------------------------
TRect CWmlStateSingle::CurrentLineRect() const
    {    
    return iBioControl->iRichBio->CurrentLineRect();
    }

// ---------------------------------------------------------
// CWmlStateSingle::IsFocusChangePossible()
// ---------------------------------------------------------
TBool CWmlStateSingle::IsFocusChangePossible( 
	TMsgFocusDirection aDirection ) const
    {
    if (aDirection == EMsgFocusUp)
    	{
    	return CursorInFirstLine();
    	}
    else
    	{
    	// Focus down will never release the focus out of this.
    	return EFalse;
    	}
    }

// ---------------------------------------------------------
// CWmlStateSingle::HeaderTextLC()
// ---------------------------------------------------------
HBufC* CWmlStateSingle::HeaderTextLC() const
    {
    TInt resourceID(0);
    switch ( iState )
        {
        case EWmlBCViewerBookmarkOne:
            {
            resourceID = R_QTN_SM_TITLE_ONE_BOOKMARK;
            break;
            }
        default:
            {
            // In detail state iState should not be anything else than above.
            Panic( EIllegalInternalState );
            break;
            }
        }
    return StringLoader::LoadLC( resourceID );
    }

// ---------------------------------------------------------
// CWmlStateSingle::OptionMenuPermissionsL()
// ---------------------------------------------------------
void CWmlStateSingle::OptionMenuPermissionsL( TUint32& aPermissions ) const
    {    
    switch ( iState )
        {
        case EWmlBCViewerBookmarkOne:
            {
            aPermissions |= EMsgBioCallBack
                | EMsgBioDelete
                | EMsgBioMessInfo
                | EMsgBioMove
                | EMsgBioHelp
                | EMsgBioCreateCC;
            break;
            }
        default:
            {
            Panic(EIllegalCommandInCurrentState);
            break;
            }
        }
    }

// ---------------------------------------------------------
// CWmlStateSingle::SetMenuCommandSetL()
// ---------------------------------------------------------
void CWmlStateSingle::SetMenuCommandSetL( CEikMenuPane& aMenuPane )
    {
    switch ( iState )
        {
        case EWmlBCViewerBookmarkOne:
            {
            iBioControl->AddMenuItemL( aMenuPane, R_QTN_SM_ADD_BOOKMARKS,
                EAddBookmark, 1 );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CWmlStateSingle::VirtualHeight()
// ---------------------------------------------------------
TInt CWmlStateSingle::VirtualHeight()
    {    
    TInt height = iBioControl->iRichBio->VirtualHeight();
    return height;
    }

// ---------------------------------------------------------
// CWmlStateSingle::VirtualVisibleTop()
// ---------------------------------------------------------
TInt CWmlStateSingle::VirtualVisibleTop()
    {    
    TInt top = iBioControl->iRichBio->VirtualVisibleTop();
    return top;
    }

// ---------------------------------------------------------
// CWmlStateSingle::IsCursorLocation()
// ---------------------------------------------------------
TBool CWmlStateSingle::IsCursorLocation( TMsgCursorLocation aLocation ) const
    {    
    TBool location( iBioControl->iRichBio->IsCursorLocation( aLocation ) );
    return location;
    }

// ---------------------------------------------------------
// CWmlStateSingle::OfferKeyEventL()
// ---------------------------------------------------------
TKeyResponse CWmlStateSingle::OfferKeyEventL( const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {    
    return iBioControl->iRichBio->OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CWmlStateSingle::SizeChanged()
// ---------------------------------------------------------
void CWmlStateSingle::SizeChanged()
    {    
    iBioControl->iRichBio->SetExtent(iBioControl->Position(), 
    	iBioControl->iRichBio->Size());
    }

// ---------------------------------------------------------
// CWmlStateSingle::FocusChanged()
// ---------------------------------------------------------
void CWmlStateSingle::FocusChanged( TDrawNow aDrawNow )
    {    
    iBioControl->iRichBio->SetFocus( iBioControl->IsFocused(), aDrawNow );
    }

// ---------------------------------------------------------
// CWmlStateSingle::CreateViewerL()
// ---------------------------------------------------------
void CWmlStateSingle::CreateViewerL( const CCoeControl& aContainer )
    {
    CRichBio* richBio = CRichBio::NewL( &aContainer, ERichBioModeStandard );
    CleanupStack::PushL( richBio );

    CWmlSubItemBase& currentItem = iBioControl->iItemEngine->CurrentItem();
    CArrayPtrFlat<CMsgNameValue>* labelsAndValues = 
		currentItem.LabelsAndValuesLC();
    TInt count = labelsAndValues->Count();
    for ( TInt i = 0; i < count; i++ )
    	{
        CMsgNameValue* nameValue = labelsAndValues->At( i );
        richBio->AddItemL( nameValue->Name(), nameValue->Value() );
        }
    // We can't check the item because the object actually is 
    // TCleanupItem
    CleanupStack::PopAndDestroy();// labelsAndValues );

    CleanupStack::Pop( richBio );
    iBioControl->iRichBio = richBio;
    }

// ---------------------------------------------------------
// CWmlStateSingle::CursorInFirstLine()
// ---------------------------------------------------------
TBool CWmlStateSingle::CursorInFirstLine() const
    {    
    return iBioControl->iRichBio->IsCursorLocation(EMsgTop);
    }

// ---------------------------------------------------------
// CWmlStateSingle::GetHelpContext()
// ---------------------------------------------------------
void CWmlStateSingle::GetHelpContext(TCoeHelpContext& aHelpContext) const
    {
    switch ( iState )
        {
        case EWmlBCViewerBookmarkOne:
            {
            aHelpContext.iContext = KSMART_HLP_ONEBOOKVIEWER;
            break;
            }        
        default:
            {
            Panic( EIllegalStateTransition );
            break;
            }
        }
	}

// ---------------------------------------------------------
// CWmlStateSingle::EnterL()
// ---------------------------------------------------------
void CWmlStateSingle::EnterL( TWmlState aState )
    {
    // This is a check for valid state transitions.
    switch ( aState )
        {
        case EWmlBCViewerBookmarkOne:
            {
            break;
            }
        default:
            {
            Panic( EIllegalStateTransition );
            break;
            }
        }
    iState = aState;
    iBioControl->UpdateHeaderTextL();
    }

// End of file
