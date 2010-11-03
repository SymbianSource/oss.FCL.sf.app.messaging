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
* Description:  State machine -based operation for restoring attached messages.
*
*/



#ifndef CMSGMAILRESTOREATTMSGSOP_H
#define CMSGMAILRESTOREATTMSGSOP_H

//  INCLUDES
#include "cmsgmailbaseop.h"
#include <msvapi.h>

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
* State machine -based operation for restoring attached messages.
*/
NONSHARABLE_CLASS( CMsgMailRestoreAttMsgsOp ) : public CMsgMailBaseOp
    {
    protected:
    
        enum TLaunchState
            {
            EGetMsgDigestEntries = EFirstFreeState, // get msg digest entries
            EPrepareAddAttMsgs, // preparation step for adding attached msgs
            EAddAttMsgs // add attached messages
            };
    
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        * @param aDocument Editor's document
        */
        static CMsgMailRestoreAttMsgsOp* NewL(
            CMsgMailEditorDocument& aDocument );

        /**
        * Destructor. Cancels operation.
        */
        ~CMsgMailRestoreAttMsgsOp();
        
    public: // API
    
        /**
        * Method for querying the count of all attached messages,
        * including invalid ones.
        * To get correct result this method should be called once the
        * operation has completed.
        * @return count of all attached messages
        */        
        TInt CountAllAttaMsgs() const;
            
        /**
        * Method for querying the count of invalid attached messages.
        * To get correct result this method should be called once the
        * operation has completed.
        * @return count of removed invalid attached messages
        */        
        TInt CountInvalidAttaMsgs() const;

    protected: // from CMsgMailBaseOp
    
        void SetNextState();
        
        void HandleStateActionL();
        
        void HandleOperationCancel();
        
        TBool HandleStateActionError( TInt aError );
        
    private:  // Implementation
        
        void GetMsgDigestEntriesL();
        
        void PrepareAddAttMsgsL();
        
        void AddAttMsgsL();
        
        void DoAddAttMsgL( TInt aIndex ) const;

    private:
    
        /**
        * Constructor.
        * @param aDocument Editor's document
        */
        CMsgMailRestoreAttMsgsOp( CMsgMailEditorDocument& aDocument );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:       
    
        // Entries belonging to the current message, not own
        const CMsvEntrySelection* iEntries;
        
        // Index for iterating entry selection
        TInt iIndex;
        
        // Number of all attached messages, including invalid ones
        TInt iAllAttaMsgCount;
        
        // Number of invalid attached messages
        TInt iInvalidAttaMsgCount;
    };

#endif      // CMSGMAILRESTOREATTMSGSOP_H

// End of File
