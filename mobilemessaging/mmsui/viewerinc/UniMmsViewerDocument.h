/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*           MMS Viewer Document class
*
*/



#ifndef MMSVIEWERDOCUMENT_H
#define MMSVIEWERDOCUMENT_H


//  INCLUDES
#include <gmxmlparser.h>
#include <gmxmldocument.h>
#include <msvapi.h>                     // CMsvEntry
#include <MsgEditorDocument.h>          // CMsgEditorDocument
#include <mmsclient.h>
#include <unidatamodel.h>               // ->SmilModel()
#include <unismillist.h>
#include <unidatautils.h>
#include <unismilmodel.h>
#include <uniobject.h>
#include "MmsMtmUi.h"                   //  IsUpload()

#include <mmsvattachmentmanager.h>

// CONSTANTS
_LIT( KUniUtilsMbmFile, "uniutils.mbm" );

// MACROS

// DATA TYPES
    
enum TMmsFeatures
{
    EMmsFeatureHelp             = 0x0001,
    EMmsFeatureDrmFull          = 0x0002,
    EMmsFeatureSVK              = 0x0004,
    EMmsFeatureUpload           = 0x0008,
    EMmsFeatureDeliveryStatus   = 0x0010,
    EMmsFeatureAudioMessaging   = 0x0020,
    EMmsFeatureRestrictedReply  = 0x0040
};


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMsvAttachment;
class CMmsClientMtm;
class CMmsMtmUi;
class RFs;


// CLASS DECLARATION

class MMmsDocumentObserver
    {
    public:
        virtual void InitializeReady( TInt aParseResult, TInt aError ) = 0;
    };

