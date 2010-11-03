/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Base class for operations
*
*/



#ifndef __AUDIOMESSAGEOPERATION_H
#define __AUDIOMESSAGEOPERATION_H


#include "audiomessagedocument.h"
#include "amsoperationobserver.h"

/**
 * active object for saving,launching, inserting and sending opers. 
 *
 * @lib audiomessage.exe
 * @since S60 v3.1
 */

class CAudioMessageOperation : public CActive
    {
    public:  // New methods

        /**
         * Destructor
         */
        virtual ~CAudioMessageOperation();

        /**
        * Returns the error code set by the operation
        */
        TInt GetError( );
        

	// From CActive
    protected: 
        /**
        * From CActive
        */
        virtual TInt RunError( TInt aError );
        

    // New methods
        /**
         * Constructor.
         */
        CAudioMessageOperation(
            MAmsOperationObserver& aObserver,
            CAudioMessageDocument& aDocument);

	    /** 
	     * Get active and complete own status with error code.
	     * To be used when state machine loop must be terminated with error
	     */
        void CompleteSelf( TInt aError );

        /**
         * Makes storecommit
         */
 		void  MakeCommitL( );
        
        /**
         * Uninitializes store
         */
        void UnInit( );
        
        /**
         * Set error code
         * @param TInt error code
         */
        void SetError( TInt aError );

        /**
        * Resets error code
        */
        void ResetError( );
        
        /*
		* Creates smill
		*/
		void AddSmilL();
		
		/**
         * Calculates amout of attachments
         */
		TInt CheckAttasL( );
 
        /**
         * Sets store for editing and attachmentmanager
         */
       	void DoEditInitL( );

        /**
         * Sets store for reading and attachmentmanager
         */
        void DoReadInitL( );

        /**
         * Returns the index of attactment aId
         */
        TInt TMsvIdToIndexL( TMsvAttachmentId& aId );
        
    protected: // data
        MAmsOperationObserver& iObserver;
        CAudioMessageDocument& iDocument;

        /**
         * errors 
		 */
        TInt iError;
        MMsvAttachmentManager* iManager;
        MMsvAttachmentManagerSync* iManagerSync;

        /**
         * store 
         * Own.  
		 */
        CMsvStore* iStore;
        CMsvMimeHeaders* iHeaders;

        TFileName iContentLocationFileName;


       	};

#endif // __AUDIOMESSAGEOPERATION_H


