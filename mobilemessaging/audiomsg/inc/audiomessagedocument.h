/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Document class for AudioMessage application
*
*/




#ifndef __AUDIOMESSAGEDOCUMENT_H
#define __AUDIOMESSAGEDOCUMENT_H

#include <msvapi.h>     
#include <MsgEditorDocument.h>
#include <mmsmsventry.h>

#include "audiomessage.hrh"

class CMmsClientMtm;
class CBaseMtmUi;
class CMsgMediaResolver;

// ========== CLASS DECLARATION ============================

/**
 * document class
 *
 * @lib AudioMessage.exe
 * @since S60 v3.1
 */
class CAudioMessageDocument : public CMsgEditorDocument
    {

    public:     // New functions

       /**
        * Messagetype accessor
        *
        * @return Message type
        */
        TAmsUiMessageType MessageType() const;

        /**
         * C++ constructor
         *
         * @param aApp   Reference to application instance
         */
        CAudioMessageDocument( CEikApplication& aApp );

        /**
         * Factory function to create this object
         *
         * @param aApp   Reference to application instance
         */
        static CAudioMessageDocument* NewL( CEikApplication& aApp );

        /**
         * Destructor.
         */
        /*virtual*/ 
        ~CAudioMessageDocument();
 		
 		/**
	     * DefaultMsgService
	     * From CMsgEditorDocument
	     * Returns default message service used by application
	     * @return TMsvId KMsvLocalServiceIndexEntryId.
	     */
	    inline TMsvId DefaultMsgService() const;
	       
	    /**
	     * DefaultMsgFolder
	     * Returns default folder for new messages.
	     * @return ID of the Drafts folder.
	     */
	    inline TMsvId DefaultMsgFolder() const;
	    
	    /**
	     * CreateNewL
	     * From CMsgEditorDocument
	     * Creates new message.
	     * A new message of type Postcard is created
	     * in a destination folder.
	     * Calls CreateNewL(TMsvId aServiceType, TMsvId aDestination,
	     *                  TInt aTypeList).
	     * @param aService service used for message
	     * @param aDestFolder folder in which message will be saved eg.
	     *                    KMsvDraftEntryIdValue
	     * @return ID of mail operation.
	     */
	    TMsvId CreateNewL(TMsvId aService, TMsvId aDestFolder);
	    
	    /**
	     * EntryChangedL
	     * From CMsgEditorDocument
	     * Called after entry is changed by BaseEditor SetEntry Function.
	     * From MsgEditorDocument
	     */
	    void EntryChangedL();

	    /**
	     * Mtm
	     * Accessor (Overrides baseeditor)
	     * Returns CMmsClientMtm reference correctly casted from CBaseMtm
	     * @param  None
	     * @return Reference to documents CMmsClientMtm
	     */
	    CMmsClientMtm& Mtm();

	    /**
	     * MtmUi
	     * Accessor (Overrides baseeditor)
	     * Returns CBaseMtmUi reference
	     * @param  None
	     * @return Reference to documents CBaseMtmUi
	     */
	    CBaseMtmUi& MtmUi();

	    /**
	     * InitializeL
	     * Creates MediaResolver
	     */ 
	    TInt InitializeL();

        /**
         * Set whether message is SendAs, Forward, Reply or Open from Draft.
         * Function determines it from launchflags & entry flags.
         */
        void SetMessageType();

        /*
         * Mutator. 
         *
         * @param size of "addresses"
         */
        inline void SetAddressSize( TInt aAddressSize );
        
        /**
         * Accessor
         *
         * @return size of address
         */
        inline TInt AddressSize() const;

        /**
         * Accessor
         *
         * @return size of audio file
         */
        inline TUint AudioFileSize() const;
        /**
         * Mutator. 
         *
         * @param size of audio file
         */
       	inline void SetAudioFileSize( TUint aAudioFileSize );

   	   	/**
         * Accessor
         *
         * @return maximum message size in bytes
         */
      	inline TUint32 MaxMessageSize() const;
     	
     	/**
         * Saved
         * Queries whether message is saved.
         * @param None
         * @return ETrue,  User has already saved message (in this or some previous session)
         *                 ie InPreparation flag is not set
         *         EFalse, Message has not been saved by user yet.
         */
        inline TBool Saved() const;

        /**
         * Mutator
         * Set the body modified flag.
         *
         * @param aModified
         */
        inline void SetBodyModified( TBool aModified );

        /**
         * Accessor
         * Get the body modified flag.
         *
         * @return iBodyModified flag
         */
        inline TBool BodyModified() const;

        /**
         * Mutator
         * Set the header modified flag.
         * @param aModified
         */
        inline void SetHeaderModified( TBool aModified );

        /**
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
        
		/**
		* Sets the duration of the voice in microseconds.
		* This is for data storage only (does not affect playback).
		* @param aDuration The new duration.
		*/
        inline void SetDuration(TTimeIntervalMicroSeconds aDuration);
		/**
		 * Request duration information from the implementor
		 * @return duration in microseconds 
		 */
		inline TTimeIntervalMicroSeconds GetDuration();
   		
	    /**
	    * Get pointer to MediaResolver
	    * @return Pointer to MediaResolver
	    */
   		inline CMsgMediaResolver* MediaResolver( );

        /*
        * Mutator
        * Set the application mode : EAmsEditor/EAmsViewer.
        * @param aStatus
        */
   		inline void SetAppMode(TAmsAppStatus aStatus );
 
        /*
        * Accessor
        *
        * @return the mode of application EAmsEditor/EAmsViewer
        *         
        */
   		inline TAmsAppStatus GetAppMode();
   		
        /*
        * Mutator
        * Set the audio clip status : EAmsClipNone/EAmsClipFromGallery/EAmsClipFromRecorder.
        * @param aStatus
        */
   		inline void SetClipStatus(TAmsClipStatus aStatus );

        /*
        * Accessor
        *
        * @return the status of audio clip : EAmsClipNone/EAmsClipFromGallery/EAmsClipFromRecorder.
        *         
        */
   		inline TAmsClipStatus GetClipStatus();

        /*
        * Mutator
        * Set the audio insert status : inside, recoder, fetched, smil added.
        * @param aStatus
        */
   		inline void SetInsertedStatus(TAmsInsertedStatus aStatus );

        /*
        * Accessor
        *
        * @return the status of audio clip : EAmsNone/EAmsRecorded/EAmsFetched/
        *          							 EAmsInsideMessage/EAmsSmilAdded
        */
   		inline TAmsInsertedStatus GetInsertedStatus();
   		
   		inline TAmsRecordIconStatus GetRecordIconStatus(); 
   		
   		inline void SetRecordIconStatus ( TAmsRecordIconStatus aRecordIconStatus );
   		   
     	/**
        * Check if message is forwarded.
        *
        * @return ETrue, if is forward.
        */
        inline TBool IsForward() const;
        
        TBool IsAmrL( RFile& aFile,TDataType& aMimeType ) const;


    private:

        /**
        * 2nd phase constructor
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Creates a new AudioMessageAppUi
        *
        * @return a pointer to CAudioMessageAppUi
        */
        CEikAppUi* CreateAppUiL();

    public:  // data
         
         /**
         * Attachmentid for audio.  
		 */
        TMsvAttachmentId iAudio;

        /**
         * Attachmentid for smil.  
		 */
        TMsvAttachmentId iSmil;


    private:  // data

        /**
	     * from where message is opened
	     */
        TAmsUiMessageType iMessageType;

	    /**
	     * mediaresolver
	     * Own.
	     */
	    CMsgMediaResolver* iMediaResolver;

	    /**
	     * for recordingdlg use
	     */
	    TInt iMaxMessageSize;
	    
	    /**
	     * for to-control
	     */
	    TInt iAddressSize;

	    /**
	     * amr file size
	     */
	    TUint iAudioFileSize;

	    /**
	     * dirty bit for recipients
	     */
	    TBool iHeaderModified;

	    /**
	     * dirty bit for voicefile
	     */
	    TBool iBodyModified;

   	    /**
	     * amr duration ms
	     */
		TTimeIntervalMicroSeconds iDuration;

   	    /**
	     * editor or viewer
	     */
		TAmsAppStatus iAppStatus;

   	    /**
	     * callery, recoder, attachment
	     */
		TAmsClipStatus iClipStatus;

   	    /**
	     * none, recoder,fetched, smiladded, inside
	     */
		TAmsInsertedStatus iInsertedStatus;

		TAmsRecordIconStatus iRecordIconStatus;
	};

#include "audiomessagedocument.inl"

#endif      // __AUDIOMESSAGEDOCUMENT_H

