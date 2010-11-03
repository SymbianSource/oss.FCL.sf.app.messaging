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
* Description:  html control
*
*/



#ifndef CMAILHTMLBODYCONTROL
#define CMAILHTMLBODYCONTROL

//  INCLUDES
#include "MMailMsgBaseControl.h"
#include <MMessageLoadObserver.h>
#include <BrCtlInterface.h> // MBrCtlLoadEventObserver
#include <BrCtlLinkResolver.h>

// FORWARD DECLARATIONS
class CMailMessage;
class MMailAppUiInterface;
class CBrCtlInterface;

// CLASS DECLARATION

/**
*  HTML body control.
*  For replacing editor base default plain text body control
*
*  @lib MailHtmlView
*  @since Series 60 3.1
*/
class CMailHtmlBodyControl : public CBase, public MMailMsgBaseControl,
	MBrCtlLoadEventObserver, MBrCtlLinkResolver, MMessageLoadObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aMessage Mail message
        * @param aUICallBack UI call back
        */
        static CMailHtmlBodyControl* NewLC( 
        	CMailMessage& aMessage, 
        	MMailAppUiInterface& aUICallBack );
        
        /**
        * Destructor.
        */
        virtual ~CMailHtmlBodyControl();

    public: // New functions
        
        /**
        * BrowserControl.
        * @since Series 60 3.0
        * @return Browser control
        */
        CBrCtlInterface& BrowserControl();

    protected:  // Functions from base classes
    		// From: MMessageLoadObserver
		void MessageLoadingL(TInt aStatus, CMailMessage& aMessage);
        
        /**
        * From MMailMsgBaseControl.
        */
        TRect CurrentLineRect();
		void SetAndGetSizeL( TSize& aSize );
        void ClipboardL( TMsgClipboardFunc aFunc );
        void EditL( TMsgEditFunc aFunc );
        TBool IsFocusChangePossible( TMsgFocusDirection aDirection )  const;
        TBool IsCursorLocation( TMsgCursorLocation aLocation ) const;
        TUint32 EditPermission()  const;
        void PrepareForReadOnly( TBool aReadOnly );
        TInt VirtualHeight();
        TInt VirtualVisibleTop();

		TInt CountPluginComponentControls() const;
		CCoeControl* PluginComponentControl( TInt aIndex ) const;
		void PluginSizeChanged();
		void PluginFocusChanged( TDrawNow aDrawNow );
		void PluginHandleResourceChange(TInt aType);
        TKeyResponse PluginOfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType);
        void SetPluginExtent(const TPoint& aPosition,const TSize& aSize);
	        
		/**
		* From MBrCtlLoadEventObserver
		*/
        void HandleBrowserLoadEventL(
        	TBrCtlDefs::TBrCtlLoadEvent aLoadEvent, 
        	TUint aSize, 
        	TUint16 aTransactionId);
        	
		/**
		* From MBrCtlLinkResolver
		*/        	
        TBool ResolveEmbeddedLinkL(
        	const TDesC& aEmbeddedUrl,
    		const TDesC& aCurrentUrl,
        	TBrCtlLoadContentType aLoadContentType, 
        	MBrCtlLinkContent& aEmbeddedLinkContent );			
    	TBool ResolveLinkL(
    		const TDesC& aUrl, 
    		const TDesC& aCurrentUrl,
            MBrCtlLinkContent& aBrCtlLinkContent );
        void CancelAll();        
        
    private:

        /**
        * constructor.
        */
        CMailHtmlBodyControl( 
        	CMailMessage& aMessage, 
        	MMailAppUiInterface& aUICallBack );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        /// Own: Browser control
        CBrCtlInterface* iBrControl;
        
        /// Ref: Link content
        MBrCtlLinkContent* iEmbeddedLinkContent;
        
        /// Ref: To viewed message
        CMailMessage& iMessage;
        
        /// Ref: UI call back
        MMailAppUiInterface& iUICallBack;
        // Reserved pointer for future extension
        //TAny* iReserved;

    };

#endif      // CMAILHTMLBODYCONTROL   
            
// End of File
