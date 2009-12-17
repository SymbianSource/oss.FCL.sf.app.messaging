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
* 		State machine -based operation for restoring attachments and attached 
* 		messages.
*
*/



#ifndef CMSGMAILRESTORESUPEROP_H
#define CMSGMAILRESTORESUPEROP_H

//  INCLUDES
#include "cmsgmailbaseop.h"

// DATA TYPES

// FORWARD DECLARATIONS
class CMsgMailRestoreAttMsgsOp;
class CMsgMailRestoreAttasOp;

// CLASS DECLARATION
/**
* State machine -based operation for restoring attachments and attached 
* messages. Utilizes CMsgMailRestoreAttasOp and CMsgMailRestoreAttMsgsOp.
*/
NONSHARABLE_CLASS( CMsgMailRestoreSuperOp ) : public CMsgMailBaseOp
    {
    protected:
    
        enum TLaunchState
            {
            ERestoreAttachments = EFirstFreeState, // restore attachments
            ERestoreAttachedMsgs, // restore attached messages
            };
    
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        * @param aDocument Editor's document
        */
        static CMsgMailRestoreSuperOp* NewL(
            CMsgMailEditorDocument& aDocument );

        /**
        * Destructor. Cancels operation.
        */
        virtual ~CMsgMailRestoreSuperOp();
        
    public: // API
    
        /**
        * Method to show a proper note if there was one or more invalid
        * attachments or attached messages.
        * @return ETrue if there was reason to show a note
        */
        TBool ShowNoteIfInvalidAttaL() const;
        
        /**
        * Method to show a proper note if there was one or more DRM restricted
        * attachments that were removed from attachment list.
        * @return ETrue if there was reason to show a note
        */
        TBool ShowNoteIfDRMAttaL() const;    

    protected: // from CMsgMailBaseOp
    
        void SetNextState();
        
        void HandleStateActionL();
        
        void HandleOperationCancel();
        
    private:  // Implementation
        
        void RestoreAttachmentsL();
        
        void RestoreAttachedMsgsL();

    private:
    
        /**
        * Constructor.
        * @param aDocument Editor's document       
        */
        CMsgMailRestoreSuperOp( CMsgMailEditorDocument& aDocument );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:

        // Operation for adding attached messages, own
        CMsgMailRestoreAttMsgsOp* iRestoreAttMsgsOp;
        
        // Operation for adding attachments, own
        CMsgMailRestoreAttasOp* iRestoreAttasOp;          
    };

#endif      // CMSGMAILRESTORESUPEROP_H

// End of File
