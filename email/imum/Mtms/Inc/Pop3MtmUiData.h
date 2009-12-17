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
*       POP3 MTM Ui data layer base API
*
*/

#ifndef __POP3MTMUIDATA_H__
#define __POP3MTMUIDATA_H__

#include <msvstd.h>
#include <mtudcbas.h>

/**
* CPop3MtmUiData - POP3 MTM Ui data layer base API
*/
class CPop3MtmUiData : public CBaseMtmUiData
    {
    public:
        /**
        * Two phase constructor
        */
        static CPop3MtmUiData* NewL(CRegisteredMtmDll& aRegisteredDll);

        /**
        * Destructor
        */
        virtual ~CPop3MtmUiData();

    private:
        /**
        * C++ constructor
        */
        CPop3MtmUiData(CRegisteredMtmDll& aRegisteredDll);

        /**
        * Symbian OS constructor
        */
        void ConstructL();

        /**
        * From CBaseMtmUiData
        */
        virtual TInt OperationSupportedL(
            TInt aOperationId,
            const TMsvEntry& aContext) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TInt QueryCapability(
            TUid aCapability,
            TInt& aResponse) const;

        /**
        * From CBaseMtmUiData
        */
        virtual const CBitmapArray& ContextIcon(
            const TMsvEntry& aContext,
            TInt aStateFlags) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanCreateEntryL(
            const TMsvEntry& aParent,
            TMsvEntry& aNewEntry,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanDeleteFromEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanUnDeleteFromEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanDeleteServiceL(
            const TMsvEntry& aService,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanReplyToEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanForwardEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanEditEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanViewEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanOpenEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanCloseEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanCopyMoveToEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanCopyMoveFromEntryL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual TBool CanCancelL(
            const TMsvEntry& aContext,
            TInt& aReasonResourceId) const;

        /**
        * From CBaseMtmUiData
        */
        virtual void PopulateArraysL();

        /**
        * From CBaseMtmUiData
        */
        virtual void GetResourceFileName(TFileName& aFileName) const;

        /**
        * From CBaseMtmUiData
        */
        virtual HBufC* StatusTextL(const TMsvEntry& aContext) const;

        /**
        *
        */
        TBool CheckEntry(const TMsvEntry& aEntry) const;

        /**
        *
        */
        void CreateSkinnedBitmapsL( TInt aNumZoomStates );

    private: // Data
        CBitmapArray* iLastResortIcon;

        enum TIconArrayOrder
            {
            EImumPropMceRemoteSub = 0,
            EImumPropMceRemoteNewSub = 2,
            EImumPropMceMailFetReaDel = 4,
            EImumPropMceMailFetRead = 6,
            EImumPropMceMailFetUnread = 8,
            EImumPropMceMailUnfetRead = 10,
            EImumPropMceMailUnfetUnread = 12
            };
    };

#endif
