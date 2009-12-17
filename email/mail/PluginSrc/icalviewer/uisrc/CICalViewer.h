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
* Description:  Message Vcal view
*
*/


#ifndef __CMAILMESSAGEICALVIEW_H
#define __CMAILMESSAGEICALVIEW_H

#include <coedef.h>
#include <w32std.h>
#include <bldvariant.hrh>
#include <MSVSTD.H>
#include <CMailMessageView.h>
#include <MMailAppUiInterface.h>
#include <MAgnEntryUi.h>
#include <CMailMessage.h>

// FORWARD DECLARATIONS
class CEikMenuPane;
class CICalEngine;
class CMailMessage;
class CMailMessageView;
class MMailMessageView;
class MMailAppUiInterface;
class CIdle;
class TMsvEmailEntry;
class CCalenImporter;
class CICalViewerView;
class CICalAttaLoader;

// CLASS DECLARATION

/**
*  Mail message ical viewer
*  ICal message viewer plug-in.
*  @since Series 60 3.0
*/
class CICalViewer : public CMailMessageView, MAgnEntryUiCallback
    {
    public:  // Constructors and destructor

        /**
        * NewL
        * Called from the ECOM plug-in framework.
        */
        static CICalViewer* NewL();

        /**
        * Destructor.
        */
        ~CICalViewer();


   private: // constructors

        void ConstructL();
        CICalViewer();

    public: // from MMailMessageView

        void ViewMessageL(
			RFileReadStream& aReadStream,
			MMailAppUiInterface& aUICallBack);
        void ViewMessageL(
			CMailMessage& aMessage,
            MMailAppUiInterface& aUICallBack,
            TUint& aParam);
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
        TBool HandleCommandL(TInt aCommand);
        TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
        const TDesC& ResourceFile();

        void MessageLoadL(TInt aState, CMailMessage& aMessage);

	private: // From MAgnEntryUiCallback		
		
		TBool IsCommandAvailable( TInt aCommandId );
		
		TInt ProcessCommandWithResultL( TInt aCommandId );

		void ProcessCommandL( TInt aCommandId );

   private:
        void CreateControlsL();
        void CreateListBoxL();
		void DoOMRViewerLaunchL();
		void DoViewMessageL( TUid aMtmUid );
		void DoGetCorrectDllDriveL( TFileName& aDriveName );
        void ParseMessageToCalEntryL(CMailMessage& aMessage);
        static TInt IdleTimerCallbackL(TAny* iCalViewer);

    private:
    	// Ref to app UI
        MMailAppUiInterface* iAppUi;

		// own: readstream for ical message
        RReadStream iStream;

        // own: idle callback to launch omr viewers
        CIdle* iIdleLauncher;

        // own: MR View control
        CICalViewerView* iICalView;
        
        // own: attachment loader
        CICalAttaLoader* iAttaLoad;

        // own: parsed Meeting Request entry
        RPointerArray<CCalEntry> iEntries;

        // own: entry ui in params for MAgnEntryUI
        MAgnEntryUi::TAgnEntryUiInParams* iEntryUiInParams;
        
        // own: entry ui out params for MAgnEntryUI
        MAgnEntryUi::TAgnEntryUiOutParams* iEntryUiOutParams;
        
        // own: resource file, placed in heap due to large size
        HBufC* iResourceFile;

        // For parameter parsing to entry ui params
        TMsvEmailEntry iMsg;

        // Calender session
        CCalSession* iSession;

        // CAgnEntryUI return value
        TInt iRetVal;

        // CalenImporter
        CCalenImporter* iImporter;
        
        // Internal status flag
        TUint32 iFlags;

    };

#endif      // CMailMessageVcalView_H

// End of File
