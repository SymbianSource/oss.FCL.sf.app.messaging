/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Progress reporter mixin.
*
*/



#ifndef __PRGREPORTER_H__
#define __PRGREPORTER_H__


//  INCLUDES
#include <mtmuibas.h>
#include <e32std.h>


// FORWARD DECLARATIONS
class CMtmStore;
class TCoeHelpContext;


// CLASS DECLARATION
/**
* MMsvProgressDecoder
* Pure virtual class to decode progress of the operation.
* Used by CMsvProgressReporterOperation to decode progress and display it.
*/
class MMsvProgressDecoder
    {
    public:
        /**
        * decode progress
        * @param aProgress: progress buffer
        * @param aReturnString: String to be displayed to the user usually in the CMsvWrappableProgressDialog dialog.
        * @param aTotalEntryCount: total entries to be processes
        * @param aEntriesDone: entries processes so far
        * @param aCurrentEntrySize: total size of the entries to be processes
        *           used to display progress bar (for example mail size to be fetched is 42000 bytes)
        *           If for example deleting messges then this should be number of messages to be deleted
        * @param aCurrentBytesTrans: number of bytes received so far
        *           used to display progress bar (for example received bytes is 21000 bytes, then progress bar half done)
        *           If for example deleting messges then this should be number of messages to deleted so far
        * @param aInternal: used only in CImapConnectionOp
        */
        virtual TInt DecodeProgress (
            const TDesC8& aProgress,
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString,
            TInt& aTotalEntryCount,
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize,
            TInt& aCurrentBytesTrans, TBool aInternal ) = 0;
    };

/**
* MMsvProgressReporter
* Pure virtual class to display progress of the operations.
* class CMsvProgressReporterOperation implement these functions.
*/
class MMsvProgressReporter
    {
    public:
        /**
        * Make progress dialog visible or not visible
        * @param aVisible: ETrue to display progress dialog, EFalse hide progress dialog
        */
        virtual void MakeProgressVisibleL( TBool aVisible ) = 0;

        /**
        * Set title
        * @aTitle: text to be displayed
        */
        virtual void SetTitleL( const TDesC& aTitle ) = 0;

        /**
        * Set title
        * @aTitleResourceId: resource id of the text to be displayed
        */
        virtual void SetTitleL( TInt aTitleResourceId ) = 0;

        /**
        * return mtm store.
        */
        virtual CMtmStore& MtmStore() = 0;

        /**
        * Set flag that this operation can be cancelled or not
        * @param aCanCancel: ETrue if this operation can be cancelled by user (Cancel button visible)
        *                    EFalse if this operation cannot be cancelled by user (Cancel button not visible)
        */
        virtual void SetCanCancelL( TBool aCanCancel ) = 0;

        /**
        * Set progress decoder.
        * @param aDecoder: reference to progress decoder.
        */
        virtual void SetProgressDecoder( MMsvProgressDecoder& aDecoder ) = 0;

        /**
        * Remove progress decoder.
        */
        virtual void UnsetProgressDecoder() = 0;

        /**
        * not used
        */
        virtual void SetHelpContext( const TCoeHelpContext& aContext ) = 0;

        /**
        * not used
        */
        virtual void SetAnimationL( TInt aResourceId ) = 0;
    };

#endif //__PRGREPORTER_H__

// End of file
