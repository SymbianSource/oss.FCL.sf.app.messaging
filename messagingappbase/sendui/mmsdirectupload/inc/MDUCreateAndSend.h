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
* Description:   Creates and sends as many MMS messages as there are files
*                to be send. Oversize JPEG images are compressed to fit into
*                message.
*
*/




#ifndef MDUCREATEANDSEND_H
#define MDUCREATEANDSEND_H

//  INCLUDES
#include    <e32base.h>
#include    <badesca.h>
#include    <AknProgressDialog.h>
#include    <apmstd.h>
#include    <sendas2.h>
#include    <txtmrtsr.h>

#include    <UniImageProcessor.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES
class TMsvSessionEvent;
class CSendUiDataUtils;
class CSendUiAttachment;
class CSendUiSingleton;

// CLASS DECLARATION
/**
*  Callback mixin class used by CMDUCreateAndSend
*  @lib MmsDirectUpload
*  @since Series 60 2.8
*/
class MMDUCreateAndSendCallback
    {
    public:
    
        /**
        * Callback function called when message creation is completed.
        * If an error occured, it can be queried with
        * CMDUCreateAndSend::Error() function.
        * @since Series 60 2.8
        * @return None.
        */
        virtual void MessagesReady() = 0;
    };

/**
*  Creates and sends as many MMS messages as there are files to be send.
*  Oversize JPEG images are compressed to fit into message.
*
*  @lib MmsDirectUpload
*  @since Series 60 2.8
*/
class CMDUCreateAndSend : public CActive,
                          public MProgressDialogCallback,
                          public MUniImageProcessorCallback

    {

    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        * @param aCallback Callback implementation.
        */
        static CMDUCreateAndSend* NewL( 
            CCoeEnv& aCoeEnv,
            CSendUiSingleton& aSingleton,
            MMDUCreateAndSendCallback* aCallback );

        /**
        * Two-phased constructor.
        * @param aCallback Callback implementation.
        */
        static CMDUCreateAndSend* NewLC(
            CCoeEnv& aCoeEnv,
            CSendUiSingleton& aSingleton,
            MMDUCreateAndSendCallback* aCallback );

        /**
        * Destructor.
        */
        virtual~CMDUCreateAndSend();

    public: // New functions

        /**
        * Creates and sends as many MMS messages as there are files
        * to be send. Oversize JPEG images are compressed to fit into
        * message. Asynchronous operation.
        *
        * @since Series 60 2.8
        * @param aAttachments, Files to be send.
        * @param aToAddress, Recipient address.
        * @param aToAlias, Alias for recipient.
        * @param aMaxMsgSize, Maximum size of the created MMS.
        * @return None.
        */
        void CreateAndSendL(
            CArrayPtrFlat<CSendUiAttachment>* aAttachments,
            const TDesC& aToAddress,
            const TDesC& aToAlias,
            TInt aMaxMsgSize );
        
        /**
        * Error.
        * @since Series 60 2.8
        * @return Symbian wide error code.
        */
        inline TInt Error() const;
        
        /**
        * Can be used to query compression results.
        * Oversized JPEG images are compressed to fit in MMS.
        * @since Series 60 2.8
        *
        * @param aCompressionOk, Return value contains amount of succesfully
        *        compressed JPEG images.
        * @param aCompressionFail, Return value contains amount of failed
        *        compression attempts.              
        * @return None.
        */
        inline void CompressionResults(
            TInt& aCompressionOk,
            TInt& aCompressionFail ) const;
            
    private: // Constructor

        /**
        * C++ default constructor.
        */
        CMDUCreateAndSend( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton, MMDUCreateAndSendCallback* aCallback );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:  // Functions from base classes

        /**
        * From CActive
        * @since Series 60 2.8
        * @return None.
        */ 
        void RunL();
        
        /**
        * From CActive
        * @since Series 60 2.8
        * @return None.
        */        
        void DoCancel();
    
        /**
        * From CActive
        * Handles a leave occurring in the request completion
        * event handler RunL().
        * @since Series 60 2.8
        * @param aError Leave code.
        * @return 
        */   
        TInt RunError( TInt aError );

        /**
        * From MProgressDialogCallback
        * Callback method
        * Get's called when a dialog is dismissed.
        * @since Series 60 2.8
        * @return None.
        */
        void DialogDismissedL( TInt aButtonId );
        
        /**
        * From MUniImageProcessorCallback.
        * Callback to tell that compression is ready.
        * @since 3.2
        * @param aBitmapSize Dimensions of result bitmap - not interested.
        * @param aFileSize   Size of the compressed image
        * @param aCompressed ETrue if compressed, EFalse if only scaled - not interested.
        * @return None.
        */
        void ImageProcessingReady( TSize aBitmapSize, TInt aFileSize, TBool aCompressed );

    private:  // New functions

        /**
        * Starts message entry creation.
        * @since Series 60 3.0
        * @return None.
        */
        void CreateMessageAndSetDataL();

        /**
        * Checks if file is oversized JPEG image.
        * @since Series 60 2.8
        * @return ETrue if image should be compress, otherwise EFalse.
        */
        TBool IsCompressionNeededL();

        /**
        * Compress oversized JPEG image.
        * @since Series 60 2.8
        * @return None.
        */
        void CompressImageL();

        /**
        * Add attachment to message.
        * @since Series 60 2.8
        * @return None.
        */
        void AddAttachmentL();

        /**
        * Sends message.
        * @since Series 60 2.8
        * @return None.
        */
        void SendMessage();

        /**
        * Deletes temporary compressed image file if needed and 
        * updates progress bar.
        * @since Series 60 2.8
        * @return None.
        */
        void FinalizeL();
            
        /**
        * Creates temp file to ram drive d:\. Original filename is used.
        * Note: handle to created file is left open.
        * @since Series 60 3.0
        * @param aSourceFile File handle to the original file.
        * @param aTargetFile Return value contains the target file handle.
        * @return None.
        */
        void CreateTempFileL(
            RFile& aSourceFile,
            RFile& aTargetFile );

        /**
        * Shows progress bar to user. Progress bar indicates the total amount
        * of messages to be created and the current situation.
        * @since Series 60 2.8
        * @param aFinalValue Final value for the progress bar.
        * @return None.
        */
        void CreateProgressNoteL( TInt aFinalValue );

        /**
        * Updates the progress bar text.
        * @since Series 60 2.8
        * @param aFinalValue Final value for the progress bar.
        * @return None.
        */
        void UpdateProgressTextL( TInt aFinalValue );

        /**
        * Checks that message server is up and running.
        * Also checks that message store is available.
        * @since Series 60 2.8
        * @return TBool ETrue if session to message server is ok and message
        *         store is available, otherwise EFalse.
        */
        TBool CheckMmsServiceAvailabilityL();
        
        /**
        * Shows requested information note.
        * @since Series 60 3.2
        */
        //void ShowInformationNoteL( TUint aTextResourceId ) const;

        /**
        * Resets the internal state.
        * @return None.
        */
        void Reset();
        
        /**
        * Complete request.
        * @return None.
        */
        void CompleteSelf();

    private: //Data
    
        enum TInternalState
            {
            EStateIdle       = 0,
            EStateCreateMessage,
            EStateImageCompression,
            EStateAddAttachment,
            EStateSendMessage,
            EStateFinalize,
            EStateReady
            };

        // Session to SendAs server. Owned.
        RSendAs                                 iSendAsSession;

        // Offers message creation and sending services. Owned.
        RSendAsMessage                          iSendAsMessage;

        // Progress dialog for creating messages. Owned.
        CAknProgressDialog*                     iProgressDialog;

        // Progress info.
        CEikProgressInfo*                       iProgressInfo;

        // Attachments for the messages. Not owned.
        const CArrayPtrFlat<CSendUiAttachment>* iAttachments;

        CCoeEnv&                                iCoeEnv;

        CSendUiSingleton&                       iSingleton;

        // Reference to an open fileserver session
        RFs& iFs;

        MMDUCreateAndSendCallback*              iCallback;

        // Image compressor. Owned.
        CUniImageProcessor*                     iCompressor;

        HBufC*                                  iToAddress;

        HBufC*                                  iToAlias;

        RFile                                   iCompressedImage;
        TFileName                               iCompressedImagePath;
        TDataType                               iAttachmentType;
        TInt                                    iState;
        TInt                                    iError;
        TInt                                    iProgressCount;
        TInt                                    iMessageCount;
        TInt                                    iMaxMsgSize;
        TInt                                    iAttachmentSize;
        TInt                                    iCompressionOk;
        TInt                                    iCompressionFail;        
        TBool                                   iImageCompressed;
        TBool                                   iMessageCreated;
        
    };

// ==========================================================

inline TInt CMDUCreateAndSend::Error() const
    {
    return iError;
    }

inline void CMDUCreateAndSend::CompressionResults(
    TInt& aCompressionOk,
    TInt& aCompressionFail ) const
    {
    aCompressionOk = iCompressionOk;
    aCompressionFail = iCompressionFail;
    }

#endif      // MDUCREATEANDSEND_H
