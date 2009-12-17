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
* Description:  
*           State machine -based operation for restoring message's attachments.
*
*/



#ifndef CMSGMAILRESTOREATTASOP_H
#define CMSGMAILRESTOREATTASOP_H

//  INCLUDES
#include "cmsgmailbaseop.h"
#include <msvapi.h>

// DATA TYPES

// FORWARD DECLARATIONS
class MMsvAttachmentManager;
class CMsgMailRestoreAttaOp;

// CLASS DECLARATION
/**
* State machine -based operation for restoring message's attachments.
* This operation involves restoring each individual attachments, and
* furthermore initialization and finalization phase.
*/
NONSHARABLE_CLASS( CMsgMailRestoreAttasOp ) : public CMsgMailBaseOp
    {
    protected:
    
        enum TLaunchState
            {
            EInitialize = EFirstFreeState, // initialize operation
            ERestoreAtta, // restore single atta
            EFinalize // finalize entire operation
            };
    
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        * @param aDocument Editor's document
        */
        static CMsgMailRestoreAttasOp* NewL(
            CMsgMailEditorDocument& aDocument );

        /**
        * Destructor. Cancels operation.
        */
        ~CMsgMailRestoreAttasOp();
        
    public: // API
            
        /**
        * Method for querying the number of all attachments involved in
        * this operation, including any discarded attachments.
        * To get correct result this method should be called once the
        * operation has completed.        
        * @return count of all attachments, i.e. 
        */
        TInt CountAllAttas() const;          
    
        /**
        * Method for querying the number of attachments that were
        * removed due to DRM restrictions.
        * To get correct result this method should be called once the
        * operation has completed.
        * @return count of removed DRM restricted attachments
        */        
        TInt CountDRMAttas() const;
        
        /**
        * Method for querying the number of invalid attachments. This
        * number does not include DRM attachments.
        * To get correct result this method should be called once the
        * operation has completed.
        * @return count of removed invalid attachments
        */        
        TInt CountInvalidAttas() const;   

    protected: // from CMsgMailBaseOp
    
        void SetNextState();
        
        void HandleStateActionL();
        
        void HandleOperationCancel();
        
        TBool HandleStateActionError( TInt aError );
        
    private:  // Implementation
        
        void InitializeL();
        
        void RestoreAttaL();
        
        void Finalize();

    private:
    
        /**
        * Constructor.
        * @param aDocument Editor's document        
        */
        CMsgMailRestoreAttasOp( CMsgMailEditorDocument& aDocument );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
    
        // Count of all attachments, including discarded attachments
        TInt iAllAttaCount;
    
        // Count of DRM restricted files
        TInt iDRMAttaCount;    
        
        // Count of invalid attachments (excluding DRM problems)
        TInt iInvalidAttaCount;
    
        // Index for iterating attachment manager's attachments
        TInt iIndex;
    
        // Attachment manager, not own
        MMsvAttachmentManager* iManager;    
        
        // Currently restored attachment, own
        CMsgMailRestoreAttaOp* iRestoreOneAtta;
    };

#endif      // CMSGMAILRESTOREATTASOP_H

// End of File
