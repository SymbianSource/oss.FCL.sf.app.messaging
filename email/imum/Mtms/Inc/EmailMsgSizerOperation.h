/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Class declaration file
*
*/

#ifndef __EMAILMSGSIZEROPERATION_H__
#define __EMAILMSGSIZEROPERATION_H__

#include <eikdialg.h>

#include <msvapi.h> // CMsvOperation
#include <miutmsg.h> // CImEmailMessage
#include <ImumInternalApi.h>        // CImumInternalApi

#include "PROGTYPE.H"

class CEikonEnv;
class CImEmailMessage;
class CImBaseEmailSettings;
class CContactItemFieldSet;
class CImIAPPreferences;

const TInt KSizerOpPriority = CActive::EPriorityStandard;

/**
* TMsvAttachmentNamesArray
*
*/
class TMsvAttachmentNamesArray : public MDesC16Array
    {
    public:
        /**
        * C++ constructor
        * TMsvAttachmentNamesArray
        * @param const CArrayFixFlat<CMsvAttachment>&, attachment list
        */
        TMsvAttachmentNamesArray(
            const CArrayFixFlat<CMsvAttachment>& aAttachmentList );

        /**
        * MdcaCount
        * From MDesC16Array
        * @return TInt, item count
        */
        virtual TInt MdcaCount() const;

        /**
        * MdcaPoint
        * From MDesC16Array'
        * @return TPtrC16, item text
        */
        virtual TPtrC16 MdcaPoint(TInt aIndex) const;

    public:
        const CArrayFixFlat<CMsvAttachment>& iAttachments;
    };


/**
* Progress for email message sizer op.
*
*/
class TEmailMsgSizerProgress
    {
    public:
        /**
        * TEmailMsgSizerProgress
        * C++ constructor
        */
        TEmailMsgSizerProgress()
            : iProgTypeId(EUiProgTypeSizingMsg), iErrorCode(KErrNone) {}        // progtype.h
    public:
        TInt iProgTypeId;
        TInt iErrorCode;
    };

/**
* Email Message Sizer Operation.
* Walks a message tree asynchronously and provides accessors for
* message part sizes after completion.
*/
class CEmailMsgSizerOperation : public CMsvOperation
    {
    public:
        /**
        * NewL
        * @param CMsvSession&, session reference
        * @param TRequestStatus&, request status
        * @param TMsvId, entry id
        * @param TUid, Mtm id
        * @return CEmailMsgSizerOperation*, self pointer
        */
        static CEmailMsgSizerOperation* NewL(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aMsgId,
            TUid aMtm);

        /**
        * ~CEmailMsgSizerOperation
        * Destructor
        */
        virtual ~CEmailMsgSizerOperation();

        /**
        * ProgressL
        * @return TDesC8&, progress
        */
        virtual const TDesC8& ProgressL();

        /**
        * TotalMessageSize
        * Valid after op completes.
        * @return TInt32, message size
        */
        TInt32 TotalMessageSize() const;

        /**
        * BodyTextSizeL
        * Valid after op completes.
        * @return TInt32, body size
        */
        TInt32 BodyTextSizeL() const;

        /**
        * SizeOfAllAttachmentsL
        * Valid after op completes.
        * @return TInt32, attachments size
        */
        TInt32 SizeOfAllAttachmentsL() const;

        /**
        * AttachmentSizeL
        * Valid after op completes.
        * @param TMsvId, attachment id
        * @return TInt32, attachment size
        */
        TInt32 AttachmentSizeL(TMsvId aAttachmentId) const;

        /**
        * HasAttachmentLargerThanL
        * Returns ETrue if there is bigger attachement than aSize found
        * by this operation.
        * @param TInt32, size
        * @return TBool, ETrue if has attachments larger than given size
        */
        TBool HasAttachmentLargerThanL( TInt32 aSize ) const;

    protected:

        /**
        * DoCancel
        * From CActive
        */
        virtual void DoCancel();

        /**
        * RunL
        * From CActive
        */
        virtual void RunL();
    private:

        /**
        * CEmailMsgSizerOperation
        * C++ constructor
        * @param CMsvSession&, session reference
        * @param TRequestStatus&, request status
        * @param TUid, Mtm id
        */
        CEmailMsgSizerOperation(
            CImumInternalApi& aMailboxApi,
            TRequestStatus& aObserverRequestStatus,
            TUid aMtm);

        /**
        * ConstructL
        * Symbian constructor
        * @param TMsvId, message id
        */
        void ConstructL( TMsvId aMsgId );

    private:

        /**
        *
        * @since S60 3.0
        */
        TInt GetSizeL(
             const TInt aIndex,
             MMsvAttachmentManager& aList ) const;

    private:
        CMsvEntry* iMsgEntry;
        CImEmailMessage* iEmailMsg;
        TPckgBuf<TEmailMsgSizerProgress> iProgress;
    };


#endif // __EMAILMSGSIZEROPERATION_H__

