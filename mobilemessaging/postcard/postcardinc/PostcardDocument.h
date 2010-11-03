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
*       CPostcardDocument, document class for Postcard application.
*
*/




#ifndef __POSTCARDDOCUMENT_H
#define __POSTCARDDOCUMENT_H

//  INCLUDES
#include <msvapi.h>                     // CMsvEntry
#include <MsgEditorDocument.h>          // CMsgEditorDocument

// CONSTANTS

// This tell how much of the max MMS size is reserved for greeting and recipient
const TInt KPostcardAddressGreetingOverhead = 2000;
// What's the max. size the greeting text can store itself internally
// (the max. size of sent text is read from central repository)
const TInt KPostcardDefaultGreetingSegment = 512; 

// MACROS

// DATA TYPES

enum TPostcardMsgType // Type of the postcard 
    {
    EPostcardNew = 0,
    EPostcardDraft,
    EPostcardForward,
    EPostcardEdit,
    EPostcardSendAs,
    EPostcardSent
    };

enum TPostcardPart // Enumerations for different parts of the postcard
    {
    EPostcardImage = 1,
    EPostcardText = 2,
    EPostcardRecipient = 4,
    EPostcardNone = 8   // Prevents unnecessary icons to be drawn at startup.
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMmsClientMtm;
class CBaseMtmUi;
class CMsgMediaResolver;
class CMsgTextUtils;

// CLASS DECLARATION

class CPostcardDocument :public CMsgEditorDocument
    {
    public:  // Constructors and destructor

    /**
    * Destructor.
    */
    ~CPostcardDocument();

    /**
    * C++ constructor
    *
    * @param aApp   Reference to application instance
    */
    CPostcardDocument( CEikApplication& aApp );

    /**
    * Factory function to create this object
    *
    * @param aApp   Reference to application instance
    */
    static CPostcardDocument* NewL( CEikApplication& aApp );

    public: // New functions

    /**
    * InitializeL
    * Creates MediaResolver and MmsData
    */ 
    void InitializeL( RFs& aFs );

    public: // Virtuals from CMsgEditorDocument

    /**
    * DefaultMsgFolder
    * Returns default folder for new messages.
    * @return ID of the Drafts folder.
    */
    TMsvId DefaultMsgFolder() const;

    /**
    * DefaultMsgService
    * From CMsgEditorDocument
    * Returns default message service used by application
    * @return TMsvId KMsvLocalServiceIndexEntryId.
    */
    TMsvId DefaultMsgService() const;

    /**
    * CreateNewL
    * From CMsgEditorDocument
    * Creates new message.
    * A new message of type Postcard is created
    * in a destination folder.
    * Calls CreateNewL(TMsvId aServiceType, TMsvId aDestination,
    *                  TInt aTypeList).
    * @param aService service used for message
    * @param aDestFolder folder in which message will be saved eg.
    *                    KMsvDraftEntryIdValue
    * @return ID of mail operation.
    */
    TMsvId CreateNewL(TMsvId aService, TMsvId aDestFolder);

    /**
    * EntryChangedL
    * From CMsgEditorDocument
    * Called after entry is changed by BaseEditor SetEntry Function.
    * From MsgEditorDocument
    */
    void EntryChangedL();

    public: // From CMsgEditorDocument

    /*
    * Mtm
    * Accessor (Overrides baseeditor)
    * Returns CMmsClientMtm reference correctly casted from CBaseMtm
    * @param  None
    * @return Reference to documents CMmsClientMtm
    */
    CMmsClientMtm& Mtm();

    /*
    * MtmUi
    * Accessor (Overrides baseeditor)
    * Returns CMmsMtmUi reference correctly casted from CBaseMtmUi
    * @param  None
    * @return Reference to documents CMmsMtmUi
    */
    CBaseMtmUi& MtmUi();

    public:

    /**
    * Get pointer to MediaResolver
    * @return Pointer to MediaResolver
    */
    inline CMsgMediaResolver* MediaResolver( ) { return iMediaResolver; };

    /**
    * Get pointer to MmsData
    * @return Pointer to MmsData
    */
	inline CMsgTextUtils* TextUtils( ) { return iTextUtils; };
			
    /**
    * Messagetype accessor
    *
    * @return Message type
    */
    inline TPostcardMsgType MessageType() const { return iMsgType; };

    public: // new ones:

    /**
    * Sets aPart as changed
    */
    void SetChanged( TPostcardPart aPart );

    /**
    * Checks if aPart is changed
    */
    TBool Changed( TPostcardPart aPart ) const;

    /**
    * ETrue if any part of the postcard has been changed
    */
    TBool Modified( ) const;

    /**
    * Set whether message is SendAs, Forward, Reply or Open from Draft.
    * Function determines it from launch flags & entry flags.
    */
    void SetMessageType();

    /**
    * Returns the max size of the message
    */
	TInt MaxMessageSize( ) const;

    /**
    * Returns the max size of the image (max message - textrecipientoverhead)
    */
	TInt MaxImageSize( ) const;

    /**
    * Called by Controller so it knows if the app is started from ROM or not
    * Returns full filepath to this application
    */        
    TFileName AppFullName( );

    /**
    * Check if the mimetype is an image
    */
    TBool IsImage( const TDataType& aMimeType );

    /**
    * Tells the maximum width and height of the image.
    * If image is bigger than that -> scaling is needed
    */
	TSize MaxImageDimensions( );
	
    protected:  // New functions

    /**
    * ConstructL
    * 2nd phase constructor.
    */
    void ConstructL();

    private: // New Functions

    /**
    * Creates a new CPostcardAppUi
    */
    CEikAppUi* CreateAppUiL();

    private:  // data

    RFs                 iFs;
    TInt                iMsgChanged;
    TPostcardMsgType    iMsgType;
    CMsgMediaResolver*  iMediaResolver;
    CMsgTextUtils*      iTextUtils;
    TInt 				iMaxMessageSize;
    };


#endif   // __POSTCARDDOCUMENT_H

// End of file
