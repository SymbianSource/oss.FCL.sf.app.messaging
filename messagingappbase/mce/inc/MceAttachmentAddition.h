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
* Description:   Active object for adding an attachment to the entry
*                in the message store (3.x platform).
*
*/




#ifndef MCE_ATTACHMENT_ADDITION_H
#define MCE_ATTACHMENT_ADDITION_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CMsvEntry;
class CMsvStore;
class MMsvAttachmentManager;


// CLASS DECLARATION

/**
*  Active object for adding an attachment to the entry in the message store.
*  @lib Mce.exe
*  @since Series 60 3.0
*/
class CMceAttachmentAddition : public CActive
	{
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
	    static CMceAttachmentAddition* NewLC();
	    
        /**
        * Destructor.
        */
	    ~CMceAttachmentAddition();
	    
    public: // New functions	    
    
        /**
        * Requests the attachment manager to add the attachment and waits for the
        * operation to complete.
        * @since Series 60 3.0
        * @param aEntry: Entry in the message store
        * @param aFileName: Name of the attachment file
        * @param aSize: Size of the attachment file
        * @return None.
        */
        void AttachFileL( CMsvEntry* aEntry, TFileName aFileName, RFile& aFileHandle, TInt aSize );

	    /**
        *
        * @since Series 60 3.0
        * @return Status of the asyncronous request.
        */	        
        TInt Result() const;
        
    public:  // Functions from base classes
    
        /**
        * From CActive, completion of the asyncronous request
        * @since Series 60 3.0
        * @return None.
        */ 
	    void RunL();
	    
	    /**
        * From CActive, cancellation of the asyncronous request
        * @since Series 60 3.0
        * @return None.
        */        
        void DoCancel();
        
        /**
        * From CActive, handles the leaves from RunL
        * @since Series 60 3.0
        * @param aError: Error code
        * @return None.
        */
        TInt RunError( TInt );

    private:

        /**
        * C++ default constructor.
        */
	    CMceAttachmentAddition();
	
	    /**
        * Resets internal state.
        * @since Series 60 3.0
        * @return None.
        */ 
	    void Reset();

    private:     // Data

        CMsvStore*                  iStore;
        MMsvAttachmentManager*      iAttachmentManager;
        TInt                        iError;    
        CActiveSchedulerWait         iWait;
	};

#endif      // CMCE_ATTACHMENT_ADDITION_H
            
// End of File
