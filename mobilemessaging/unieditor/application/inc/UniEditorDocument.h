/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CUniDocument class definition.      
*
*/



#ifndef __UNIEDITORDOCUMENT_H
#define __UNIEDITORDOCUMENT_H

//  INCLUDES
#include <msvapi.h>                     // CMsvEntry
#include <MsgEditorDocument.h>          // CMsgEditorDocument
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <gsmuelem.h>
#include <eikenv.h>

#include "UniEditorConsts.h"
#include "UniMtmUi.h"                   //  IsUpload()
#include "UniClientMtm.h"
#include "unidatamodel.h"

// CONSTANTS

// MACROS

// DATA TYPES

enum TUniMessageType
    {
    EUniReadOnly = 0,
    EUniNewMessage,
    EUniOpenFromDraft,
    EUniReply,
    EUniForward,
    EUniSendUi
    };

// Current state where the message is.
enum TUniState
    {
    EUniSms = 1,
    EUniMms = 2
    };

//The order of the save types is significant!
//Don't mess!
enum TUniSaveType
    {
    ERegularSave = 0,        // Page change
    EClosingSave,        // User save to draft
    ESendingSave         // Before send ie. text conversion is made (paragraph sep -> linefeed)
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CUniClientMtm;
class CUniMtmUi;
class CUniEditorPlugin;
class CUniDataModel;
class CMsvMimeHeaders;
class CMsvAttachment;
class CCnvCharacterSetConverter;
class CUniEditorObjectsModelObserver;

// CLASS DECLARATION

/**
* CUniEditorDocument. The document is part of the Symbian OS application framework.
*
* @since 3.2
*/
class CUniEditorDocument : public CMsgEditorDocument
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor
        *
        * @param aApp   Reference to application instance
        */
        CUniEditorDocument( CEikApplication& aApp, CCoeEnv& aCoeEnv );

        /**
        * Factory function to create this object
        *
        * @param aApp   Reference to application instance
        */
        static CUniEditorDocument* NewL( CEikApplication& aApp,
                                         CCoeEnv& aCoeEnv );

        /**
        * Destructor.
        */
        virtual ~CUniEditorDocument();

    public:

        /**
        * MessageSize
        * Calculates message size according to TS 23.140 v5.5.0.
        * @param aSmilEstimate  if ETrue, uses estimated SMIL size (from CUniSmilModel)
        *                       if EFalse, uses SMIL file size
        * @return size in bytes
        */
        TInt MessageSize( TBool aSmilEstimate = EFalse );

        /**
        * Safely deletes atta from given message (from store & smilmodel)
        * This should be called only from Uni Objects view.
        *
        * @param aSlideNum
        * @param aObject
        * @param aAttachmentObject
        */
        void DeleteAttachmentL( TInt aSlideNum, 
                                CUniObject* aObject, 
                                TBool aAttachmentObject );

        /**
        * Set whether message is SendAs, Forward, Reply or Open from Draft.
        * Function determines it from launchflags & entry flags.
        */
        void SetMessageType();

        /**
        * Launches plugings if they are not launched yet
        */
        void LaunchPlugings();

        /**
        * Returns the overall state of the message
        */
        TUniState UniState() const;
        
        /**
        * Creates correct character converter.
        */
        void CreateCharConverterL( TUniMessageCharSetSupport aMode );

        /**
        * Count of the SD objects in the message
        */
        TInt SuperDistributableObjectCount() const;   

        /**
        * From CMsgEditorDocument 
        * Returns default folder for new messages.
        * @return ID of the Drafts folder.
        */
        TMsvId DefaultMsgFolder() const;

        /**
        * From CMsgEditorDocument
        * Returns default message service used by application
        * @return TMsvId KMsvLocalServiceIndexEntryId.
        */
        TMsvId DefaultMsgService() const;

        /**
        * From CMsgEditorDocument
        * Creates new message.
        * A new message of type KUniMtmType is created
        * in a destination folder.
        * Calls CreateNewL(TMsvId aServiceType, TMsvId aDestination,
        *                  TInt aTypeList).
        * @param aService service used for sending message 
        * @param aDestFolder folder in which message will be saved eg.
        *                    KMsvDraftEntryIdValue
        * @return New message operation.
        */
        TMsvId CreateNewL(TMsvId aService, TMsvId aDestFolder);

        /**
        * From CMsgEditorDocument
        * Called after entry is changed by BaseEditor SetEntry Function.
        * From MsgEditorDocument
        */
        void EntryChangedL();

        /**
        * From CMsgEditorDocument 
        * Checks the mtm type of the message.
        * If the mtm type is not UniMtm, does the specific msg type to uni mode conversion
        */
        TMsvId PrepareContextL( TMsvId aContext );

        /**
        * From CMsgEditorDocument 
        * In case of a reply or forward, calls DoReplyForwardL to create entry
        */
        TMsvId ChangeContextL( const TEditorParameters& aParameters );

        /**
        * From CMsgEditorDocument 
        * Called by ChangeContextL to create the plugin and do the conversion
        */
        TMsvId DoReplyForwardL( TBool aReply,
                                TMsvId aSrc,
                                TMsvId aDest,
                                TMsvPartList aParts );

        /*
        * From CMsgEditorDocument 
        * Accessor (Overrides baseeditor)
        * Returns CUniClientMtm reference correctly casted from CBaseMtm
        * @param  None
        * @return Reference to documents CUniClientMtm
        */
        CUniClientMtm& Mtm();

        /*
        * From CMsgEditorDocument 
        * Accessor (Overrides baseeditor)
        * Returns CUniMtmUi reference correctly casted from CBaseMtmUi
        * @param  None
        * @return Reference to documents CUniMtmUi
        */
        CUniMtmUi& MtmUiL();

        /**
        * From MMsgAttachmentModelObserver
        */
        RFile GetAttachmentFileL( TMsvAttachmentId aId );
    
        /**
        * Returns number of non conformant attachments at this moment.
        * The result may differ compared to the load time, if images
        * are processed and inserted images conforms now.
        * @return count of non-conformant objects
        */
        TInt UpdatedNonConformantCount() const;    

        /**
        * @return the max size of the image
        */
        TSize MaxImageSize() const;

        /**
        * @return the alternate encoding type based on input language id.
        * Turkish SMS-PREQ2265 specific
        */      
        TSmsEncoding GetLanguageSpecificAltEncodingType(TInt aLangId);
        
    public: // inlines

        /**
        * Current slide of the model where document is indexing.
        * @return Index of the current slide.
        */
        inline TInt CurrentSlide();

        /**
        * Sets current slide of the message.
        * @param aSlideNum  - Index of the slide.
        */
        inline void SetCurrentSlide( TInt aSlideNum );

        /**
        * Saved
        * Accessor
        * Queries whether message is saved.
        * @param None
        * @return ETrue,  User has already saved message (in this or some previous session)
        *                 ie InPreparation flag is not set
        *         EFalse, Message has not been saved by user yet.
        */
        inline TBool Saved() const;

        /**
        * Messagetype accessor
        *
        * @return Message type
        */
        inline TUniMessageType MessageType() const;

        /*
        * Mutator
        * Set the body modified flag.
        *
        * @param aModified
        */
        inline void SetBodyModified( TBool aModified );

        /*
        * Accessor
        * Get the body modified flag.
        *
        * @return iBodyModified flag
        */
        inline TBool BodyModified() const;

        /*
        * Mutator
        * Set the header modified flag.
        * @param aModified
        */
        inline void SetHeaderModified( TBool aModified );

        /*
        * Accessor
        * Get the header modified flag.
        *
        * @return iHeaderModified flag
        */
        inline TBool HeaderModified() const;

        /*
        * Accessor
        *
        * @return ETrue if either iBodyModified or iHeaderModified
        *         flag is ETrue.
        */
        inline TBool Modified() const;

        /*
        * Accessor
        *
        * @return creation mode
        */
        inline TInt CreationMode() const;

        /*
        * Mutator. Use this to override the current
        * creation mode in MMS Settings.
        *
        * Notice: This does not automatically update the
        *         creation mode of "MsgMediaResolver". It
        *         should be done explicitly.
        *
        * @param creation mode
        */
        inline void SetCreationMode( TInt aCreationMode );

        /*
        * Accessor
        *
        * @return ETrue if creation mode is user changeable
        */
        inline TBool CreationModeUserChangeable() const;
        
        /*
        * Accessor
        *
        * @return size of "addresses"
        */
        inline TInt AddressSize() const;

        /*
        * Mutator. 
        *
        * @param size of "addresses"
        */
        inline void SetAddressSize( TInt aAddressSize );

        /*
        * Accessor
        *
        * @return size of "subject"
        */
        inline TInt SubjectSize() const;

        /*
        * Mutator. 
        *
        * @param size of "subject"
        */
        inline void SetSubjectSize( TInt aSubjectSize );

        /*
        * Accessor
        *
        * @return maximum message size in bytes
        */
        inline TUint MaxMessageSize() const;
        
        /**
        */
        inline TInt NonConformantCount() const;
        
        /**
        * Returns previous save type
        */
        inline TUniSaveType PrevSaveType() const;        
        
        /**
        * Sets previous save type
        */
        inline void SetPrevSaveType( TUniSaveType aPrevSaveType );
        
        /**
        * Returns/sets the state of the message body (body + subject)
        */
        inline TUniState UniBodyState() const;
        inline void SetBodyUniState( TUniState aState );        

        /**
        * Returns/sets the state of the header part (recipient fields)
        */
        inline TUniState UniHeaderState() const;
        inline void SetHeaderUniState( TUniState aState );        

        /**
        * Returns pointer to message plugins
        */
        inline CUniEditorPlugin* SmsPlugin();
        inline CUniEditorPlugin* MmsPlugin();

        /**
        * Pointer to datamodel
        */
        inline CUniDataModel* DataModel();

        /**
        * Max number of MMS recipients
        */
        inline TInt MaxMmsRecipients() const;

        /**
        * Max number of SMS recipients
        */
        inline TInt MaxSmsRecipients() const;

        /**
        * Max number of SMS sub messages
        * After this number the message will be changed to MMS
        */
        inline TInt MaxSmsParts() const;

        /**
        * Abs. max number of SMS sub messages even in hardcoded mode
        */
        inline TInt AbsoluteMaxSmsParts() const;

        /**
        * Is Email Over Sms feature supported
        */
        inline TBool EmailOverSmsSupported() const;

        /**
        * Character converter functions
        */
        inline CCnvCharacterSetConverter* CharConverter();
        
        /**
        * Character set support.
        */
        inline TUniMessageCharSetSupport CharSetSupport();

        /**
        * Returns the CSP bits of the SIM card
        */
        inline TInt CSPBits() const;
        
        /**
        * Returns Eikon environment pointer.
        */
        inline CEikonEnv* EikonEnv(); 
        
        /**
        * Abs. max number of characters in SMS message. 
        * If set will overdrive maximum number of SMS sub messages.
        *
        * @return -1 if not specified. Otherwise maximum character limit.
        */
        inline TInt AbsoluteMaxSmsCharacters() const;
        
        /**
        * Set unicode character mode enabled or disabled.
        *
        * @param aEnabled Enabled if ETrue. Otherwise disabled.
        */
        inline void SetUnicodeCharacterMode( TBool aEnabled );
        
        /**
        * Return whether unicode character mode is enabled or not.
        *
        * @return ETrue if enabled. Otherwise EFalse.
        */
        inline TBool UnicodeCharacterMode() const;        
        
        /**
        * Returns the currently applicable character limits for
        * single SMS part and for concatenated SMS parts.
        *
        * @param aSinglePartLength On return contains the character limit 
        *                          for single SMS part
        * @param aConcatenatedPartLength On return contains the character limit of 
        *                                single part on multiparted SMS message.
        */
        void SmsCharacterLimits( TInt& aSinglePartLength, TInt& aConcatenatedPartLength ) const;

        /**
         * Returns current alternate encoding type
         * Turkish SMS-PREQ2265 specific
         */
        inline TSmsEncoding AlternativeEncodingType();

        /**
         * Sets the current alternate encoding type to input value
         * Turkish SMS-PREQ2265 specific
         */
        inline void SetAlternativeEncodingType(TSmsEncoding aAlternateEncoding);

        /**
         * Returns number of bytes to show warning notification when SMS size sweeps the boundary value
         * Korean operator req : 415-5434
         */
        inline TInt SmsSizeWarningBytes();
        
    private:

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();
        
        /**
        * Creates a new UniAppUi
        *
        * @return a pointer to CUniAppUi
        */
        CEikAppUi* CreateAppUiL();
        
        /**
         * From MsgEditorDocument. See MsgEditorDocument.h
         */
        CMsgAttachmentModel* CreateNewAttachmentModelL( TBool aReadOnly );

    private:  // data
        
        enum TUniDocumentFlags
            {
            EUniDocumentHeaderModified = 0x1,
            EUniDocumentBodyModified = 0x2,
            EUniDocumentCreationModeUserChangeable = 0x4,
            EUniDocumentEMailOverSms = 0x8,
            EUniDocumentRestrictedReplySms = 0x10,
            EUniDocumentRestrictedReplyMms = 0x20,
            EUniDocumentCSPBitsSupported = 0x40,
            EUniDocumentUnicodeCharacterMode = 0x80
            };
        
        CEikonEnv*          iEnvironment;
        RFs&                iFs;
        TUniMessageType     iMessageType;
        
        CUniDataModel*      iDataModel;
        CCnvCharacterSetConverter* iCharConverter;

        TInt                iCurrentSlide;
        TUint32             iMaxMessageSize;
        TInt                iAddressSize;
        TInt                iSubjectSize;
        TInt                iMaxMmsRecipients;
        TInt                iMaxSmsRecipients;
        TInt                iMaxConcatenatedSms;
        TInt                iAbsMaxConcatenatedSms;
        TInt                iCreationMode;
        TInt                iNonConfCount;
        TInt                iMaxImageWidth;
        TInt                iMaxImageHeight;
        
        TInt                iSupportedFeatures;
        TUniSaveType        iPrevSaveType;
        TUniState           iBodyState;
        TUniState           iHeaderState;
        TInt                iFlags;
        TInt                iCSPBits;
        TUniMessageCharSetSupport    iCharSetSupport;
        
        CUniEditorObjectsModelObserver* iObjectObserver; 
        
        CUniEditorPlugin*   iSmsPlugin;
        CUniEditorPlugin*   iMmsPlugin;
        
        /** Maximum amount of unicode characters that can be send with SMS. */
        TInt                iAbsMaxSmsCharacters;
        TInt                iCharSetMode;

        /** Turkish SMS-PREQ2265 specific */
        TSmsEncoding        iAlternativeEncodingType;
        
        /** Sms size limit to show warning notifications: Korean req:415-5434 */
        TInt                iSmsSizeWarningBytes;
};

#include "UniEditorDocument.inl"

#endif   // __UNIDOCUMENT_H

// End of file
