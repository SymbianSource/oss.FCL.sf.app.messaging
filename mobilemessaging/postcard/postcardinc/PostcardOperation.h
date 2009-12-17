/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       CPostcardOperation
*
*/



#ifndef __POSTCARDOPERATION_H
#define __POSTCARDOPERATION_H

// INCLUDES

#include <e32base.h>
#include <f32file.h>
#include <cmsvattachment.h>

#include "PostcardDocument.h"
#include "PostcardAppUi.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CMmsClientMtm;
class CMsvStore;
class MMsvAttachmentManager;
class CMsvMimeHeaders;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CPostcardOperation
*
* @lib postcard.exe
* @since 3.0
*/
class CPostcardOperation : public CActive
    {
    public:  // New methods

        /**
        * Destructor
        *
        * @since    3.0
        */
        virtual ~CPostcardOperation( );

        /**
        * Returns the error code set by the operation
        */
        virtual TInt GetError( );

        /**
        * Sets iStore and iManager
        */
		void InitL( );

        /**
        * Checks the attas of the current entry
        * and marks the number of different attachs in iNumOf.. members
        * Returns error code if error was found
        */
        TInt CheckAttasL( );
		
        /**
        * Deletes iStore and commits iManager if iCommit is set
        */
		void UnInitL( );

        /**
        * Pure virtual. All operations are started by using this function.
        */
		virtual void Start( TInt aArgument ) = 0;

        /**
        * Returns the index of attactment aId
        */
        TInt TMsvIdToIndexL( TMsvAttachmentId& aId );

    protected: // From base classes
    

        /**
        * Constructor.
        *
        * @since    3.0
        */
        CPostcardOperation(
            MPostcardOperationObserver& aOperationObserver,
            CPostcardDocument& aDocument,
            CPostcardAppUi& aAppUi,
            RFs& aFs );

        /**
        * CompleteSelf
        *
        * @since 3.0
        */
        void CompleteSelf( TInt aError );

        /**
        * Set error code
        * @param TInt error code
        */
        void SetError( TInt aError );

        /**
        * Resets error code
        */
        void ResetError( );

        /**
        * Recognises the file type according to mime type
        */
        TPostcardFileType RecogniseFileType( const TDataType& aMimeType );

    protected: // data

        MPostcardOperationObserver& iObserver;
        CPostcardDocument& iDocument;
        CPostcardAppUi& iAppUi;
        RFs& iFs;
        TInt iError;

        MMsvAttachmentManager* iManager;
        CMsvAttachment* iAttachment;
        CMsvStore* iStore;
        CMsvMimeHeaders* iHeaders;

        TMsvAttachmentId    iImage;
        TMsvAttachmentId    iText;
        TMsvAttachmentId    iRecipient;
        TMsvAttachmentId	iSmil;
        TInt 				iLaunchState;

        TBool				iCommit; // If set, commits iManager in UnInitL

    };

#endif // __POSTCARDOPERATION_H
