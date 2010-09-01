/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: State machine -based operation for fetching files as attachments.
*
*/



#ifndef CMSGMAILFETCHFILEOP_H
#define CMSGMAILFETCHFILEOP_H

//  INCLUDES
#include "cmsgmailbaseop.h"
#include <MsgAttachmentUtils.h>
#include <akninputblock.h>

// DATA TYPES

// FORWARD DECLARATIONS
class CMsgMailRestoreSuperOp;
class MMsvAttachmentManager;

// CLASS DECLARATION
/**
* State machine -based operation for fetching files as attachments.
* This operation consinsts of file selection query, adding attachments
* and performing restore operation for all attachments.
*/
NONSHARABLE_CLASS( CMsgMailFetchFileOp ) : public CMsgMailBaseOp, 
                                           public MAknInputBlockCancelHandler
    {
    protected:
    
        enum TLaunchState
            {
            ESelectFiles = EFirstFreeState, // user selects files to attach
            EAddAttas, // add attachments to message
            ERestoreAllAttas, // performs full restore, updates mime types etc.
            EFinalize // finalize operation
            };
    
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        * @aFetchType attachment type
        * @param aDocument Editor's document
        */
        static CMsgMailFetchFileOp* NewL(
            MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType,
            CMsgMailEditorDocument& aDocument );

        /**
        * Destructor. Cancels operation.
        */
        ~CMsgMailFetchFileOp();        

    protected: // from CMsgMailBaseOp
    
        void SetNextState();
        
        void HandleStateActionL();
        
        void HandleOperationCancel();
    
    public: // from MAknInputBlockCancelHandler
        
        void AknInputBlockCancel();
        
    private:  // Implementation

        void SelectFilesL();
        
        void AddAttasL();
        
        void RestoreAllAttasL();
        
        void FinalizeL();    
    
        void AddMultipleAttachmentsL( CDesCArray& aAttachments,
                                      CMsgMailEditorDocument& aDocument,
                                      TRequestStatus& aStatus ) const;
    
        void AddSingleAttachmentL( const TDesC& aAttachmentName,
                                   CMsgMailEditorDocument& aDocument,
                                   TRequestStatus& aStatus ) const;

    private:
    
        /**
        * Constructor.
        * @aFetchType attachment type
        * @param aDocument Editor's document        
        */
        CMsgMailFetchFileOp(
            MsgAttachmentUtils::TMsgAttachmentFetchType aFetchType,
            CMsgMailEditorDocument& aDocument );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
    
        // number of attachments that were selected by the user to be fetched
        TInt iFetchCount;
    
        // attached file type
        MsgAttachmentUtils::TMsgAttachmentFetchType iFetchType;
                      
        // 1st alternative, fetched file name when single item is fetched
        TFileName iFileName;
                    
        // Attachment manager, not own
        MMsvAttachmentManager* iManager;    
    
        // 2nd alternative, file names when multiple items are fetched, own
        CDesCArray* iFetchArray; 

        // Operation for adding attachments, own
        CMsgMailRestoreSuperOp* iRestoreSuperOp;             
        
        // Own. Input blocker to prevent user interaction during process
        CAknInputBlock* iBlocker;
    };

#endif      // CMSGMAILFETCHFILEOP_H

// End of File
