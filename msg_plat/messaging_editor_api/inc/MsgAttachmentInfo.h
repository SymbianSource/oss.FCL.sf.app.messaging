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
* Description:  MsgAttachmentInfo  declaration
*
*/



#ifndef CMSGATTACHMENTINFO_H
#define CMSGATTACHMENTINFO_H

// ========== INCLUDE FILES ================================

#include <apmstd.h>
#include <msvstd.h>

#include <cmsvattachment.h>

// ========== CONSTANTS ====================================

// Farsi takes 14 bytes when attachment has > 1000 bytes
const TInt KMsgAttaSizeStringLength = 32;

// ========== MACROS =======================================

// ========== DATA TYPES ===================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================

class CGulIcon;
class CMsgAttachmentModel;

// ========== CLASS DECLARATION ============================

/**
* Attachment information node.
*/
class CMsgAttachmentInfo : public CBase
    {
    public:
        
        enum TDRMDataType
            {
            ENoLimitations = 0,
            EForwardLockedOrCombinedDelivery,
            ESeparateDeliveryValidRights,
            ECombinedDeliveryInvalidRights,
            ESeparateDeliveryInvalidRights
            };
        
        /**
        * Factory method that creates this object.
        * @param aAttachmentModel
        * @param aFileName
        * @param aSize
        * @param aFetched
        * @param aAttachedThisSession
        * @param aAttachmentId
        * @param aDataType
        * @param aDRMDataType
        * @return
        */
        static CMsgAttachmentInfo* NewL( CMsgAttachmentModel& aAttachmentModel,
                                         const TFileName& aFileName,
                                         TInt aSize,
                                         TBool Fetched,
                                         TBool aAttachedThisSession,
                                         TMsvAttachmentId aAttachmentId, 
                                         const TDataType& aDataType,
                                         TDRMDataType aDRMDataType );
        
        /**
        * Destructor.
        */
        IMPORT_C ~CMsgAttachmentInfo();

        /**
        * Returns reference to the filename this attachment represents.
        * @return
        */
        IMPORT_C const TFileName& FileName() const;

        /**
        * Set the filename of this attachment.
        * @param aFileName
        */
        IMPORT_C void SetFileName( const TFileName& aFileName );

        /**
        * Returns the size of this attachment.
        * @return
        */
        IMPORT_C TInt Size() const;

        /**
        * Returns the size of this attachment as a string.
        * @return
        */
        IMPORT_C TBuf<KMsgAttaSizeStringLength> SizeString() const;

        /**
        * Returns the id of this attachment.
        * @return
        */
        IMPORT_C TMsvAttachmentId AttachmentId() const;

        /**
        * Sets the id of this attachment.
        * @param aId
        */
        IMPORT_C void SetAttachmentId( TMsvAttachmentId aId );

        /**
        * Returns ETrue if this attachment is fetched.
        * @return
        */
        IMPORT_C TBool IsFetched() const;

        /**
        * Sets the fetched attribute of this attachment.
        * @param aFetched
        */
        IMPORT_C void SetFetched( TBool aFetched );

        /**
        * Returns the datatype (MIME type) of this attachment.
        * @return
        */
        IMPORT_C const TDataType& DataType() const;

        /**
        * Sets the datatype (MIME type) of this attachment.
        * @param aDataType
        */
        IMPORT_C void SetDataType( const TDataType& aDataType );

        /**
        * Returns ETrue if this attachment was attached in this session.
        * @return
        */
        IMPORT_C TBool IsAttachedThisSession() const;

        /**
        * Sets flag which tells if this attachment is attached in this session.
        * @param aFlag
        */
        IMPORT_C void SetAttachedThisSession( TBool aFlag );

        /**
        * Returns supported flag.
        * @return
        */
        IMPORT_C TBool IsSupported() const;

        /**
        * Sets supported flag.
        * @param aFlag
        */
        IMPORT_C void SetSupported( TBool aFlag );

        /**
        * Get icon of this attachment.
        * @return
        */
        IMPORT_C CGulIcon* Icon() const;

        /**
        * Sets flag if separator line must be drawn before this attachment
        * in attachment view.
        * @param aFlag
        */
        IMPORT_C void SetSeparator( TBool aFlag );

        /**
        * Return separator flag.
        * @return
        */
        IMPORT_C TBool IsSeparator() const;

        /**
        * Sets the DRM (digital rights management) status of this attachment.
        * @return aDRMDataType
        */
        IMPORT_C void SetDRMDataType( TDRMDataType aDRMDataType );

        /**
        * Returns the DRM (digital rights management) status of this attachment.
        * @return
        */
        IMPORT_C TInt DRMDataType() const;
        
        /**
        * Saves status whether this attachment has been saved into file system
        * @param aSaved ETrue attachment has been saved
        *               EFalse attachment has not been saved
        */
        IMPORT_C void SetSaved( TBool aSaved );
        
        /**
        * Returns status information whether this attachment has been saved into file system
        * @param aSaved ETrue attachment has been saved
        *               EFalse attachment has not been saved
        */
        IMPORT_C TBool IsSaved() const;
        
        /**
        * Sets character set for text attachment.
        * @param aSaved ETrue attachment has been saved
        *               EFalse attachment has not been saved
        */
        inline void SetCharacterSet( TInt aCharConvCharSet );
        
        /**
        * Returns character set of text attachment.
        * @return character set
        */
        inline TInt CharacterSet() const;
        
        /**
        * Sets attachment empty ( i.e. no content is connected to the attachment ).
        * @param aSaved ETrue attachment has been saved
        *               EFalse attachment has not been saved
        */
        inline void SetEmptyAttachment( TBool aValue );
        
        /**
        * Returns whether attachment is empty.
        * @return 
        */
        inline TBool IsEmptyAttachment();
        
    protected:

        /**
        * Constructor.
        * @param aAttachmentModel
        * @param aFileName
        * @param aSize
        * @param aFetched
        * @param aAttachedThisSession
        */
        IMPORT_C CMsgAttachmentInfo( CMsgAttachmentModel& aAttachmentModel,
                                     const TFileName& aFileName,
                                     TInt aSize,
                                     TBool aFetched,
                                     TBool aAttachedThisSession);
        
        /**
        * 2nd phase constructor.
        * @param aDataType
        * @param aDRMDataType
        */
        IMPORT_C void ConstructL( const TDataType& aDataType,
                                  TDRMDataType aDRMDataType );

    public:
        
        /**
        * Updates size and icon.
        */
        void ChangeSizeAndIconL();

    private:
    
        /**
        * Performs icon updating.
        */
        void DoUpdateIconL();
        
    private:
        
        enum TAttachmentInfoFlags
            {
            EAttachmentNoFlagsSet   = 0x00000000,
            EAttachmentFetched      = 0x00000001,        
            EAttachedThisSession    = 0x00000002,
            EAttachmentSupported    = 0x00000004,
            ESeparatorBefore        = 0x00000008,
            EAttachmentSaved        = 0x00000010,
            EEmptyAttachment        = 0x00000020
            };
        
        CMsgAttachmentModel&            iAttachmentModel;
        TFileName                       iFileName;
        TInt                            iSize;
        TMsvAttachmentId                iAttachmentId;
        TDataType                       iDataType;        
        TDRMDataType                    iDRMDataType;
        TBuf<KMsgAttaSizeStringLength>  iSizeString;
        CGulIcon*                       iIcon;
        TInt                            iCharset;
        TInt                            iAttachmentFlags;
    };

#include "MsgAttachmentInfo.inl"

#endif // CMSGATTACHMENTINFO_H

// End of File
