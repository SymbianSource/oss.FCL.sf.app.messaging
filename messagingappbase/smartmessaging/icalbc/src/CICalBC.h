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
*     Bio control for iCalendar messages.
*
*/



#ifndef CICALBC_H
#define CICALBC_H

// INCLUDES
#include <coecobs.h>          // for MCoeControlObserver
#include <eikbctrl.h>         // for CEikBorderedControl
#include <msgbiocontrol.h>    // for CMsgBioControl
#include <MMsgBioControlExtension.h> // for MMsgBioControlExtension

#include <MMailAppUiInterface.h>
#include <CMailMessageView.h>
#include <MMailMessageView.h>

// FORWARD DECLARATIONS
class CEikStatusPane;
class CMsvAttachment;
class CMsgBaseControl;


// CLASS DECLARATION

/**
 * Bio control for Calendar messages.
 */
class CICalendarBioControl: public CMsgBioControl,
                            public MMailAppUiInterface
    {
    public:

        /**
         * Two-phased constructor
         * @param aObserver Reference to the Bio control observer.
         * @param aSession Reference to Message Server session.
         * @param aId Id of the message.
         * @param aEditorOrViewerMode Flags the new Bio control as editor or viewer.
         * @param aFile filehandle.
         * @return The newly created object.
         */
        IMPORT_C static CMsgBioControl* NewL(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile );

         /// Destructor
        ~CICalendarBioControl();

    public: //from MMsgBioControl

        void SetAndGetSizeL(TSize& aSize);
        void SetMenuCommandSetL(CEikMenuPane& aMenuPane);
        TBool HandleBioCommandL(TInt aCommand);
        TRect CurrentLineRect() const;
        TBool IsFocusChangePossible(TMsgFocusDirection aDirection) const;
        HBufC* HeaderTextL(void) const;
        TInt VirtualHeight();
        TInt VirtualVisibleTop();
        TBool IsCursorLocation(TMsgCursorLocation aLocation) const;

    public: // from MMailMessageInterface
		void AddAttachmentL( CMsvAttachment& aAttachmentInfo,
        	                 TBool aCanBeRemoved );
        void AddControlL(CCoeControl& aControl);
        //void AddControlL(CMsgBaseControl& aControl);
        void AddControlL(TInt aControlType);
        void AddControlL(MMailMsgBaseControl& aControl);
        CCoeControl& ParentControl();
        CEikStatusPane* StatusPane();
        void StartWaitNoteL(/*const TDesC& aText*/);
        void StopWaitNote();
        void SetTitleTextL(TInt aResourceId);
        void SetAddressFieldTextL(TInt aControlType, CDesCArray& aText);
        void SetAddressFieldTextL(TInt aControlType, TDesC& aText);
        void SetBodyTextL(CRichText& aText);
        void SetSubjectL(const TDesC& aText);
        TRect MsgViewRect(TInt aPane);
        void AppUiHandleCommandL(TInt aCommand);
        TBool IsNextMessageAvailableL( TBool aForward );
		void NextMessageL( TBool aForward );

    protected: // from CCoeControl

        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void SizeChanged();
        void FocusChanged(TDrawNow aDrawNow);
        void SetContainerWindowL(const CCoeControl& aContainer);
        TKeyResponse OfferKeyEventL(
            const TKeyEvent& aKeyEvent,
            TEventCode aType);

    private: //new functions

		/**
		* Opens a read stream. Puts a stream cleanup (closing) item on the
		* cleanup stack.
		* @param aStream The stream that is to be opened.
		* @param aFile File handle from which the stream is to be opened.
		*/
		void OpenStreamLC(RFileReadStream& aStream,
    		TFileName& aFileName);

        /**
         * This is needed because the menuPane adding is done in a different
         * way in BVA than in SMS Editor/Viewer.
         * @param aMenuPane Reference to the menu pane.
         * @param aStringRes String resource ID of the command text.
         * @param aCommandOffset Offset in the Options list.
         */
        void FileBasedAddMenuItemL(CEikMenuPane& aMenuPane,
            TInt aStringRes, TInt aCommandOffset);

    private: //construction

        /**
         * The constructor.
         * @param aObserver MMsgBioControlObserver
         * @param aSession CMsvSession, the Message Server session.
         * @param aId Id of the message in Message Server.
         * @param aEditorOrViewerMode Flags the control as being either editor or viewer
         * @param aFile filehandle.
         */
        CICalendarBioControl(
            MMsgBioControlObserver& aObserver,
            CMsvSession* aSession,
            TMsvId aId,
            TMsgBioMode aEditorOrViewerMode,
            const RFile* aFile );

        /// Second phase constructor.
        void ConstructL();

    private:

        TBool iIsFileBased;
        /// The viewer control
        CCoeControl* iViewer;
        CCoeControl* iParentControl;
        MMailMessageView* iICalContainer;
        RFileReadStream iVCalStream;
        TDesC* iResourceFile;
        TInt iTitleResourceID;

    };


#endif // CICALBC_H

// End of file
