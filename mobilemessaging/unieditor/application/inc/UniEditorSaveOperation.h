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
* Description:   CUniEditorSaveOperation, asynchronous operation for object saving     
*
*/



#ifndef __UNIEDITORSAVEOPERATION_H
#define __UNIEDITORSAVEOPERATION_H

// INCLUDES
#include "UniMsvEntry.h"
#include "UniEditorOperation.h"
#include "UniEditorDocument.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class CMDXMLDocument;
class CUniEditorHeader;
class CMsgEditorView;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CUniEditorSaveOperation, Storage for single attachment in presentation. 
*
* @since 3.2
*/
class CUniEditorSaveOperation : public CUniEditorOperation,
                                public MUniObjectSaveObserver,
                                public MUniSmilComposeObserver
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @since    3.1
        * @param    aObserver
        * @return   Pointer to instance in cleanup stack
        */
        static CUniEditorSaveOperation* NewL(
            MUniEditorOperationObserver& aObserver,
            CUniEditorDocument& aDocument,
            CUniEditorHeader& aHeader,
            CMsgEditorView& aView,
            RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CUniEditorSaveOperation();

        /**
        * Starts save operation
        */
        void Save( TUniSaveType aSaveType );
        
        /**
        * Start single object's save operation
        */
        void SaveL( CUniObject& aObject );
        
        /**
        * Detaches DOM object.
        */
        inline CMDXMLDocument* DetachDom();

    protected:

        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
       TInt RunError( TInt aError );
        /**
        * From MUniObjectSaveObserver
        */
        void ObjectSaveReady( TInt aError );

        /**
        * From MUniSmilComposeObserve
        */
        void SmilComposeEvent( TInt aError );
        
        /**
        * From CUniEditorOperation
        */
        void DoCancelCleanup();
        
    private:

        /**
        * Constructor.
        *
        * @since    3.1
        */
        CUniEditorSaveOperation( MUniEditorOperationObserver& aObserver,
                                 CUniEditorDocument& aDocument,
                                 CUniEditorHeader& aHeader,
                                 CMsgEditorView& aView,
                                 RFs& aFs );

        /**
        * 2nd phase constructor.
        *
        * @since    3.1
        */
        void ConstructL();

        /**
        * Performs save step
        */
        void DoSaveStepL();

        /**
        * Performs save checks
        */
        void DoSaveChecksL();

        /**
        * Saves object.
        */
        void DoSaveObjectL();

        /**
        * Saves attachments
        */
        void DoSaveAttachments();

        /** 
        * Composes SMIL
        */
        void DoComposeSmilL();

        /**
        * Finalizes save operation
        */
        void DoFinalizeSaveL();

        /**
        * Saves plugins
        */
        void DoSavePluginsL();

        /**
        * Checks disk space.
        */
        TBool CheckDiskSpaceL();

        /**
        * Creates description.
        *
        * @return Pointer to description buffer. NULL if description should not be updated.
        */
        HBufC* CreateDescriptionL();

        /**
        * Return ETrue if forward message, otherwise EFalse.
        */
        inline TBool IsForward() const;

    protected: // data

        // assigned to iOperationState
        enum TUniEditorSaveState
            {
            EUniEditorSaveCheck,
            EUniEditorSaveObjects,
            EUniEditorSaveAttachment,
            EUniEditorSaveSmilCompose,
            EUniEditorSaveFinalize,
            EUniEditorSavePlugins,
            EUniEditorSaveEnd,
            EUniEditorSaveError
            };

        CUniEditorHeader& iHeader;
        CMsgEditorView& iView;

        CMDXMLDocument* iDom;

        TUint32 iMaxMessageSize;
        TUniSaveType iSaveType;
        TBool iComposeSmil;
        
        CUniObject* iSavedObject;
        
        CMsvStore* iEditStore;

    };

#include "UniEditorSaveOperation.inl"

#endif // __MMSEDITORSAVEOPERATION_H
