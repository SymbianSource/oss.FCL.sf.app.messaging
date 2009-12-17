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
* Description:  Common base class for Mail application UI classes.
*
*/


#ifndef MSGMAILAPPUI_H
#define MSGMAILAPPUI_H

//  INCLUDES
#include "MailLog.h"
#include <MsgEditorAppUi.h>
#include <MsgEditor.hrh>

// FORWARD DECLARATIONS
class CMsgExpandableControl;
class CMsgAddressControl;
class CMsgAttachmentControl;
class CMsgRecipientItem;
class CMsgBodyControl;
class CMsgMailDocument;
class TMsvEmailEntry;

// CLASS DECLARATION

/**
*  A common base class for Mail editor and viewer applications' UIs.
*/
class CMsgMailAppUi :public CMsgEditorAppUi
    {
    public: // iCommonFlags
    	enum TCommonFlags
            {
            EWestern = KBit0,
            EHelpSupported = KBit1
            };
    public:  // Constructors and destructor
                
        /**
        * C++ default constructor.
        */
        IMPORT_C CMsgMailAppUi();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CMsgMailAppUi();

    public: // New functions
        
        /**
        * Saves a message.
        * @param  aInPreparation in certain cases new message 
        *         has to saved temporarily, then aInPreparation flag is set 
        *         to ETrue (default is EEalse) so automatic cleanup is made 
        *         by server if something goes wrong.
        * @param  aReply not used!. See CMsgMailDocument::CreateReplyL 
        *               Defaults to EFalse.
        * @return EFalse, if there are mailboxes defined, ETrue otherwise
        */
        IMPORT_C TBool MsgSaveL(const TBool aInPreparation, 
            const TBool aReply = EFalse);

        /**
        * Checks if critical level is reached when aSize is reserved
        * @param aSize amount of kilobytes to be reserved
        * @return ETrue if critical level is not reached
        */
        IMPORT_C TBool CheckFFSL(const TInt aSize);

        /**
        * *Deprecated*.
        * Leaves with KErrNotsupported. 
        * Use MailUtils::IsClosedFileL instead of this.
        * @param aBuf MIME type
        * @return ETrue if sending is not allowed.
        */
        IMPORT_C TBool IsBannedMimeL(const TDesC& aBuf) const;

		/**
		* Checks an EMail address.
		* @param Address
		* @return ETrue if EMail address is valid.
		*/
		IMPORT_C TBool IsValidAddress( const TDesC& aAddress ) const;
		
		/**
		* inline
		* @return reference to application coe enviroment
		*/		
		CCoeEnv& AppCoeEnv();

        /**
         * Enable or disable Dialer functionality.
         * @param aEnable ETrue  : Dialer enabled (send key opens Dialer)
         *                EFalse : Dialer disabled
         */
        IMPORT_C void SetDialerEnabled( TBool aEnable );
        
    protected:  // New functions
        
        /**
        * By default Symbian OS constructor is private.
        */
        IMPORT_C void ConstructL();

        /**
        * Returns a pointer to subject control UI object.
        * @return A pointer to a control.
        */
        CMsgExpandableControl* SubjectControl() const;

        /**
        * Returns a pointer to address control UI object.
        * @param aId id of the control.
        * @return A pointer to a control.
        */
        CMsgAddressControl* AddressControl(const TInt aId) const;

        /**
        * inline
        * Returns a pointer to attachment control UI object.
        * @return A pointer to a control. Ownership not transferred.
        */
        CMsgAttachmentControl* AttachmentControl() const;

        /**
        * Returns a pointer to body control UI object.
        * @return A pointer to a control.
        */
        CMsgBodyControl* BodyControl() const;

        /**
        * *Deprecated*.
        * Leaves with KErrNotsupported. 
        * Use MailUtils::IsClosedFileL instead of this.
        * @param aMimeType MIME type to be checked
        * @return ETrue if banned otherwise EFalse
        */
        IMPORT_C TBool CheckIfBannedL(const TDesC& aMimeType);
        
        /**
        * *Deprecated*.
        * Leaves with KErrNotsupported. 
        * Use MailUtils::IsClosedFileL instead of this.
        * @param aFile file to be checked
        * @return ETrue if banned otherwise EFalse
        */        
        IMPORT_C TBool CheckIfBannedL(RFile& aFile);

        /**
        * Cleans up wait note wrapper
        * @param aAny a pointer to wrapper.
        */
        IMPORT_C static void CleanupWaitNoteWrapper(TAny* aAny);
        
        /**
        * Removes wait note from screen, used as a cleanup method
        * @param aAny a pointer to dialog.
        */
        IMPORT_C static void CleanupWaitDialog(TAny* aAny);
        
        /**
        * Calculates message size.
        * Size is based to content of the UI components.
        * @param aCountAttachments ETrue if size of the attachments
        *   need to be count also. 
        * @return Size in bytes.
        */        
        IMPORT_C TInt MessageSizeL( TBool aCountAttachments );

    private: // implementation
        CMsgMailDocument* MailDocument();
        TBool DoSaveMessageL( TBool aInPreparation, TBool aReply );
        HBufC* ConstructAddressStringLC(CMsgRecipientItem* aRecItem);
        TBool StripIllegalCharsL(TDes& aString);
        void CheckStoreSpaceL( TMsvEmailEntry aMessage, TBool aReply );    
		TBool SetServiceIdL( 
            TMsvEmailEntry& aEntry, CMsgMailDocument& aDocument );
        void SetEntryTimeL( TMsvEntry& aEntry );
        void SetEntryFlagsL( 
            TMsvEmailEntry& aEntry, 
            TMsvId aOrigMessageId );
        void SetHeaderFieldsL( TMsvEmailEntry& aMessage );
        void SetSmallIconL();    
		void AppendRecipientsL( 
            CMsgAddressControl* aField, CDesCArray& aArray );
		
		/**
		 * Construct message details from recipient array.
		 * @param aControl Source for recipients.
		 * @return Constructed details string. Empty string is returned if no
		 *         recipients are found.
		 */
		HBufC* MakeDetailsLC( CMsgAddressControl& aControl );
		
    protected:  // Data            
        /// Own: Feature flags for editor / viewer
        TUint32 iCommonFlags;
                        
        /// Message ID
        TMsvId iMessageID;        

    private:    // Data
        
        /// Own: Array containing banned characters in user inputted addresses.
        CDesCArrayFlat* iBannedChars;

        /// Own: Is refresh needed in address control after deleting
        TBool iNeedRefresh;
        
        RConeResourceLoader iResource;
        
    UNIT_TEST( T_UT_Mail )
    };

#include "MsgMailAppUi.inl"

#endif      // MSGMAILAPPUI_H   
            
// End of File
