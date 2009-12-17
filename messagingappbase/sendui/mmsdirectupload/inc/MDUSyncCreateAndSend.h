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
* Description:   Wrapper to make message creation and sending synchronous.
*
*/




#ifndef CMDUSYNCCREATEANDSEND_H
#define CMDUSYNCCREATEANDSEND_H

//  INCLUDES
#include "MDUCreateAndSend.h"

// FORWARD DECLARATIONS
//class CMDUCreateAndSend;
class CActiveSchedulerWait;
class CSendUiSingleton;

// CLASS DECLARATION

/**
*  Wrapper to make message creation and sending synchronous.
*
*  @lib MmsDirectUpload.lib
*  @since Series 60 2.8
*/
class CMDUSyncCreateAndSend : public CBase, 
                              public MMDUCreateAndSendCallback
                              
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMDUSyncCreateAndSend* NewLC( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton );

        /**
        * Destructor.
        */
        virtual ~CMDUSyncCreateAndSend();

    public: // New functions        

        /**
        * Creates and sends as many MMS messages as there are files
        * to be send.Synchronous operation.
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
            TInt aMaxMsgSize);
                    
        /**
        * Error
        * @since Series 60 3.0
        * @return Returns system wide error code.
        */
        inline TInt Error() const;
        
        /**
        * Can be used to query compression results.
        * Oversized JPEG images are compressed to fit in MMS.
        *
        * @since Series 60 2.8
        * @param aCompressionOk, Return value contains amount of succesfully
        *        compressed JPEG images.
        * @param aCompressionFail, Return value contains amount of failed
        *        compression attempts.
        * @return None.
        */
        inline void CompressionResults(
            TInt& aCompressionOk,
            TInt& aCompressionFail ) const;

    private: // Functions from base classes

        /**
        * From MMmsCreateAndSendCallback.
        * Callback to tell that message creation and sending is ready.
        * @since Series 60 2.8
        * @return None.
        */
        void MessagesReady();

    private: 

        /**
        * C++ default constructor.
        */
        CMDUSyncCreateAndSend();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CCoeEnv& aCoeEnv, CSendUiSingleton& aSingleton );

        /**
        * Starts asynchronous wait loop.
        * @since Series 60 3.0
        * @return None.
        */
        void BeginActiveWait();

    private: // Data
        
        // Asynchronous image compressor. Owned.
        CMDUCreateAndSend*              iCreateAndSend;
        
        // For Active Scheduler wait loop. Owned
        CActiveSchedulerWait            iWait;
};


// ==========================================================

inline TInt CMDUSyncCreateAndSend::Error() const
    {
    return ( !iCreateAndSend ? KErrNotReady : iCreateAndSend->Error() );
    }

inline void CMDUSyncCreateAndSend::CompressionResults(
    TInt& aCompressionOk,
    TInt& aCompressionFail ) const
    {
    iCreateAndSend->CompressionResults( aCompressionOk, aCompressionFail );
    }

#endif      // CMDUSYNCCREATEANDSEND_H

// End of file