/**
*   CMmsViewerDocument, base class for editor and viewer application document
*   classes. The document is part of the Symbian OS application framework.
*/
class CMmsViewerDocument :  public CMsgEditorDocument,
                            public MUniDataModelObserver

    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aApp A CEikApplication reference
        */
        static CMmsViewerDocument* NewL(    CEikApplication& aApp );  
        
        /**
        * Destructor.
        */
        virtual ~CMmsViewerDocument();

    public: // New functions

        /**
        * InitializeL
        * Load attachments info from store to attachment array
        */ 
        void InitializeL( MMmsDocumentObserver& aObserver );

        /**
        * MessageSize
        * Calculates message size according to TS 23.140 v5.5.0.
        * @return size in bytes
        */
        TInt MessageSize( );

        /**
        * Checks whether the message is generated from a received error response.
        * A message is considered "error response message" if it does not have any
        * files but does have an error response text.
        *
        * Notice: This method must not be called before InitializeL.
        *
        * @return ETrue, message is an "error response message"
        *         EFalse, message is an ordinary message.
        */
        TBool ErrorResponseMessage();


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
        * MtmUiL
        * Accessor (Overrides baseeditor)
        * Returns CMmsMtmUi reference correctly casted from CBaseMtmUi
        * @param  None
        * @return Reference to documents CMmsMtmUi
        */
        CMmsMtmUi& MtmUiL();

    public: // From MMsgAttachmentModelObserver

        /**
        * From MMsgAttachmentModelObserver
        */
        RFile GetAttachmentFileL( TMsvAttachmentId aId );
        
    public:
    
        /**
        * ParseFileCompleteL
        * Call back function used to inform client of Parser when RunL function completes
        */
        void ParseFileCompleteL();
 
    public: // inlines

        /**
        * DataModel
        * Accessor.
        * Get handle to container class CUniDataUtils that owns instances 
        * of CMsgMediaResolver, CMmsConformance, CUniDataUtils, CUniObjectList, 
        * CUniSmilList and CUniSmilModelDataModel
        * @return reference to uni data utils
        */
        inline CUniDataModel& DataModel( ) const;

        /**
        * Current slide of the model where document is indexing.
        * @return Index of the current slide.
        */
        inline TInt CurrentSlide();

        /**
        * Sets current slide of the message.
        * @param aSlideNum  - Index of the slide.
        */
        inline void SetCurrentSlide( TInt aSlideNum );

        /** 
        * Returns SMIL type of the message.
        * @return Possible return values are:
        *         -EMmsSmil
        *         -ETemplateSmil
        *         -E3GPPSmil
        *         ( - ENoSmil is mapped to EMmsSmil =
        *         ( - EMultipleSmil is mapped to EMmsSmil )
        */
        inline TUniSmilType SmilType();

        /**
        * SmilModel
        * Accessor.
        * Get handle to smilmodel
        * @return reference to smilmodel
        */
        inline CUniSmilModel* SmilModel() const;

        /**
        * DataUtils
        * Accessor.
        * Get handle to to msicellaneous utility functions of CUnitDataUtils
        * @return reference to uni data utils
        */
        inline CUniDataUtils* DataUtils() const;
        
        /**
        * Returns object list owned by the model. Object list includes all
        * attachments that are referenced from the SMIL part (or if there is no
        * SMIL part attachments that are of supported media type).
        *
        * @return reference to the object list.
        */
        inline CUniObjectList* ObjectList() const;

        /**
        * Returns attachment list owned by the model. Attachment list includes all 
        * attachments that are NOT referenced from the SMIL part.
        *
        * @return reference to the attachment list.
        */
        inline CUniObjectList* AttachmentList() const;

        /**
        * Returns SMIL list owned by the model. SMIL list includes all 
        * attachments that are of type "application/smil".
        *
        * @return reference to the SMIL attachment list.
        */
        inline CUniSmilList* SmilList() const;

        /*
        * Accessor
        *
        * @return creation mode
        */
        inline TInt CreationMode() const;

        /*
        * Accessor
        *
        * @return maximum message size in bytes
        */
        inline TUint32 MaxMessageSize() const;

        /*
        * Accessor
        *
        * @return parse result from CUniSmilModel::ParseL()
        */
        inline TInt ParseResult() const;

        /**
        * Check if the message is an "MMS Upload" message
        *
        * @return ETrue, message is an "MMS Upload" message
        *         EFalse, otherwise
        */        
        inline TBool IsUpload() const;
        
        inline RFile GetAttachmentFileL( CUniObject& aUniObject );
        
        /* 
        * New function added to avoid problem in deleting message
        * with drm protected image.
        * Be careful to call this function. Viewer application should
        * be in the very end of its life span. 
        */
        inline void DeleteModel( );        

    protected:  // From CMsgEditorDocument

        /**
        * Creates new attachment model object.
        * From CMsgEditorDocument
        */
        CMsgAttachmentModel* CreateNewAttachmentModelL( TBool aReadOnly );
        
        /**
        * CreateNewL
        * From CMsgEditorDocument
        * Needs to be implemented but not in use in mms viewer
        */
        TMsvId CreateNewL(TMsvId aService, TMsvId aDestFolder);
        
    protected:  // New functions

        /**
        * C++ constructor.
        * @param aApp A CEikApplication reference
        */
        CMmsViewerDocument( CEikApplication& aApp );
                
        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Creates a new CMmsTestViewreAppUi
        * @return a pointer to CMmsViewerAppUi
        */
        CEikAppUi* CreateAppUiL();
        
        /**
        * To prevent default-constructor...
        */
        CMmsViewerDocument( );

        /***
        * From MUniDataModelObserver
        */
        void RestoreReady( TInt aParseResult, TInt aError );

    private: // New Functions
    
    protected:  // data

        RFs                 iFs;
        CUniDataModel*      iDataModel;
        
        TUniSmilType        iSmilType;
        TInt                iCurrentSlide;
        TUint32             iMaxMessageSize;
        TInt                iCreationMode;
        TInt                iOrigAttachmentCount;

        MMmsDocumentObserver* iObserver;
        TInt                iParseResult;
};

#include "UniMmsViewerDocument.inl"

#endif   // MMSVIEWERDOCUMENT_H

// End of file
