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
* Description:  State machine -based operation for sending mail message.
*
*/



#ifndef CMSGMAILSENDOP_H
#define CMSGMAILSENDOP_H

//  INCLUDES
#include "cmsgmailbaseop.h"
#include <msvapi.h>
#include <muiuflagger.h>                    // CMuiuFlags

// DATA TYPES

// FORWARD DECLARATIONS
class CMsgMailPreferences;
class MMsgMailAppUiOpDelegate;

// CLASS DECLARATION
/**
* State machine -based operation for sending mail message.
* The most important steps are to save and send the message.
* Some tasks need to be delegated to Editor App UI.
*/
NONSHARABLE_CLASS( CMsgMailSendOp ) : public CMsgMailBaseOp
    {
    protected:
    
        enum TSendState
            {
            EPrepare = EFirstFreeState, // praparation phase
            ESaveMessage, // save message
            ESendMessage, // send message
            EFinalize // finalize operation
            };
    
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        * @param aDocument Editor's document
        * @param aOpDelegate interface for delegating some tasks to app ui
        */
        static CMsgMailSendOp* NewL(
            CMsgMailEditorDocument& aDocument,
            MMsgMailAppUiOpDelegate& aOpDelegate );

        /**
        * Destructor. Cancels operation.
        */
        virtual ~CMsgMailSendOp();
        
    protected: // from CMsgMailBaseOp
    
        void SetNextState();
        
        void HandleStateActionL();
        
        void HandleOperationCancel();

    private:  // Implementation
        
        void PrepareL();
        
        void SaveMessageL();
        
        void SendMessageL();
        
        void Finalize();

        TMsvId SendingBoxL( CMsgMailPreferences& aPrefs ) const;
        
        void SetSchedulingOptionsL( CMsgMailPreferences& aPrefs,
                                    TMsvId aSendingBox ) const;
                                    
        TInt WaitNoteTextResourceId() const;
        
        TBool CheckOwnAddressL() const;
        
        void SendNativeMailL();
        

    private:
    
        /**
        * Constructor.        
        * @param aDocument Editor's document
        * @param aOpDelegate interface for delegating some tasks to app ui
        */
        CMsgMailSendOp( CMsgMailEditorDocument& aDocument,
                        MMsgMailAppUiOpDelegate& aOpDelegate );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
    
        // For delegating some unmanageable tasks, not own
        MMsgMailAppUiOpDelegate& iOpDelegate;
        
        CMuiuFlags*     iFlags;
    };

#endif      // CMSGMAILSENDOP_H

// End of File
