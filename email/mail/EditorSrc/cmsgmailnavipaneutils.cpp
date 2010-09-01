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
* Description:  Navi pane handling utilities
*
*/


// Mail includes
#include "cmsgmailnavipaneutils.h"
#include <avkon.hrh>
#include <aknenv.h>
#include <AknsUtils.h>
#include <AknIndicatorContainer.h>
#include <MsgAttachmentUtils.h>
#include <AknIconUtils.h>
#include <aknEditStateIndicator.h>
#include <aknnavi.h>    // for CAknNavigationControlContainer
#include <aknnavide.h>  // for CAknNavigationDecorator
#include <avkon.mbg>

// LOCAL CONSTANTS

const TInt KMaxNaviMailSize(9999*1024); // largest size shown in navi pane
const TInt KMaxNaviMailDigits(20); // some language variants require this
								   // to be longer than 10, since they
								   // write kB with more characters
								   // this will cause overlapping in 
								   // navi pane with priority 

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::NewL()
// Symbian two-phased constructor
// -----------------------------------------------------------------------------
//
CMsgMailNaviPaneUtils* CMsgMailNaviPaneUtils::NewL()
    {
    CMsgMailNaviPaneUtils* self = new( ELeave ) CMsgMailNaviPaneUtils();
    CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::CMsgMailNaviPaneUtils()
// A default constructor
// -----------------------------------------------------------------------------
//
CMsgMailNaviPaneUtils::CMsgMailNaviPaneUtils()
    {
    }

// -----------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::ConstructL()
// -----------------------------------------------------------------------------
//
void CMsgMailNaviPaneUtils::ConstructL()
    {
    CEikStatusPane* statusPane =
        CEikonEnv::Static()->AppUiFactory()->StatusPane();					// CSI: 27 # Must be used because of iEikEnv 
																			// is not accessible.
    ASSERT( statusPane );
    // We don't own iNaviPane. 
    iNaviPane = static_cast<CAknNavigationControlContainer*>(
        statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    ASSERT( iNaviPane );
    // iNaviDecorator must be deleted in destructor. 
    iNaviDecorator = iNaviPane->CreateEditorIndicatorContainerL();
    ASSERT( iNaviDecorator );
    iNaviIndi = static_cast<CAknIndicatorContainer*>(
        iNaviDecorator->DecoratedControl() );
    ASSERT( iNaviIndi );

    // Address fields contain by default t9 and abc icons, and to avoid
    // other indicators changing position when our own navi pane is enabled or
    // disabled we will create invisible indicators.
    CreateInvisibleIndicatorL( EAknNaviPaneEditorIndicatorLowerCase );
    CreateInvisibleIndicatorL( EAknNaviPaneEditorIndicatorT9 );
    }


// -----------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::~CMsgMailNaviPaneUtils()
// -----------------------------------------------------------------------------
//
CMsgMailNaviPaneUtils::~CMsgMailNaviPaneUtils()
    {
    delete iNaviDecorator;    
    }


// ----------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::SetPriorityIndicator()
// if priority is EMsvMediumPriority then no indicator is shown
// ----------------------------------------------------------------------------
void CMsgMailNaviPaneUtils::SetPriorityIndicator( TMsvPriority aPriority )
	{
    SetIndicatorState( EAknNaviPaneEditorIndicatorMcePriorityHigh,
        ( aPriority == EMsvHighPriority ) ? ETrue : EFalse );

    SetIndicatorState( EAknNaviPaneEditorIndicatorMcePriorityLow,
        ( aPriority == EMsvLowPriority ) ? ETrue : EFalse );
	}

// -----------------------------------------------------------------------------
// void CMsgMailNaviPaneUtils::SetMessageSizeIndicatorL()
// -----------------------------------------------------------------------------
//
void CMsgMailNaviPaneUtils::SetMessageSizeIndicatorL( TInt aSize )
    {
    TBuf<KMaxNaviMailDigits> sizeString;
    if ( aSize > KMaxNaviMailSize )
        {
        aSize = KMaxNaviMailSize;
        }

    MsgAttachmentUtils::FileSizeToStringL( sizeString, aSize, ETrue ); 
    
    SetIndicatorState( EAknNaviPaneEditorIndicatorMessageLength, ETrue );
    SetIndicatorValueL( EAknNaviPaneEditorIndicatorMessageLength, sizeString );
    }

// -----------------------------------------------------------------------------
// void CMsgMailNaviPaneUtils::SetAttachmentIndicator()
// -----------------------------------------------------------------------------
//
void CMsgMailNaviPaneUtils::SetAttachmentIndicator( TBool aHasAttachments )
    {
    SetIndicatorState( EAknNaviPaneEditorIndicatorAttachment, aHasAttachments );
    }

// ---------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::EnableOwnNaviPaneL
// Enable or disable our own navi pane. 
// ---------------------------------------------------------------------------
//
void CMsgMailNaviPaneUtils::EnableOwnNaviPaneL( TBool aDoEnable )
    {
    if( aDoEnable )
        {
        // If iNaviDecorator already exists in window stack, 
        // it is put foremost. 
        iNaviPane->PushL( *iNaviDecorator );
        iNaviDecorator->MakeVisible( ETrue );
        }
    else
        {
        iNaviDecorator->MakeVisible( EFalse );
        }                
    }

// ---------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::SetIndicatorState
// Sets indicator state on/off in both default and our own navi pane
// ---------------------------------------------------------------------------
//
void CMsgMailNaviPaneUtils::SetIndicatorState(
    TInt aIndicatorId,
    TBool aDoEnable )
    {    
    TInt state( aDoEnable ? EAknIndicatorStateOn : EAknIndicatorStateOff );
    TUid indicator( TUid::Uid( aIndicatorId ) );    
    
    // default navi pane
    MAknEditingStateIndicator* ind = AVKONENV->EditingStateIndicator();
    if ( ind )
        {        
        CAknIndicatorContainer* defaultNaviIndi = ind->IndicatorContainer();    
        ASSERT( defaultNaviIndi );
        defaultNaviIndi->SetIndicatorState( indicator, state );
        }
        
    // our own navi pane
    iNaviIndi->SetIndicatorState( indicator, state );    
    }
    
// ---------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::SetIndicatorValueL
// Sets indicator value in both default and our own navi pane
// ---------------------------------------------------------------------------
//
void CMsgMailNaviPaneUtils::SetIndicatorValueL(
    TInt aIndicatorId,
    const TDesC& aString )
    {    
    TUid indicator( TUid::Uid( aIndicatorId ) );
    
    // default navi pane
    MAknEditingStateIndicator* ind = AVKONENV->EditingStateIndicator();
    if ( ind )
        {        
        CAknIndicatorContainer* defaultNaviIndi = ind->IndicatorContainer();    
        ASSERT( defaultNaviIndi );
        defaultNaviIndi->SetIndicatorValueL( indicator, aString );
        }
        
    // our own navi pane
    iNaviIndi->SetIndicatorValueL( indicator, aString );
    }

// ---------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::CreateInvisibleIndicatorL
// We add invisible "abc" and t9 icons to our navi pane so that email priority
// icon won't change it's place horizontally when focus is moved to/from 
// attachment field. This is quite ugly, but avkon doesn't provide support for
// keeping indicator in fixed location.
// ---------------------------------------------------------------------------
//
void CMsgMailNaviPaneUtils::CreateInvisibleIndicatorL( TInt aIndicatorId )
    {
    // 1. Turn indicator on   
    
    SetIndicatorState( aIndicatorId, ETrue );
    
    // 2. Make icon invisible
                    
    CFbsBitmap* bitmap( NULL );
    // Get bitmap for "abc" icon
    CreateIconLC( aIndicatorId, bitmap );
               
    // We cannot use CFbsBitmap::SizeInPixels() since it is zero due
    // to icon scalability. To get a size for our empty mask we query
    // content dimensions. All scaling and sizing of actual indicators
    // will be taken care by CAknIndicatorContainer
    TSize size;        
    User::LeaveIfError( AknIconUtils::GetContentDimensions( bitmap,
                                                            size ) );        
    CFbsBitmap* emptyMask = new (ELeave) CFbsBitmap();
    CleanupStack::PushL( emptyMask );
    User::LeaveIfError( emptyMask->Create( size, EGray2 ) );

    // Ownership of bitmap and newMask is transferred to iNaviIndi. 
    iNaviIndi->ReplaceIndicatorIconL( TUid::Uid( aIndicatorId ),
                                      EAknIndicatorStateOn, 
                                      CAknIndicatorContainer::ELayoutModeWide,
                                      bitmap,
                                      emptyMask );
    CleanupStack::Pop( 2, bitmap );						// CSI: 47,12 # emptyMask, bitmap
    }

// ---------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::CreateIconLC
//
// ---------------------------------------------------------------------------
//
void CMsgMailNaviPaneUtils::CreateIconLC(
    TInt aIndicatorId,
    CFbsBitmap*& aBitmap ) const
    {
    TAknsItemID skinId;
    TInt bitmapId;
    TInt maskId; // dummy    
    GetIconIds( aIndicatorId, skinId, bitmapId, maskId );
    MAknsSkinInstance *skin = AknsUtils::SkinInstance();    
    AknsUtils::CreateIconLC( skin,
                             skinId,
                             aBitmap,
                             AknIconUtils::AvkonIconFileName(),
                             bitmapId );    
    }

// ---------------------------------------------------------------------------
// CMsgMailNaviPaneUtils::GetIconIds
//
// ---------------------------------------------------------------------------
//
void CMsgMailNaviPaneUtils::GetIconIds(
    TInt aIndicatorId,
    TAknsItemID& aSkinId,
    TInt& aFileBitmapId,
    TInt& aFileMaskId ) const
    {
    switch ( aIndicatorId )
        {
        case EAknNaviPaneEditorIndicatorLowerCase:
            {
            aSkinId = KAknsIIDQgnStatCaseSmall;
            aFileBitmapId = EMbmAvkonQgn_stat_case_small;
            aFileMaskId = EMbmAvkonQgn_stat_case_small_mask;
            break;
            }
        case EAknNaviPaneEditorIndicatorT9:
            {
            aSkinId = KAknsIIDQgnStatT9;
            aFileBitmapId = EMbmAvkonQgn_stat_t9;
            aFileMaskId = EMbmAvkonQgn_stat_t9_mask;
            break;
            }            
        default:
            {
            // do nothing
            }
        }
    }

// End of File
