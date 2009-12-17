/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CUniEditorProcessTextOperation class definition.      
*
*/



#ifndef CUNIEDITORPROCESSTEXTOPERATION_H
#define CUNIEDITORPROCESSTEXTOPERATION_H

// INCLUDES

#include <xhtmlparser.h>
#include "UniEditorOperation.h"

// FORWARD DECLARATIONS

// DATA TYPES

// CLASS DECLARATION

/**
* CUniEditorProcessTextOperation
*
* @since 3.2
*/
class CUniEditorProcessTextOperation : public CUniEditorOperation,
                                       public MXhtmlParserObserver
    {
    public: // new operations

        /**
        * Factory method
        */
        static CUniEditorProcessTextOperation* NewL( 
            MUniEditorOperationObserver& aObserver,
            CUniEditorDocument& aDocument,
            RFs& aFs );
        
        /**
        * Destructor
        */
        virtual ~CUniEditorProcessTextOperation();
        
        /**
        * Starts text process operation
        *
        * @param aImageInfo
        * @param aAttachmentId  Attachment id, if the image is already stored as an attachment
        *                       KMsvNullIndexEntryId, otherwise.
        * @param aMessageSize
        */
        void Start();
        
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
        * From CUniEditorOperation
        */
        void DoCancelCleanup();
        
        /**
        * From MXhtmlParserObserver
        *
        * Call back function used to inform a client of the Parser
        * when a parsing operation completes.
        */
    	void ParseCompleteL();
    	
    	/**
    	* From MXhtmlParserObserver
        *
        * Call back function used to inform a client of the Parser
        * about error.
        */
    	void ParseError( TInt aError );
        
    private: // new operations

        /**
        * C++ constructor
        */
        CUniEditorProcessTextOperation( MUniEditorOperationObserver& aObserver,
                                        CUniEditorDocument& aDocument,
                                        RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Checks whether some processing is needed for the text objects.
        */
        void DoStartCheck();

        /**
        * Start process step.
        */
        void DoStartProcessL();
        
        /**
        * Processing ready.
        */
        void DoReady();

        /**
        * Releases the limited resources used by the operation.
        */
        void Reset();
        
         /**
        * Creates empty text attachment.
        */
        void CreateEmptyTextAttachmentL( RFile& aFileHandle );
        
         /**
        * Replaces XHTML attachment with plain text attachment.
        */
        void ReplaceXhtmlAttachmentL( RFile& aFileHandle );
        
        /**
        * Writes text into file.
        */
        void WriteTextToFileL( CRichText& aText, RFile& aFileHandle );
        
        /**
        * Notify observer
        */ 
        void NotifyObserver( TUniEditorOperationEvent aEvent );
        
        /**
        * Performs parse error handling.
        */
        void HandleParserErrorL( TInt aError );
        
    private: // data

        enum TUniProcessStates
            {
            EUniProcessTxtCheck,
            EUniProcessTxtProcess,
            EUniProcessTxtNewFile,
            EUniProcessTxtReady,
            EUniProcessTxtError
            };
        
        CUniObject* iProcessedObject;
        CMsvStore* iEditStore;

        CXhtmlParser* iParser;
        TInt iSlideNum;
        
        TMsvAttachmentId iNewAttachmentId;
    };

#endif // CUNIEDITORPROCESSTEXTOPERATION_H
