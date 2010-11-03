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
* Description:  State machine -based operation for restoring sinle attachment.
*
*/



#ifndef CMSGMAILRESTOREATTAOP_H
#define CMSGMAILRESTOREATTAOP_H

//  INCLUDES
#include "cmsgmailbaseop.h"
#include <msvapi.h>

// DATA TYPES

// FORWARD DECLARATIONS
class MsgMailDRMHandler;
class MMsvAttachmentManager;

// CLASS DECLARATION
/**
* State machine -based operation for restoring sinle attachment.
* This class is used by it's parent CMsgMailRestoreAttaOp.
* This operation modifies the attachment model as necessary.
*/
NONSHARABLE_CLASS( CMsgMailRestoreAttaOp ) : public CMsgMailBaseOp
    {
    protected:
    
        enum TLaunchState
            {
            ECheckAttaMime = EFirstFreeState, // check & update mime type
            ECheckAttaDRM, // check DRM status
            EFinalize // finalize operation
            };
    
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        * @aIndex attachment's index
        * @param aDocument Editor's document
        */
        static CMsgMailRestoreAttaOp* NewL(
            TInt aIndex,
            CMsgMailEditorDocument& aDocument );

        /**
        * Destructor. Cancels operation.
        */
        ~CMsgMailRestoreAttaOp();

    protected: // from CMsgMailBaseOp
    
        void SetNextState();
        
        void HandleStateActionL();
        
        void HandleOperationCancel();
        
    private:  // Implementation
        
        void CheckAttaMimeL();
        
        void CheckAttaDRML();
        
        void Finalize();

    private:
    
        /**
        * Constructor.
        * @aIndex attachment's index
        * @param aDocument Editor's document        
        */
        CMsgMailRestoreAttaOp( TInt aIndex,
                               CMsgMailEditorDocument& aDocument );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
    
        // Boolean which tells if DRM check noticed that attachment is
        // forward locked and thereby removed from attachment list
        TBool iIsFLAtta;
    
        // Index of this attachment
        TInt iIndex;
    
        // Attachment manager, not own
        MMsvAttachmentManager* iManager;    
        
        // DRM handler, own
        MsgMailDRMHandler* iDrmHandler;
    };

#endif      // CMSGMAILRESTOREATTAOP_H

// End of File
