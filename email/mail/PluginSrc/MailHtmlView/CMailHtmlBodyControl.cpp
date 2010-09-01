/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HTML body control
*
*/



// INCLUDE FILES
#include    "CMailHtmlBodyControl.h"
#include 	"MailLog.h"
#include 	<MsgBaseControl.h>
#include	<MsgEditor.hrh>
#include 	<CMailMessage.h>
// Browser
#include 	<BrCtlInterface.h>
#include 	<BrCtlDefs.h>
#include    <TXTRICH.H> 
#include    <MMailAppUiInterface.h>

// CONSTANTS
//const ?type ?constant_var = ?constant;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::CMailHtmlBodyControl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMailHtmlBodyControl::CMailHtmlBodyControl( 
	CMailMessage& aMessage,
	MMailAppUiInterface& aUICallBack ): 
	iMessage( aMessage ), iUICallBack( aUICallBack )
    {
    }

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::ConstructL()
    {   
    CCoeControl& appui = iUICallBack.ParentControl();
    TRect base = iUICallBack.MsgViewRect(0);
	LOG2("CMailHtmlBodyControl::ConstructL W:%d H:%d", 
	base.Width(), base.Height() );


    TUint capabilities(TBrCtlDefs::ECapabilityClientResolveEmbeddedURL);
    capabilities |= TBrCtlDefs::ECapabilityFindItem;
    capabilities |= TBrCtlDefs::ECapabilityDisableInputAndPlugins;
    
    iBrControl = CreateBrowserControlL(
    	&appui, 
    	base, capabilities, 
    	TBrCtlDefs::ECommandIdBase, 
    	NULL, this, NULL, NULL, NULL, NULL);
    iBrControl->SetBrowserSettingL(TBrCtlDefs::ESettingsSmallScreen, ETrue);
    iBrControl->SetBrowserSettingL(TBrCtlDefs::ESettingsEmbedded, ETrue);
    
/*
    iBrControl->SetBrowserSettingL(TBrCtlDefs::ESettingsSmallScreen, 1);
    //iBrControl->SetBrowserSettingL(TBrCtlDefs::ESettingSecurityWarnings, 1);
    iBrControl->SetBrowserSettingL(TBrCtlDefs::ESettingsAutoLoadImages, 1);
    //iBrControl->SetContainerWindowL(appui.Window());
    iBrControl = CreateBrowserControlL(CCoeControl* aParent, TRect aRect,
    TUint aBrCtlCapabilities, TUint aCommandIdBase = TBrCtlDefs::ECommandIdBase,
    MBrCtlSoftkeysObserver* aBrCtlSoftkeysObserver = NULL,
    MBrCtlLinkResolver* aBrCtlLinkResolver = NULL,
    MBrCtlSpecialLoadObserver* aBrCtlSpecialLoadObserver = NULL,
    MBrCtlLayoutObserver* aBrCtlLayoutObserver = NULL,
    MBrCtlDialogsProvider* aBrCtlDialogsProvider = NULL);    */    
    }

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMailHtmlBodyControl* CMailHtmlBodyControl::NewLC( 
	CMailMessage& aMessage ,MMailAppUiInterface& aUICallBack )
    {
    CMailHtmlBodyControl* self = new( ELeave ) CMailHtmlBodyControl( 
    	aMessage, aUICallBack );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

    
// Destructor
CMailHtmlBodyControl::~CMailHtmlBodyControl()
    {
    delete iBrControl;
    }


// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::BrowserControl
// -----------------------------------------------------------------------------
//
CBrCtlInterface& CMailHtmlBodyControl::BrowserControl()
    {
    return *iBrControl;
    }
    
// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::MessageLoadingL
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::MessageLoadingL(TInt aStatus, CMailMessage& aMessage)
    {
	if( aStatus == CMailMessage::ELinkedFileReady ) 
		{
        LOG1( "CMailHtmlBodyControl::ELinkedFileReady 0x%08x", 
	        aMessage.LinkedHtmlId());		
		RFile linkedHtml = aMessage.LinkedHtmlContent();
		if ( linkedHtml.SubSessionHandle() != 0 )
			{			
			CleanupClosePushL( linkedHtml );
			TInt fileSize(0);
			linkedHtml.Size( fileSize );
			HBufC8* data = HBufC8::NewLC( fileSize );
			TPtr8 ptr( data->Des() );
			User::LeaveIfError( linkedHtml.Read( ptr ) );
			_LIT(KContent, "image/gif");
			iEmbeddedLinkContent->HandleResolveComplete(
			    KContent,
                KNullDesC(),
                data );
	        CleanupStack::PopAndDestroy(2);		// CSI: 47 # data, linkedHtml
			}
		else
		    {		    
		    iEmbeddedLinkContent->HandleResolveError( KErrCorrupt );
		    }
		}		    
    }
    
// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::CurrentLineRect
// -----------------------------------------------------------------------------
//
TRect CMailHtmlBodyControl::CurrentLineRect()
	{
	TRect rect = iBrControl->Rect();
	LOG2("CMailHtmlBodyControl::CurrentLineRect W:%d H:%d", 
		rect.Width(), rect.Height() );	

	return rect;
	}
	
// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::SetAndGetSizeL
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::SetAndGetSizeL( TSize& aSize )
	{
	TSize htmlSize = iBrControl->ContentSize();
	TSize brc = iBrControl->Size();
	iBrControl->SetSize( aSize );
	htmlSize = iBrControl->ContentSize();	
	}

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::ClipboardL
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::ClipboardL( TMsgClipboardFunc aFunc )
	{
	LOG1("CMailHtmlBodyControl::ClipboardL %d", aFunc );
	}
	
// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::EditL
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::EditL( TMsgEditFunc aFunc )
	{
	LOG1("CMailHtmlBodyControl::EditL %d", aFunc );	
	}
// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::IsFocusChangePossible
// -----------------------------------------------------------------------------
//
TBool CMailHtmlBodyControl::IsFocusChangePossible( 
	TMsgFocusDirection aDirection )  const
	{
	LOG1("CMailHtmlBodyControl::IsFocusChangePossible %d", aDirection );
	return aDirection == EMsgFocusDown ? EFalse : ETrue;
	}
// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::IsCursorLocation
// -----------------------------------------------------------------------------
//
TBool CMailHtmlBodyControl::IsCursorLocation( 
	TMsgCursorLocation aLocation ) const
	{
	LOG1("CMailHtmlBodyControl::IsCursorLocation %d", aLocation );
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::EditPermission
// -----------------------------------------------------------------------------
//
TUint32 CMailHtmlBodyControl::EditPermission() const
	{
	return 0;
	}

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::PrepareForReadOnly
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::PrepareForReadOnly( TBool /*aReadOnly*/ )
	{
	
	}


// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::VirtualHeight
// Returns approximate height of the control.
// -----------------------------------------------------------------------------
//
TInt CMailHtmlBodyControl::VirtualHeight()
    {
	LOG1("CMailHtmlBodyControl::VirtualHeight H:%d", 
	    iBrControl->Size().iHeight);
    TSize htmlSize = iBrControl->ContentSize();
	LOG1("CMailHtmlBodyControl::VirtualHeight brContent h:%d", 
	    htmlSize.iHeight);    
    return htmlSize.iHeight;
    }

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::VirtualVisibleTop
// Returns a topmost visible text position.
// -----------------------------------------------------------------------------
//
TInt CMailHtmlBodyControl::VirtualVisibleTop()
    {
    LOG("CMailHtmlBodyControl::VirtualVisibleTop");   
    return iBrControl->Rect().iTl.iY; // left corner
    }

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::HandleBrowserLoadEventL
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::HandleBrowserLoadEventL(
        	TBrCtlDefs::TBrCtlLoadEvent aLoadEvent, 
        	TUint /*aSize*/, 
        	TUint16 /*aTransactionId*/)
	{
	if (aLoadEvent == TBrCtlDefs::EEventNewUrlContentArrived)
		{
		//
		}
	}
	
// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::ResolveEmbeddedLinkL
// -----------------------------------------------------------------------------
//	
TBool CMailHtmlBodyControl::ResolveEmbeddedLinkL(const TDesC& aEmbeddedUrl,
    const TDesC& aCurrentUrl,
    TBrCtlLoadContentType aLoadContentType, 
    MBrCtlLinkContent& aEmbeddedLinkContent)
	{
	TBool resolve(EFalse);
	LOG2("CMailHtmlBodyControl::ResolveEmbeddedLinkL eURL:%S, cURL:%S",
		&aEmbeddedUrl , &aCurrentUrl);
	// Resolve local image link
	if (aLoadContentType==ELoadContentTypeImage)
		{
		iEmbeddedLinkContent = &aEmbeddedLinkContent;
		iMessage.LoadLinkedHtmlContentL(aCurrentUrl, aEmbeddedUrl, *this);
		resolve = ETrue;
		}
    return resolve;
	}
// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::ResolveLinkL
// -----------------------------------------------------------------------------
//        	
TBool CMailHtmlBodyControl::ResolveLinkL(const TDesC& aUrl, 
    const TDesC& aCurrentUrl,
    MBrCtlLinkContent& /*aBrCtlLinkContent*/)
	{
	LOG2("CMailHtmlBodyControl::ResolveLinkL aURL:%S, cURL:%S",
		&aUrl , &aCurrentUrl);
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::CancelAll
// -----------------------------------------------------------------------------
//            
void CMailHtmlBodyControl::CancelAll()
	{
	// Cancel content loadin
	LOG("CMailHtmlBodyControl::CancelAll");	
	iMessage.Cancel();
	}
	
	
// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::CountPluginComponentControls
// -----------------------------------------------------------------------------
//
TInt CMailHtmlBodyControl::CountPluginComponentControls() const
    {
    return iBrControl ? 1 : 0; // iBrControl
    }

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::PluginComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CMailHtmlBodyControl::PluginComponentControl(TInt /*aIndex*/) const
    {
    return iBrControl;
    }

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::PluginSizeChanged
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::PluginSizeChanged()
    {
    LOG("CMailHtmlBodyControl::PluginSizeChanged");
    
   for (TInt i=0; i<4; ++i)								// CSI: 47 # <insert a comment here>
    	{
	    TRect base = iUICallBack.MsgViewRect(i);
		LOG2("CMailHtmlBodyControl::SizeChanged-EB- W:%d H:%d", 
			base.Width(), base.Height() );
    	}
    TPoint move(10,10);									// CSI: 47 # <insert a comment here>
    iBrControl->SetPosition( move );
/*	LOG2("CMailHtmlBodyControl::SizeChanged W:%d H:%d", 
		Size().iWidth, Size().iHeight);
	iBrControl->SetSize( Size() );
	
	LOG2("CMailHtmlBodyControl::SizeChanged - Position X:%d Y:%d", 
		Position().iX, Position().iY);
	iBrControl->SetSize( Size() );	
	
	LOG2("CMailHtmlBodyControl::SizeChanged - BRC Position X:%d Y:%d", 
		iBrControl->Position().iX, iBrControl->Position().iY);
	iBrControl->SetPosition( Position() );		

    for (TInt i=0; i<4; ++i)
    	{
	    TRect base = iUICallBack.MsgViewRect(i);
		LOG2("CMailHtmlBodyControl::SizeChanged-EB- W:%d H:%d", 
			base.Width(), base.Height() );
    	}*/
    }

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::PluginFocusChanged
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::PluginFocusChanged( TDrawNow /*aDrawNow*/ )
    {
    LOG("CMailHtmlBodyControl::PluginFocusChanged");
	//iBrControl->SetFocus( IsFocused(), aDrawNow );
    }



// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::PluginHandleResourceChange
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::PluginHandleResourceChange(TInt aType)
    {
    LOG("CMailHtmlBodyControl::PluginHandleResourceChange");
    iBrControl->HandleResourceChange( aType );
    }

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::PluginOfferKeyEventL
// -----------------------------------------------------------------------------
//    
TKeyResponse CMailHtmlBodyControl::PluginOfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    LOG("CMailHtmlBodyControl::PluginOfferKeyEventL");
    return iBrControl->OfferKeyEventL( aKeyEvent, aType );
    }

// -----------------------------------------------------------------------------
// CMailHtmlBodyControl::SetPluginExtent
// -----------------------------------------------------------------------------
//
void CMailHtmlBodyControl::SetPluginExtent(
    const TPoint& aPosition,const TSize& aSize)
	{
	LOG2("CMailHtmlBodyControl::SetPosition X:%d Y:%d", 
		aPosition.iX, aPosition.iY);
	iBrControl->SetExtent( aPosition, aSize );		
	}

//  End of File  

