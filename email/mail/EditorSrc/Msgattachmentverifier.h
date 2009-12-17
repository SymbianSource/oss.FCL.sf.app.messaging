/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of class CMsgAttachmentVerifier
*
*/


#ifndef MSGATTACHMENTVERIFIER_H
#define MSGATTACHMENTVERIFIER_H

// INCLUDES
#include <e32base.h>
#include <MMGFetchVerifier.h>
#include <MAknFileSelectionObserver.h>

// FORWARD DECLARATIONS
class MsgMailDRMHandler;
class CMsvSession;

// CLASS DECLARATION

/**
*  Attachment verifier for MG fetch
*  @since  2.5
*/
class CMsgAttachmentVerifier: public CBase, 
	public MMGFetchVerifier, public MAknFileSelectionObserver
    {

public:  // Constructors and destructor
        
    /**
    * Two-phased constructor.
    */
    static CMsgAttachmentVerifier* NewLC( CMsvSession& aMsvSession );
    
    /**
    * Destructor.
    */
    ~CMsgAttachmentVerifier();

private: 
	// From MMGFetchVerifier
    TBool VerifySelectionL( const MDesCArray* aSelectedFiles );
    // From MAknFileSelectionObserver
    TBool OkToExitL( const TDesC& aDriveAndPath, const TEntry& aEntry );

private:

    /**
    * C++ default constructor.
    */
    CMsgAttachmentVerifier( CMsvSession& aMsvSession );

    /**
    * 2nd phase constructor.
    */
    void ConstructL();
    
private: // implementation
	
	TBool VerifyFileL( const TDesC& aFilename,
	                   TInt& aFileByteSize );
	                   
    TBool VerifyDiskSpaceL( TInt aAttachmentsByteSize );

protected:  // Data

    CMsvSession& iMsvSession;
    
    MsgMailDRMHandler* iDRMHandler;
    };

#endif      // MSGATTACHMENTVERIFIER_H   
            
// End of File
