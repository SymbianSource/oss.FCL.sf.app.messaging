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
* Description:  
*       MMS viewer header.
*
*/



#ifndef __MMSVIEWERHEADER_H
#define __MMSVIEWERHEADER_H

// INCLUDES

#include <mmsclient.h>
#include <MsgEditorView.h>
#include <unibaseheader.h>  
#include <muiumsvuiserviceutilitiesinternal.h> // TMuiuAddressType

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMsgAddressControl;
class CMsgExpandableControl;
class CUniObjectList;

// CLASS DECLARATION

// ==========================================================

/**
* 
*/
class CMmsViewerHeader : public CUniBaseHeader
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        * @param aParent parent control
        * @param aFlags display flags - by default editor state
        * @return new object
        */
        static CMmsViewerHeader* NewL(
            CMmsClientMtm& aMtm,
            CUniObjectList& aAttachmentList,
            CMsgEditorView& aView,
            RFs& aFs );

        /**
        * Destructor.
        */
        ~CMmsViewerHeader();

        inline TMuiuAddressType SenderType();

        inline HBufC* Alias();

        /**
        * Due to infamous Japanese pictographs in subject field showing 
        * transparently even on slides 2,3...etc, function is used to remove
        * subject text when subject is visible and to the slide N is moved from slide 1
        */
        void RemoveSubjectContentL( );

        /**
        * Due to infamous Japanese pictographs in subject field showing 
        * transparently even on slides 2,3...etc, function is used to restore
        * subject when slide 1 is entered from slide N.
        */
        void RestoreSubjectContentL( );

        /**
        * Removes attachment field. When attachments view is removed separately, unnecessary
        * focus movements to attachment filed can be avoided e.g. in connection of forwarding
        * message. 
        */
        void RemoveAttachmentL( );

        /**
        * @return ETrue, if sender has alias in its address AND the alias is remote alias.
        */
        inline TBool IsRemoteAlias();

    private: // Constructors

        /**
        * Hidden C++ default constructor.
        */
        CMmsViewerHeader(
            CMmsClientMtm& aMtm,
            CUniObjectList& aAttachmentList,
            CMsgEditorView& aView,
            RFs& aFs );

        /**
        * By default Symbian constructor is private.
        */
        void ConstructL( );
        
        /**
        * Create UI control for from field. Base implementation is empty
        * @param aReadContent determines whether recipient data is read from
        *        MTM
        */
        void InsertFromL( TBool aReadContent );

        /**
        * Creates UI control for subject
        * @param aReadContent determines whether content is read to the
        *        UI control from MTM.
        *        Reading is not needed, if control is added by user selection
        */
        void InsertSubjectL(TBool aReadContent );

        /**
        * Create UI control for recipient.
        * @param aData data area for this recipient type
        * @param aResource resource to read basic recipient data from
        * @param aReadContent determines whether recipient data is read from
        *        MTM
        */
        void InsertRecipientL(  TAddressData&  aData,
                                THeaderFields  aRecipientType,
                                TBool          aReadContent);

        /**
        * Create UI control for recipient.
        * @param aData data area for this recipient type
        * @param aResource resource to read basic recipient data from
        * @param aReadContent determines whether recipient data is read from
        *        MTM
        * @param aAddHeadersRecipientType recipient type to be added as EUniFeature*      
        */
        void DoInsertRecipientL(    TAddressData&   aData,
                                    TInt            aResource,
                                    TBool           aReadContent,
                                    TInt            aAddHeadersRecipientType );

        /**
        * Creates UI control for attachment field
        * @param aReadContent Determines whether content is read to the UI control.
        */
        void InsertAttachmentL( TBool /*aReadContent*/);

    private: //Data
        TMuiuAddressType iSenderType;
        HBufC*          iAlias;
        // Indicates to which field Cc recipients are added, if it is variated Off
        // In practise only value is EUniFeatureTo 
        TInt            iAddIntoOtherCc;
        // Indicates to which field Bcc recipients are added, if it is variated Off
        // Value is either EUniFeatureTo or EUniFeatureCc
        TInt            iAddIntoOtherBcc;
        CUniObjectList& iAttachmentList;
        TBool           iRemoteAlias;
    };

inline TMuiuAddressType CMmsViewerHeader::SenderType()
    {
    return iSenderType;
    }

inline HBufC* CMmsViewerHeader::Alias()
    {
    return iAlias;
    }

inline TBool CMmsViewerHeader::IsRemoteAlias()
    {
    return iRemoteAlias;
    }

#endif // __MMSVIEWERHEADER_H
