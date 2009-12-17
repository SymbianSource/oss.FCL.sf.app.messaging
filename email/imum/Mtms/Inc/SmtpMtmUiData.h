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
*       SMTP MTM Ui data layer base API
*
*/


#ifndef __SMTPMTMUIDATA_H__
#define __SMTPMTMUIDATA_H__

#include <msvstd.h>
#include <mtudcbas.h>

/**
* CSmtpMtmUiData - SMTP MTM Ui data layer base API
*/
class CSmtpMtmUiData : public CBaseMtmUiData
    {
    public:

        /**
        * Two phase constructor
        */
        static CSmtpMtmUiData* NewL(CRegisteredMtmDll& aRegisteredDll);

        /**
        * Destructor
        */
        virtual ~CSmtpMtmUiData();

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
        virtual TBool CanDeleteFromEntryL(
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
        virtual HBufC* StatusTextL(const TMsvEntry& aContext) const;

    protected:
        /**
        * Creates skinned icons. Fallbacks are normal icons
        * @since S60 2.8
        * @param aNumZoomStates: amount of elements in vector
        *        ( bitmap & bitmapmask )
        */
        void CreateSkinnedBitmapsL( TInt aNumZoomStates );

    private:

        /**
        * C++ constructor
        */
        CSmtpMtmUiData(CRegisteredMtmDll& aRegisteredDll);

        /**
        * Checks that entry is smtp entry
        * @param aContext: entry to be checked
        * @return ETrue if entry ok, EFalse otherwise
        */
        TBool CheckEntry(const TMsvEntry& aContext) const;

        /**
        * From CBaseMtmUiData
        */
        virtual void PopulateArraysL();

        /**
        * From CBaseMtmUiData
        */
        virtual void GetResourceFileName(TFileName& aFileName) const;
    
    private: // Data
        
        // FF Email feature variation
        TBool iFFEmailVariation;
    };

#endif // __SMTPMTMUIDATA_H__
