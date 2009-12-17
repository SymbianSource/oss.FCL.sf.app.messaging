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
* Description: State machine -based operation for mail editor view launching.
*
*/



#ifndef CMSGMAILLAUNCHOP_H
#define CMSGMAILLAUNCHOP_H

//  INCLUDES
#include "cmsgmailbaseop.h"

// DATA TYPES

// FORWARD DECLARATIONS
class CRichText;
class MMsgMailAppUiOpDelegate;
class CMsgMailRestoreSuperOp;

// CLASS DECLARATION
/**
* State machine -based operation for mail editor view launching.
* Operation consists e.g. of initialization, restoring body and
* attachments, executing view and finalizing operation. This operation
* needs to delegate some of its tasks to Editor App UI.
*/
NONSHARABLE_CLASS( CMsgMailLaunchOp ) : public CMsgMailBaseOp
    {
    protected:
    
        enum TLaunchState
            {
            EInitLaunch = EFirstFreeState, // initialization
            ERestoreBody, // restore message body
            EConvertLineBreaks, // convert body line breaks            
            ERestoreAllAttas, // restore attachments and atta msgs
            EExecuteView, // execute editor view
            EFinalize // finalize operation
            };
    
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        * @param aDocument Editor's document
        * @param aOpDelegate interface for delegating some tasks to app ui
        */
        static CMsgMailLaunchOp* NewL(
            CMsgMailEditorDocument& aDocument,
            MMsgMailAppUiOpDelegate& aOpDelegate );

        /**
        * Destructor. Cancels operation.
        */
        virtual ~CMsgMailLaunchOp();

    protected: // from CMsgMailBaseOp
    
        void SetNextState();
        
        void HandleStateActionL();
        
        void HandleOperationCancel();
        
    private:  // Implementation
        
        void InitLaunchL();
        
        void RestoreBodyL();
        
        void ConvertLineBreaksL();
        
        void RestoreAllAttasL();

        void ExecuteViewL();
        
        void FinalizeL();
        
        void ConvertLineBreaksL( CRichText& aRtf ) const;

    private:
    
        /**
        * Constructor.
        * @param aDocument Editor's document
        * @param aOpDelegate interface for delegating some tasks to app ui        
        */
        CMsgMailLaunchOp( CMsgMailEditorDocument& aDocument,
                          MMsgMailAppUiOpDelegate& aOpDelegate );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
    
        // Body text, valid after initialization phase, not own
        CRichText* iBodyText;
        
        // For delegating some unmanageable tasks, not own
        MMsgMailAppUiOpDelegate& iOpDelegate;
        
        // Operation for adding attachments and attached messages
        CMsgMailRestoreSuperOp* iRestoreSuperOp;
    };

#endif      // CMSGMAILLAUNCHOP_H

// End of File
