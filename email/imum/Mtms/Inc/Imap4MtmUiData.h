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
*       IMAP4 MTM Ui data layer base API
*
*/

#ifndef __IMAP4MTMUIDATA_H__
#define __IMAP4MTMUIDATA_H__

#include <msvstd.h>
#include <mtudcbas.h>

class TAknsItemID;
/**
* CImap4MtmUiData - IMAP4 MTM Ui data layer base API
*/
class CImap4MtmUiData : public CBaseMtmUiData
    {
    public:

        /**
        *
        */
        static CImap4MtmUiData* NewL(CRegisteredMtmDll& aRegisteredDll);

        /**
        * Destructor
        */
        virtual ~CImap4MtmUiData();

    private:

        /**
        * C++ constructor
        */
        CImap4MtmUiData(CRegisteredMtmDll& aRegisteredDll);

        /**
        * Symbian OS constructor
        */
        void ConstructL();

        /**
        * From CBaseMtmUiData
        * returns 0 if operation *IS* supported,
        * otherwise R_EIK_TBUF_NOT_AVAILABLE
        */
        virtual TInt OperationSupportedL(
            TInt aOperationId,
            const TMsvEntry& aContext) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TInt QueryCapability(TUid aCapability, TInt& aResponse) const;

        /**
        * From CBaseMtmUiData
        * service: set aStateFlags & EMtudEntryStateOpen if service includes
        *   unread messages, otherwise aStateFlags should be 0
        * @return array of two bitmaps: first icon and second mask for the icon
        */
        virtual const CBitmapArray& ContextIcon(
            const TMsvEntry& aContext,
            TInt aStateFlags) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanCreateEntryL(
            const TMsvEntry& aParent,
            TMsvEntry& aNewEntry,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanReplyToEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanForwardEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanEditEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanViewEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanOpenEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanCloseEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanDeleteFromEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanUnDeleteFromEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanCopyMoveToEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanCopyMoveFromEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanDeleteServiceL(
            const TMsvEntry& aService,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanCancelL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual HBufC* StatusTextL(const TMsvEntry& aContext) const;

        /**
        * From CBaseMtmUiData
        */
        virtual void PopulateArraysL();

        /**
        * From CBaseMtmUiData
        */
        virtual void GetResourceFileName(TFileName& aFileName) const;

        /**
        * Checks that entry is imap4 type
        */
        TBool CheckEntry(const TMsvEntry& aContext) const;

        /**
        *
        */
        void CreateSkinnedBitmapsL( TInt aNumZoomStates );

    private:
        CBitmapArray* iLastResortIcon;

        enum TIconArrayOrder
            {
            EImumPropMceRemoteSub = 0,
            EImumPropMceRemoteNewSub = 2,
            EImumPropMceMailFetReaDel = 4,
            EImumPropMceMailFetRead = 6,
            EImumPropMceMailFetUnread = 8,
            EImumPropMceMailUnfetRead = 10,
            EImumPropMceMailUnfetUnread = 12,
            EImumPropFolderSmall = 14,
            EImumPropFolderSubs = 16,
            EImumPropFolderSubsNew = 18,
            EImumPropFolderSubSmall = 20,
            EImumPropFolderSubSubs = 22,
            EImumPropFolderSubSubsNew = 24,
            EImumPropFolderUnsubsNew = 26,
            EImumPropFolderSubUnsubsNew = 28
            };

    };

#endif // __IMAP4MTMUIDATA_H__
