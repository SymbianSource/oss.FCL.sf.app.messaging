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
* Description:  Message App UI call back interface.
*
*/



#ifndef MMAILAPPUIINTERFACE_H
#define MMAILAPPUIINTERFACE_H

#include <badesca.h>
#include <e32const.h>

// DATA TYPES
enum TMailUiParams
	{
	EMailUseDefaultNaviPane = KBit0
	};

// FORWARD DECLARATIONS
class CCoeControl;
class MMailMsgBaseControl;
class CMsvAttachment;
class CEikStatusPane;
class CRichText;

// CLASS DECLARATION

/**
*  MMailAppUiInterface interface.
*
*  @since Series 60 3.0
*/
class MMailAppUiInterface
    {
    public:  // destructor
        /**
        * Destructor.
        */
        virtual ~MMailAppUiInterface(){};

    public: // New functions
        /**
        * AddAttachmentsL.
        * Add attachment to mail UI application.
        * @since Series 60 3.0
        * @param aAttachmentInfo attachment info.
        * @param aCanBeRemoved ETrue if attachment can be removed via
        * attachment view. 
        */
        virtual void AddAttachmentL(
        	CMsvAttachment& aAttachmentInfo,
        	TBool aCanBeRemoved = ETrue ) = 0;
        
        /**
        * AddControlL.
        * @since Series 60 3.0
        * @param aControlType controls to be created.
        * See TMailControlType.
        */
        virtual void AddControlL(TInt aControlType) = 0;

        /**
        * AddControlL.
        * @since Series 60 3.0
        * @param aControl Replaces default body control with custom control.
        * This can  be called only once. Leaves with KErrAlreadyExists if
        * body control is already replaced with custom control.
        */
        virtual void AddControlL(CCoeControl& aControl) = 0;

        /**
        * AddControlL.
        * @since Series 60 3.0
        * @param aControl Replaces default body control with custom control.
        * This can  be called only once. Leaves with KErrAlreadyExists if
        * body control is already replaced with custom control.
        */
        virtual void AddControlL(MMailMsgBaseControl& aControl) = 0;

        /**
        * ParentControl.
        * @since Series 60 3.0
        * @param aText wait note text.
        * @return message view control. << window-owning >>
        */
        virtual CCoeControl& ParentControl() = 0;
        
        /**
        * StatusPane.
        * @since Series 60 3.0
        * @return pointer to status pane or NULL.
        */        
        virtual CEikStatusPane* StatusPane() = 0;

        /**
        * StartWaitNoteL.
        * @since Series 60 3.0
        * Start Mail viewer "Opening" wait note.
        */
        virtual void StartWaitNoteL(/*const TDesC& aText*/) = 0;

        /**
        * StopWaitNote.
        * @since Series 60 3.0
        * Stop the wait note.
        */
        virtual void StopWaitNote() = 0;

        /**
        * SetTitleTextL.
        * @since Series 60 3.0
        * @param aResourceId title text resource id
        * See <avkon.rh> "STRUCT TITLE_PANE"
        */
        virtual void SetTitleTextL(TInt aResourceId) = 0;

        /**
        * SetFromFieldTextL.
        * @since Series 60 3.0
        * @param aControlType header control type.
        * @param aText Recipient array.
        * Does nothing if used control type is not created with AddControlL
        */
        virtual void SetAddressFieldTextL(TInt aControlType, CDesCArray& aText) = 0;
        
        /**
        * SetFromFieldTextL.
        * @since Series 60 3.0
        * @param aControlType header control type.
        * @param aText recipient text
        * Does nothing if used control type is not created with AddControlL
        */
        virtual void SetAddressFieldTextL(TInt aControlType, const TDesC& aText) = 0;

        /**
        * SetBodyTextL.
        * @since Series 60 3.0
        * @param aText Body text.
        */
        virtual void SetBodyTextL(CRichText& aText) = 0;

        /**
        * SetSubjectFieldTextL.
        * @since Series 60 3.0
        * @param aControlType header control type.
        * Does nothing if used control type is not created with AddControlL
        */
        virtual void SetSubjectL(const TDesC& aText) = 0;
        
        /**
        * MsgViewRect.
        * @since Series 60 3.0
        * @param aPane View part id.
        * @return reserved size for selected view.
        */
        virtual TRect MsgViewRect(TInt aPane) = 0;        
        
        /**
        * AppUiHandleCommandL.
        * @since Series 60 3.0
        * @param aCommand AppUi command ID.
        */        
        virtual void AppUiHandleCommandL(TInt aCommand) = 0;
        
        /**
        * Checks if next or previous message is available.
        * @param aForward  direction: ETrue = next, EFalse = previous.
        * @return ETrue if next message is available in desired direction.
        */
        virtual TBool IsNextMessageAvailableL( TBool aForward ) = 0;
        
        /**
        * Launch editor/viewer for next/previous message.
        * @param aForward  direction: ETrue = next, EFalse = previous.
        */
        virtual void NextMessageL( TBool aForward ) = 0;
        
        /**
         * View plugin notifies when the message is ready to be shown
         * @since S60 5.0
         */
        virtual void ViewMessageComplete() = 0;
        
    };

#endif      // MMAILAPPUIINTERFACE_H

// End of File
