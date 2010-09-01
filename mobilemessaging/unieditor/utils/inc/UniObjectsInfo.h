/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Object info derived from attachment info.
*
*/



#ifndef C_UNIOBJECTINFO_H
#define C_UNIOBJECTINFO_H

// ========== INCLUDE FILES ================================

#include <MsgAttachmentInfo.h>
#include <MsgAttachmentUtils.h>

// ========== CONSTANTS ====================================

// ========== MACROS =======================================

// ========== FUNCTION PROTOTYPES ==========================

// ========== FORWARD DECLARATIONS =========================
class CUniObject;

// ========== DATA TYPES ===================================

// ========== CLASS DECLARATION ============================

/**
*  Object info class derived from attachment info.
*
*  @lib UniUtils.lib
*
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CUniObjectsInfo ) : public CMsgAttachmentInfo
    {
    public:

        enum TUniObjectsInfoFlags
            {
            EMmsSlide     = 0x0001
            };

        /**
        * Factory method that creates this object.
        *
        * @since S60 3.2
        *
        * @param aAttachmentModel Reference to attachment model.
        * @param aObject Object to which info is related.
        * @param aDRMType DRM type of the object
        * @param aAttachmentObject ETrue if attachment object, otherwise EFalse.
        * @param aSlideNumber Slide number where object is located.
        *
        * @return New instance
        */
        static CUniObjectsInfo* NewLC(
            CMsgAttachmentModel& aAttachmentModel,
            CUniObject& aObject,
            TInt aDRMType,
            TBool aAttachmentObject,
            TInt aSlideNumber );

        /**
        * Factory method that creates this object for empty slide.
        *
        * @since S60 3.2
        *
        * @param aAttachmentModel Reference to attachment model.
        * @param aEmptySlideName Name string of the empty slide
        *
        * @return New instance
        */
        static CUniObjectsInfo* NewEmptySlideLC(
            CMsgAttachmentModel& aAttachmentModel,
            const TDesC& aEmptySlideName );
        
        /**
        * Destructor
        *
        * @since S60 3.2
        *
        */
        virtual ~CUniObjectsInfo();
        
        /**
        * Sets type of this object.
        *
        * @since S60 3.2
        *
        * @param aType New type
        */
        inline void SetType( MsgAttachmentUtils::TMsgAttachmentFetchType aType );

        /**
        * Returns type of this object.
        *
        * @since S60 3.2
        *
        * @return object type
        */
        inline MsgAttachmentUtils::TMsgAttachmentFetchType Type() const;

        /**
        * Returns current flags.
        *
        * @since S60 3.2
        *
        * @return iFlags
        */
        inline TUint32 Flags() const;

        /**
        * Sets flags.
        *
        * @since S60 3.2
        *
        * @param aFlags New flags
        */
        inline void SetFlags( TUint32 aFlags );

        /**
        * Get pointer to a related object
        *
        * @since S60 3.2
        */
        inline CUniObject* Object() const;
        
        /**
        * Indicates whether object is attachment.
        *
        * @since S60 3.2
        *
        * @return ETrue     if object is attachment
        *         EFalse    otherwise
        */
        inline TBool AttachmentObject() const;
        
        /**
        * Returns the slide number where object is located.
        *
        * @since S60 3.2
        *
        * @return Slide number on MMS SMIL.
        */
        inline TInt SlideNumber() const;

    protected:

        /**
        * C++ constructor.
        *
        * @since S60 3.2
        *
        * @param aAttachmentModel Reference to attachment model.
        * @param aObject Object to which info is related.
        * @param aAttachmentObject ETrue if attachment object, otherwise EFalse.
        * @param aSlideNumber Slide number where object is located.
        */
        CUniObjectsInfo( CMsgAttachmentModel& aAttachmentModel,
                         CUniObject& aObject,
                         TBool aAttachmentObject,
                         TInt aSlideNumber );

        /**
        * Constructor (for empty slide).
        *        
        * @since S60 3.2
        *
        * @param aAttachmentModel Reference to attachment model.
        * @param aEmptySlideName Name string of the empty slide
        */
        CUniObjectsInfo( CMsgAttachmentModel& aAttachmentModel,
                         const TDesC& aEmptySlideName );

        /**
        * Symbian OS constructor.
        */
        void ConstructL( TInt aDRMType );

    private:

        MsgAttachmentUtils::TMsgAttachmentFetchType iType;
        TUint32 iFlags;
        CUniObject* iObject;
        TBool iAttachmentObject;
        TInt iSlideNumber;
    };

#include <uniobjectsinfo.inl>

#endif // C_UNIOBJECTINFO_H

// End of File
