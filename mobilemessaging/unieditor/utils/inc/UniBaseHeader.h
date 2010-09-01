/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unified Message Editor base header.
*
*/



#ifndef C_UNIBASEHEADER_H
#define C_UNIBASEHEADER_H

// INCLUDES
#include <MsgEditorView.h>
#include <MsgEditor.hrh>    // TMsgControlId
#include <MsgAddressControl.h>
#include <MsgAttachmentControl.h>
#include <cmsvrecipientlist.h>
#include <unimodelconst.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMsgExpandableControl;
class CUniAddressHandler;
class CMsgBaseControl;
class CMsgEditorAppUi;
class CBaseMtm;

// CLASS DECLARATION

// ==========================================================

/**
*  Unified Editor Base Header class declaration.
*  Class provides common services for editor and viewer header classes. 
*
*  @lib UniUtils.lib
*
*  @since S60 3.2
*/
class CUniBaseHeader : public CBase
    {
    public:  // Constructor and destructor

        enum THeaderFields
            {
            EHeaderAddressFrom = 0,
            EHeaderAddressTo,
            EHeaderAddressCc,
            EHeaderAddressBcc,
            EHeaderSubject,
            EHeaderAttachment
            };

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CUniBaseHeader();
        
        /**
        * Adds correct header fields to the view.
        *
        * @since S60 3.2
        */   
        IMPORT_C void AddToViewL();

        /* Removes headers from 
        *
        * @since S60 3.2
        *
        * @param aFlag Selected additional headers.
        *              Combination of EUniFeatureSubject, EUniFeatureCc
        *              and EUniFeatureBcc of enum TUniFeatures.
        *              Note! Do not edit initial value unless you know
        *              what you do. EUniFeatureTo and EUniFeatureFrom must be included.
        */
        IMPORT_C void RemoveFromViewL( TInt aFlags = 0x7fffffff );

        /**
        * Currently focused address field
        *
        * @since S60 3.2
        *
        * @param aFocusedControl currently focused control
        *
        * @return KErrNotFound, or HeaderAddressTo, EHeaderAddressCc,
        *         EHeaderAddressBcc,
        */
        IMPORT_C TInt FocusedAddressField ( CMsgBaseControl* aFocusedControl );
        
        /**
        * Returns specified header address control.
        *
        * @since S60 3.2
        *
        * @param aType Header control type.
        *
        * @return NULL if control was not found. 
        *         Otherwise pointer to correct header object.         
        */
        inline CMsgAddressControl* AddressControl( THeaderFields aType ) const;
        
        /**
        * Returns specified header address control's ID.
        *
        * @since S60 3.2
        *
        * @param aType Header control type.
        *
        * @return TMsgControlId of the specified control
        */
        inline TMsgControlId AddressControlId( THeaderFields aType ) const;
        
        /**
        * Returns subject control.
        *
        * @since S60 3.2
        *
        * @return NULL if control was not found. 
        *         Otherwise pointer to CMsgExpandableControl object.         
        */
        inline CMsgExpandableControl* SubjectControl() const;
        
        /**
        * Returns attachment control.
        *
        * @since S60 3.2
        *
        * @return NULL if control was not found. 
        *         Otherwise pointer to CMsgAttachmentControl object.         
        */
        inline CMsgAttachmentControl* AttachmentControl() const;
             
        /**
        * Returns header variation bit mask.
        *
        * @since S60 3.2
        *
        * @return Combination of bit flags of variation of EUniFeatureSubject, 
        *         EUniFeatureCc, EUniFeatureBcc. 
        *         Note that the flags must be read first using AddHeadersVariationL()
        */
        inline TInt AddHeadersVariation() const;
        
        /**
        * Reads additional headers variation of bit flags of configuration of KMmsuiHeaderSubject, 
        * KMmsuiHeaderCc, KMmsuiHeaderBcc
        *
        * @since S60 3.2
        *
        * @return Combination of bit flags of variation of EUniFeatureSubject, 
        *         EUniFeatureCc, EUniFeatureBcc. 
        */
        IMPORT_C TInt AddHeadersConfigL();

        /**
        * Saves new additional headers selection
        * Currently this function can be called either before updating 
        * controls on the UI or afterwards
        *
        * @since S60 3.2
        *
        * @param aFlag Selected additional headers.
        *              Combination of EUniFeatureSubject, EUniFeatureCc
        *              and EUniFeatureBcc of enum TUniFeatures
        */
        IMPORT_C void SaveAddHeadersConfigL( TInt aAddHeaders );

        /**
        * Returns whether message has recipients.
        * 
        * @since S60 3.2
        *
        * @return  ETrue, header contains recipients
        *          EFalse, header does not include any recipient
        */
        IMPORT_C TBool HasRecipients();
        
        /**
        * Function checks whether component id represents an address control
        * visible on the first slide of the editor.
        * This is to avoid following code:
        * if (  id == EMsgComponentIdTo 
        *    || id == EMsgComponentIdCc
        *    || id == EMsgComponentIdBcc )...
        * 
        * @since S60 3.2
        *
        * @return  ETrue, is an address field
        *          EFalse, is not an address field. If the address field would not be
        *          visible on the first slide, return EFalse.
        */
        IMPORT_C TBool IsAddressControl( TInt aId );                
        
        /**
        * Handles notifying the resource change event to the controls that are owned by the header.
        * 
        * @param aType Resource change type the same way as in corresponding CCoeControl function call.
        */
        IMPORT_C void HandleResourceChange( TInt aType );
        
    protected: // Constructors

        enum
            {
            // For internal use of header class hierarchy
            // This can be whatever bit excluding 
            // EUniFeatureCc, EUniFeatureBcc and EUniFeatureSubject
            EUniFeatureTo = 0x4000000,
            EUniFeatureFrom = 0x8000000,
            EUniFeatureAttachment = 0x10000000
            };

        struct TAddressData
            {
            CMsgExpandableControl*  iControl;
            TBool                   iOwned;
            CUniAddressHandler*     iAddressHandler; // no address handler for subject
            TMsgControlId           iControlType;
            TMsvRecipientTypeValues iRecipientTypeValue;    // from msv api
            };

        /**
        * Hidden C++ default constructor.
        */
        IMPORT_C CUniBaseHeader( CBaseMtm& aMtm,
                                 CMsgEditorView& aView,
                                 RFs& aFs );

        /**
        * Implements adding controls into view
        * 
        * @since S60 3.2
        *
        * @param aReadContent determines whether content is read to the
        *        UI control from MTM.
        *        Reading is not needed, if control is added by user selection
        */
        IMPORT_C void DoAddToViewL( TBool aReadContent );

        /**
        * Checks recipient address types, which the message contains
        * 
        * @since S60 3.2
        *
        * @return combination of EUniFeatureCc and EUniFeatureBcc of enum TUniFeatures
        */
        IMPORT_C TInt MtmAddressTypes();

        /**
        * Add header variation. 
        *
        * @since S60 3.2
        *
        * @return Sets combination of bit flags of variation of EUniFeatureSubject, 
        *         EUniFeatureCc, EUniFeatureBcc to iAddHeadersVariation
        */
        IMPORT_C void AddHeadersVariationL( TBool aIsUpload );

        /**
        * Create UI control for recipient. 
        * 
        * @since S60 3.2
        *
        * @param aData data area for this recipient type
        * @param aRecipientType recipient type
        * @param aReadContent determines whether recipient data is read from
        *        MTM
        */
        virtual void InsertRecipientL( TAddressData&  aData,
                                       THeaderFields  aRecipientType,
                                       TBool          aReadContent) = 0;
        /**
        * Create UI control for from field. Base implementation is empty
        * 
        * @since S60 3.2
        *
        * @param aReadContent determines whether recipient data is read from
        *        MTM
        */
        virtual void InsertFromL( TBool aReadContent );

        /**
        * Creates UI control for subject
        * 
        * @since S60 3.2
        *
        * @param aReadContent determines whether content is read to the
        *        UI control from MTM.
        *        Reading is not needed, if control is added by user selection
        */
        virtual void InsertSubjectL( TBool aReadContent ) = 0;
        
        /**
        * Creates UI control for attachment field
        * 
        * @since S60 3.2
        *
        * @param aReadContent Determines whether content is read to the UI control.
        */
        virtual void InsertAttachmentL( TBool aReadContent ) = 0;

    protected: //Data

        CBaseMtm&               iMtm;
        CMsgEditorView&         iView;
        RFs&                    iFs;
        TInt                    iAddHeadersVariation;
        TInt                    iAddHeadersConfig;
        TAddressData            iHeaders[6];
        
        /// Derived class must remember to use flags EUniFeatureTo, EUniFeatureFrom, too
        TInt                    iAddDelayed;
    };

#include <unibaseheader.inl>

#endif // C_UNIBASEHEADER_H
