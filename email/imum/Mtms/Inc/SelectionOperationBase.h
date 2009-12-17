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
*       Base class for operations which perform the same function on
*       all entries of a CMsvEntrySelection
*
*/

#ifndef __SELECTIONOPERATIONBASE_H__
#define __SELECTIONOPERATIONBASE_H__

#include <msvapi.h>

//  Forward declarations
class CMsvEntrySelection;
class CMsvEntry;

/**
* CSelectionOperationBase
* Base class for operations which perform the same function on
* all entries of a CMsvEntrySelection
*/
class CSelectionOperationBase : public CMsvOperation
    {
    public:

        /**
        *
        */
        CSelectionOperationBase(
            CImumInternalApi& aMailboxApi,
            TInt aPriority,
            TRequestStatus& aObserverRequestStatus,
            CMsvEntrySelection* aEntrySelection );

        /**
        *
        */
        CSelectionOperationBase();

        /**
        *
        */
        inline void SetSelectionOwnershipExternal( TBool aExternalOwnership );
        /**
        *
        */
        inline TBool SelectionOwnershipExternal() const;

        /**
        *
        */
        inline TInt NumItems() const;

        /**
        *
        */
        inline TInt CurrentItemIndex() const;

        /**
        * Should be called in ConstructL of derived class
        * before SetActive();
        */
        void StartL();

    protected:
        /**
        * Destructor
        */
        ~CSelectionOperationBase();

        /**
        *
        */
        void Complete(TInt aCompletionCode);

        /**
        * From CActive
        */
        virtual void DoCancel();

    private:
        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
        void DoRunL();

        /**
        * From CActive
        */
		virtual TInt RunError(TInt aError);                       

        /**
        *
        */
        virtual CMsvOperation* CreateOperationL( TMsvId aMsvId )=0;

        /**
        *
        */
        virtual TBool StopOnError( TInt aErrorCode )=0;

        /**
        * Called if DoRunL leaves.
        */
        virtual void OpRunError( TInt aErrorCode ) = 0;
        /**
        * Get error from progress buffer.
        */
        virtual TInt CheckForError(const TDesC8& aProgress)=0;

    protected:
        CMsvOperation* iOperation;
    private:
        CMsvEntrySelection* iEntrySelection;
        TInt iCurrentEntryIndex;
        TBool iSelectionOwnershipExternal;
    };

#include "SelectionOperationBase.inl"

#endif //__SELECTIONOPERATIONBASE_H__
