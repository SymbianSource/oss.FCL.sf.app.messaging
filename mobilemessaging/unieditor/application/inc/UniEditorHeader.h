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
* Description:   UniEditor header.   
*
*/



#ifndef __UNIEDITORHEADER_H
#define __UNIEDITORHEADER_H

// INCLUDES
#include <MsgEditorView.h>
#include <MsgEditor.hrh>    // TMsgControlId
#include <MsgAddressControl.h>  

#include "UniClientMtm.h"
#include "uniaddresshandler.h"
#include "unibaseheader.h"
#include "uniobjectlist.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMsgExpandableControl;
class CMsgBaseControl;
class CMsgEditorAppUi;
class CUniEditorDocument;

// CLASS DECLARATION

// ==========================================================

/**
* Unified Message editor header class
*
* @since 3.2
*/
class CUniEditorHeader : public CUniBaseHeader,
                         public MUniObjectListObserver
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        *
        * @return new object
        */
        static CUniEditorHeader* NewL( CUniClientMtm&      aMtm,
                                       CUniEditorDocument& aDoc,
                                       CMsgEditorView&     aView,
                                       RFs&                aFs);

        /**
        * Destructor.
        */
        virtual ~CUniEditorHeader();
                
        /**
        * Delete controls from UI. Contents of data is lost
        *
        * @param aFlag Combination of EUniFeatureSubject, EUniFeatureCc
        *              and EUniFeatureBcc of enum TUniFeatures
        * @param aContentModified indicates whether removing causes data
        *        to be deleted
        */
        void AddHeadersDeleteL( TInt    aFlags,
                                TBool   aContentModified);

        /**
        * Add controls to the UI. Adding maybe delayed, if header controls
        * are not visible. 
        *
        * @param aFlags Combination of EUniFeatureSubject, EUniFeatureCc
        *               and EUniFeatureBcc of enum TUniFeatures. Bits of existing fields
        *               are accepted and thus ignored.
        */
        void AddHeadersAddL(TInt aFlags);

        /**
        *  Clears old addresses from MTM and copies data from 
        *  address control to MTM, if any of the controls have changed.
        *  If changes have not taken place, returns immediately.
        *  MTM is not saved into messaging store
        *
        *  @param  aDocumentHeaderModified indicates whether document header is modified
        *
        *  @return ETrue, if addresses were really copied
        *          EFalse, no changes, addresses were not copied
        */        
        TBool CopyAddressesToMtmL(  TBool   aDocumentHeaderModified );

        /**
        *  Clears old addresses from MTM and copies data from 
        *  address control to MTM, if any of the controls have changed.
        *  Saves also subject, if it has changed.
        *  If changes have not taken place, returns immediately.
        *
        *  @param aDocumentHeaderModified indicates whether document header is modified
        *  @param aSaveToMtm whether changes are saved to store, if controls were
        *         modified
        *
        *  @return ETrue, if addresses were really copied
        *          EFalse, no changes, addresses were not copied
        */        
        TBool CopyHeadersToMtmL(    TBool aDocumentHeaderModified,
                                    TBool aSaveToMtm );

        /**
        * Verifies addresses in address control.
        *
        * @param aAcceptEmails tells if email addresses should be accepted
        *
        * @return ETrue, if verify successful,
        *         EFalse otherwise
        */        
        TBool VerifyAddressesL( TBool& aModified, TBool aAcceptEmails );

        /**
        * Removes duplicate addresses from recipients at send time.
        *
        * @param aAppUi Pointer to editor appui
        *
        * @return ETrue if something was removed,
        *         EFalse otherwise
        */        
        TBool RemoveDuplicateAddressesL();

        /**
        * Add recipient.
        *
        * @param aFocusedControl currently focused control
        * @param aParent    IN  Parent control (normally iView)
        * @param aIncludeEmailAddress IN Specifies whether recipients with email
        *                                address should be included.
        * @param aInvalid   OUT ETrue if invalid addresses found
        *                       EFalse otherwise
        *
        * @return ETrue if something was added,
        *         EFalse otherwise
        */
        TBool AddRecipientL( CMsgBaseControl* aFocusedControl,
                             MObjectProvider* aParent, 
                             TBool aIncludeEmailAddresses,
                             TBool& aInvalid );

        /**
        * Check if the header of the message is in the SMS scope
        *
        * @return ETrue, if header is SMS compatible
        *         EFalse otherwise
        */
        TBool IsHeaderSmsL();
        
        /**
        * Makes details. Operation not bound to any address type
        */
        inline void MakeDetailsL( TDes& aDetails );

        /**
        * Refreshes the length of the current longest email address (iLongestEmail variable)
        */
        void RefreshLongestEmailAddressL();
        
        /**
        * Returns the length of longest email address
        */
        inline TInt LongestEmailAddress();
        
        /**
        * Returns ETrue if there are unverified recipients
        */        
        TBool NeedsVerificationL();

        /**
        * Shows invalid recipient information notes for all invalid attachments
        * on every address field detected during editor application launching.
        */
        void ShowInvalidContactNotesL();

    public: // Functions from base classes
    
        /**
        * From MUniObjectListObserver
        * See UniObjectList.h
        */
        void ObjectAddedL( CUniObjectList* aList, 
                           CUniObject* aObject,
                           TInt aIndex );
        
        /**
        * From MUniObjectListObserver
        * See UniObjectList.h
        */
        void ObjectRemovedL( CUniObjectList* aList, 
                             CUniObject* aObject, 
                             TInt aIndex );
    
    private: // Constructors

        /**
        * C++ constructor.
        */
        CUniEditorHeader( CUniClientMtm& aMtm,
                          CUniEditorDocument& aDoc,
                          CMsgEditorView& aView,
                          RFs& aFs );

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Creates UI control for subject
        *
        * @param aReadContent determines whether content is read to the
        *        UI control from MTM.
        *        Reading is not needed, if control is added by user selection
        */
        void InsertSubjectL(TBool aReadContent );
        
        /**
        * From CUniBaseHeader
        * See UniBaseHeader.h
        */
        void InsertAttachmentL( TBool aReadContent );
        
        /**
        * Removes attachment control from header
        */
        void RemoveAttachmentL();

        /**
        * Adds additional headers onto the UI immediately
        *
        * @param aReadContent determines whether content is read to the
        *        UI control from MTM.
        *        Reading is not needed, if control is added by user selection
        */
        void DoAddHeadersAddL( TBool aReadContent );

        /**
        * Create UI control for recipient.
        *
        * @param aData data area for this recipient type
        * @param aResource resource to read basic recipient data from
        * @param aReadContent determines whether recipient data is read from
        *        MTM
        */
        void InsertRecipientL(  TAddressData&  aData,
                                THeaderFields  aRecipientType,
                                TBool          aReadContent);

        /**
        * Tells in which index the control aControlToAdd should be added
        */
        TInt ControlIndexForAdding( TInt aControlToAdd );
    
    	/**
        * Tells if the address is valid email address
        */
        TBool IsValidEmailAddress( const TDesC& aAddress );
        
    private: //Data
    
        CUniEditorDocument& iDoc;
        TInt                iLongestEmail;
    };

// ---------------------------------------------------------
// CUniEditorAppUi::MakeDetails
// ---------------------------------------------------------
//
inline void CUniEditorHeader::MakeDetailsL( TDes& aDetails )    
    {
    return iHeaders[EHeaderAddressTo].iAddressHandler->MakeDetailsL( aDetails );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::LongestEmailAddress
// ---------------------------------------------------------
//
inline TInt CUniEditorHeader::LongestEmailAddress()
    {
    return iLongestEmail;
    }

#endif // __UNIEDITORHEADER_H

// End of File