/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CUniEditorVCardOperation class definition.      
*
*/



#ifndef __UNIEDITORVCARDOPERATION_H
#define __UNIEDITORVCARDOPERATION_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

// Virtual phonebook
#include <CVPbkVCardEng.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkSingleContactOperationObserver.h>

#include <MsgCheckNames.h>

#include "UniEditorOperation.h"
#include "UniEditorDocument.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CContactMatcher;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CUniEditorVCardOperation, Storage for single attachment in presentation. 
*
* @since 3.2
*/
class CUniEditorVCardOperation : public CUniEditorOperation,
                                 public MVPbkSingleContactOperationObserver
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @since    3.2
        * @param    aObserver, reference to application UI.
        * @param    aDocument, reference to application's document.
        * @param    aFs, reference to file session
        * @return   Pointer to instance in cleanup stack
        */
        static CUniEditorVCardOperation* NewL(
            MUniEditorOperationObserver& aObserver,
            CUniEditorDocument& aDocument,
            RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CUniEditorVCardOperation();

        /**
        * Starts the operation         
        */
        void Start();

        /**
        *
        */
        void GetContactsL();

        /**
        * Start the vCard export and insert operation.
        */
        void ExportVCardAndAddAttachmentL();
   
        /**
        * Returns the cound of added vCards.
        *
        * @return   Count of the added vCard.
        */
        TInt AddedVCardCount();

    protected: 

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CUniEditorOperation
        */
        void DoCancelCleanup();
    
        /**
        * From MVPbkSingleContactOperationObserver
        */
        void VPbkSingleContactOperationComplete(
            MVPbkContactOperationBase& aOperation, 
            MVPbkStoreContact* aContact);
        
        /**
        * From MVPbkSingleContactOperationObserver
        */
        void VPbkSingleContactOperationFailed(
            MVPbkContactOperationBase& aOperation, 
            TInt aError);
    
    private:

        /**
        * Constructor.
        *
        * @param    aObserver, reference to application UI.
        * @param    aDocument, reference to application's document.
        * @param    aFs, reference to file session
        */
        CUniEditorVCardOperation( MUniEditorOperationObserver& aObserver,
                                  CUniEditorDocument& aDocument,
                                  RFs& aFs );

        /**
        * 2nd phase constructor.        
        */
        void ConstructL();
        
        /**        
        *
        */
        void Reset();

        /**
        *
        */
        void ResetAll();
        
        /**
        * Reports events to application UI.
        */
        void ReportEvent( TUniEditorOperationEvent aEvent );
        
        /**
        * Creates an empty attachment to message store.
        */
        void CreateEmptyVCardAttachmentL();

        /**
        * Retrieves contact's data from phonebook.
        */
        void RetrieveStoreContactL();
        
    private:

        enum TUniProcessStates
            {
            EUniProcessVCardStart,
            EUniProcessVCardExportNext,
            EUniProcessVCardCreatingEmptyVCardAttachment,
            EUniProcessVCardComplete,
            EUniProcessVCardError,
            EUniProcessVCardCancel
            };
    
        MVPbkContactLink*           iContact;
        CMsgCheckNames*             iCheckNames;
        CVPbkContactLinkArray*      iContacts;
        MVPbkStoreContact*          iStoreContact;
        CVPbkVCardEng*              iVCardEng;
        MVPbkContactOperationBase*  iVCardExportOp;
        
        RFileWriteStream            iVCardStream;
        
        TMsvAttachmentId            iNewVCardAttaId;
        CMsvStore*                  iEditStore;
        TInt                        iLastError;
        
        TInt                        iAddedVCardCount;
        TInt                        iCurrentContactIndex;
    };

#endif // __UNIEDITORVCARDOPERATION_H
