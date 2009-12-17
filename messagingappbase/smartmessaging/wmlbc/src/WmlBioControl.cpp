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
* Description:   Bio control for Wap bookmarks
*
*/



// INCLUDE FILES

#include "WmlBioControl.h"              // Own header
#include "WmlItemEngine.h"              //
#include "WmlState.h"                   // CWmlState and inherited states
#include "WmlBioControl.pan"
#include "WmlLog.h"

#include <akntitle.h>                   // CAknTitlePane
#include <aknappui.h>
#include <bsp.h>                        // CParsedField
#include <biouids.h>                    // KUidMsvBIODataStream
#include <CRichBio.h>                   // CRichBio
#include <bldvariant.hrh>               //
#include <featmgr.h>
#include <csxhelp/smart.hlp.hrh>

// ================= LOCAL FUNCTIONS =======================

/**
* This method is a helper method for destroying the contents
* of the array, which is popped from the cleanupstack.
*/
static void CleanupResetAndDestroy(TAny* aSelf)
    {
	__ASSERT_DEBUG(aSelf, User::Panic( _L("wmlbc.dll"), ENullPtr ));    
    CArrayPtrSeg<CParsedField>* self =
        static_cast<CArrayPtrSeg<CParsedField>* >(aSelf);
    self->ResetAndDestroy();
    delete self;
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWmlBioControl::NewL
// ---------------------------------------------------------
EXPORT_C CMsgBioControl* CWmlBioControl::NewL(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aIsEditor,
    const RFile* aFile)
    {
    RDebug::Print(_L("WMLBC"));
    if ( aSession == NULL )
        {
        User::Leave( KErrNotFound );
        }

    CWmlBioControl* self = new ( ELeave ) CWmlBioControl(
        aObserver, aSession, aId, aIsEditor, aFile );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CWmlBioControl::~CWmlBioControl
// ---------------------------------------------------------
CWmlBioControl::~CWmlBioControl()
    {
    FeatureManager::UnInitializeLib();
    delete iStateFactory;
    delete iRichBio;

    delete iItemEngine;
    }

// ---------------------------------------------------------
// CWmlBioControl::SetAndGetSizeL
// ---------------------------------------------------------
//
void CWmlBioControl::SetAndGetSizeL( TSize& aSize )
    {
    iStateFactory->State().SetAndGetSizeL( aSize );
    
    //the following compares the window size against the 
    //"virtual" height of the viewer. This way there is no 
    //scrolling if virtualHeight is less than window size
    if(iRichBio->VirtualHeight() <= Window().Size().iHeight)
        {
        aSize.iHeight = iRichBio->VirtualHeight();
        SetSizeWithoutNotification(aSize);    
        }
    else
        {
        SetSizeWithoutNotification(aSize);    
        }        
    }

// ---------------------------------------------------------
// CWmlBioControl::SetMenuCommandSetL
// ---------------------------------------------------------
//
void CWmlBioControl::SetMenuCommandSetL( CEikMenuPane& aMenuPane )
    {
    iStateFactory->State().SetMenuCommandSetL( aMenuPane );
    }

// ---------------------------------------------------------
// CWmlBioControl::HandleBioCommandL()
// ---------------------------------------------------------
//
TBool CWmlBioControl::HandleBioCommandL( TInt aCommand )
    {
    return iStateFactory->State().HandleBioCommandL( aCommand );
    }

// ---------------------------------------------------------
// CWmlBioControl::CurrentLineRect
// ---------------------------------------------------------
//
TRect CWmlBioControl::CurrentLineRect() const
    {
    TRect lineRect(0,0,0,0);
    lineRect = iStateFactory->State().CurrentLineRect();
    return lineRect;
    }

// ---------------------------------------------------------
// CWmlBioControl::IsFocusChangePossible
// ---------------------------------------------------------
//
TBool CWmlBioControl::IsFocusChangePossible(
    TMsgFocusDirection aDirection ) const
    {
    return iStateFactory->State().IsFocusChangePossible( aDirection );
    }

// ---------------------------------------------------------
// CWmlBioControl::HeaderTextL
// ---------------------------------------------------------
//
HBufC* CWmlBioControl::HeaderTextL() const
    {
    HBufC* title = iStateFactory->State().HeaderTextLC();
    CleanupStack::Pop( title );
    return title;
    }

// ---------------------------------------------------------
// CWmlBioControl::UpdateHeaderTextL
// ---------------------------------------------------------
//
void CWmlBioControl::UpdateHeaderTextL() const
    {
    HBufC* title = iStateFactory->State().HeaderTextLC();
    CEikStatusPane* sp =
        ((CAknAppUi*)CEikonEnv::Static()->EikAppUi())->StatusPane();
    CAknTitlePane* titlePane = STATIC_CAST(CAknTitlePane*,
        sp->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
    titlePane->SetTextL( *title );
    CleanupStack::PopAndDestroy( title );
    }

// ---------------------------------------------------------
// CWmlBioControl::OptionMenuPermissionsL
// ---------------------------------------------------------
//
TUint32 CWmlBioControl::OptionMenuPermissionsL() const
    {
    TUint32 permissions = EMsgBioExit;
    iStateFactory->State().OptionMenuPermissionsL( permissions );
    return permissions;
    }

// ---------------------------------------------------------
// CWmlBioControl::VirtualHeight
// ---------------------------------------------------------
//
TInt CWmlBioControl::VirtualHeight()
    {
    return iStateFactory->State().VirtualHeight();
    }

// ---------------------------------------------------------
// CWmlBioControl::VirtualVisibleTop
// ---------------------------------------------------------
//
TInt CWmlBioControl::VirtualVisibleTop()
    {
    return iStateFactory->State().VirtualVisibleTop();
    }

// ---------------------------------------------------------
// CWmlBioControl::IsCursorLocation
// ---------------------------------------------------------
//
TBool CWmlBioControl::IsCursorLocation(TMsgCursorLocation aLocation) const
    {
    return iStateFactory->State().IsCursorLocation( aLocation );
    }

// ---------------------------------------------------------
// CWmlBioControl::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CWmlBioControl::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    return iStateFactory->State().OfferKeyEventL( aKeyEvent, aType );
    }

// ---------------------------------------------------------
// CWmlBioControl::GetHelpContext
// ---------------------------------------------------------
//
void CWmlBioControl::GetHelpContext(TCoeHelpContext& aHelpContext) const
    {
    const TUid KUidSmart = {0x101F4CDA};
    iStateFactory->State().GetHelpContext(aHelpContext); // adds the context of current state
    aHelpContext.iMajor = KUidSmart;
    }

// ---------------------------------------------------------
// CWmlBioControl::HandleResourceChange
// ---------------------------------------------------------
//
void CWmlBioControl::HandleResourceChange( TInt aType )
    {
    LOG("CWmlBioControl::HandleResourceChange begin");
    CMsgBioControl::HandleResourceChange( aType );
    
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iRichBio->Reset();
        }
    LOG("CWmlBioControl::HandleResourceChange end");
    }

// ---------------------------------------------------------
// CWmlBioControl::CountComponentControls
// ---------------------------------------------------------
//
TInt CWmlBioControl::CountComponentControls() const
    {
    // There can richbio or...
    if ( iRichBio )
        {
        return 1;
        }
    // No control at all.
    else
        {
        return 0;
        }
    }

// ---------------------------------------------------------
// CWmlBioControl::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CWmlBioControl::ComponentControl( TInt /*aIndex*/ ) const
    {
    // Precondition is that we have richbio control, otherwise this
    // method should not be called at all.
    __ASSERT_DEBUG( iRichBio, Panic( ERichBioNull ) );
    return iRichBio;
    }

// ---------------------------------------------------------
// CWmlBioControl::SizeChanged()
// ---------------------------------------------------------
//
void CWmlBioControl::SizeChanged()
    {
    iStateFactory->State().SizeChanged();
    }

// ---------------------------------------------------------
// CWmlBioControl::FocusChanged
// ---------------------------------------------------------
//
void CWmlBioControl::FocusChanged( TDrawNow aDrawNow )
    {
    iStateFactory->State().FocusChanged( aDrawNow );
    }

// ---------------------------------------------------------
// CWmlBioControl::SetContainerWindowL
// ---------------------------------------------------------
//
void CWmlBioControl::SetContainerWindowL( const CCoeControl& aContainer )
    {
    CCoeControl::SetContainerWindowL( aContainer );
    // Let the state create the viewer.
    iStateFactory->State().CreateViewerL( aContainer );
    }

// ---------------------------------------------------------
// CWmlBioControl::CWmlBioControl
// ---------------------------------------------------------
//
CWmlBioControl::CWmlBioControl(
    MMsgBioControlObserver& aObserver,
    CMsvSession* aSession,
    TMsvId aId,
    TMsgBioMode aIsEditor,
    const RFile* aFile):
        CMsgBioControl( aObserver,
                        aSession,
                        aId,
                        aIsEditor,
                        aFile)
    {
    // all functionality in base class
    }

// ---------------------------------------------------------
// Symbian OS default constructor can leave.
// ---------------------------------------------------------
//
void CWmlBioControl::ConstructL()
    {
    SetExtension( this );
    
   	FeatureManager::InitializeLibL();
    
    iStateFactory = CWmlStateFactory::NewL( *this );

    // Load resources
    _LIT(KWMLBCResourceFileName, "wmlbc.rsc");

    LoadResourceL( KWMLBCResourceFileName );
    LoadStandardBioResourceL();

    CArrayPtrSeg<CParsedField>* parsedFieldArray =
        ReadStoreLC( iId );

	
    iItemEngine = CWmlItemEngine::NewL();
    iItemEngine->CreateItemArraysL( *parsedFieldArray );

	
    // Array was pushed inside the TCleanupItem and all it's
    // elements are also destroyed by this call
    CleanupStack::PopAndDestroy(); // parsedFieldArray

    TInt bmCount = iItemEngine->BookmarkCount();
    
    if ( bmCount == 0 && !FeatureManager::FeatureSupported( KFeatureIdSmartmsgSMSWapApReceiving ) )
		{
		User::Leave(KErrNotSupported);
		}	

    iStateFactory->SetInitialStateL(bmCount);
    }

// ---------------------------------------------------------
// CWmlBioControl::ReadStoreLC
// ---------------------------------------------------------
//
CArrayPtrSeg<CParsedField>* CWmlBioControl::ReadStoreLC(
    const TMsvId aId )
    {
    CMsvEntry* entry = MsvSession().GetEntryL( aId );
    CleanupStack::PushL( entry );

    if ( !entry->HasStoreL() )
        {
		LOG("CWmlBioControl: No store for entry");
        User::Leave( KErrNotFound );
        }

    CMsvStore* store = entry->ReadStoreL();
    CleanupStack::PushL( store );

    RMsvReadStream readStream;
    readStream.OpenLC( *store, KUidMsvBIODataStream );

    CArrayPtrSeg<CParsedField>* parsedFields =
        InternalizeFieldDataL( readStream );

    CleanupStack::PopAndDestroy(3, entry); // readStream, store, entry

    CleanupStack::PushL( TCleanupItem(CleanupResetAndDestroy, parsedFields) );

    return parsedFields;
    }

// ---------------------------------------------------------
// CWmlBioControl::InternalizeFieldDataL
// ---------------------------------------------------------
//
CArrayPtrSeg<CParsedField>* CWmlBioControl::InternalizeFieldDataL(
    RMsvReadStream& aReadStream )
    {
    CArrayPtrSeg<CParsedField>* parsedFieldArray =
        new ( ELeave ) CArrayPtrSeg<CParsedField>(1);
    // Push array to the cleanupstack so that all of it's items
    // are destroyed when it's popped.
    CleanupStack::PushL( TCleanupItem(CleanupResetAndDestroy, parsedFieldArray) );

    CParsedField* parsedField = NULL;

    const TInt streamCount = aReadStream.ReadUint8L(); // leaves on eof

    for ( TInt i=0; i < streamCount; i++ )
        {
        parsedField = new (ELeave) CParsedField();
        CleanupStack::PushL( parsedField );
        parsedField->InternalizeL( aReadStream );
        parsedFieldArray->AppendL( parsedField ); // takes ownership
        CleanupStack::Pop( parsedField );
        }

    if ( parsedFieldArray->Count() == 0 )
        {
        User::Leave( KErrMsgBioMessageNotValid ); // no data found
        }

    CleanupStack::Pop( parsedFieldArray );
    return parsedFieldArray;
    }
    
TAny* CWmlBioControl::BioControlExtension(TInt aExtensionId)
    {
    if ( aExtensionId == KMsgBioControlScrollExtension )
        {
        return static_cast<MMsgBioControlScrollExtension*> (this);
        }
    else
        {
        return NULL;
        }
    }    
 

TInt CWmlBioControl::ExtScrollL( TInt aPixelsToScroll,
                                        TMsgScrollDirection aDirection )
    {
    return iRichBio->ScrollL(aPixelsToScroll, aDirection);
    }

void CWmlBioControl::ExtNotifyViewEvent( TMsgViewEvent aEvent, TInt aParam )
    {
    iRichBio->NotifyViewEvent( aEvent, aParam );
    }

    

//  End of File
