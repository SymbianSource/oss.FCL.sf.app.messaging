/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*     Changes message flag.
*
*/



#if !defined(__MCECHANGEFLAGOPERATION_H__)
#define __MCECHANGEFLAGOPERATION_H__


//  INCLUDES
#include <msvapi.h>
#include "MceUtils.h"
#include <MsvPrgReporter.h>


// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  Marks email messages read in remote mailbox.
*/

class CMceRemoveNewFlag : 
    public CMsvOperation,
    public MMsvProgressDecoder
    {
    public:

        enum TRemoveType
            {
            EMceRemoveEntryFlagUnread   = 0x01,
            EMceRemoveEntryFlagNew      = 0x02,
            EMceRestoreEntryFlagRead    = 0x04
            };
        
        /**
        * Constructor.
        */
        static CMceRemoveNewFlag* NewL(
            TUint32 aRemoveFlags,
            CMsvSession& aMsvSession, 
            TRequestStatus& aObserverRequestStatus, 
            CMsvEntrySelection* aSelection );
        /**
        * Destructor.
        */
        virtual ~CMceRemoveNewFlag();

   public: // Functions from base classes    

        /**
        * From CActive
        */
        virtual const TDesC8& ProgressL();

        /**
        * From MMsvProgressDecoder
        */ 
        virtual TInt DecodeProgress (
            const TDesC8& aProgress, 
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString, 
            TInt& aTotalEntryCount, 
            TInt& aEntriesDone,
            TInt& aCurrentEntrySize, 
            TInt& aCurrentBytesTrans, 
            TBool aInternal );

   protected: // Functions from base classes

        /** 
        * From CActive
        */
        void DoCancel();
        /** 
        * From CActive
        */
        void RunL();

   private:
        /**
        * Constructor.
        */
        CMceRemoveNewFlag(
            TUint32 aRemoveFlags,
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aFolderId,
            CMsvEntrySelection* aSelection );

        /**
        * Constructor.
        */
        void ConstructL();      

        /** 
        * Starts next email message handling to remove new/unread flag
        */
        void RemoveNextNewFlagL();        

        /** 
        * Removes new/unread flag from email message
        */
        void MakeNextRemoveL();
        
        /**
        * Conditional item deletion from selection
        * @since Series60 3.0
        * @param aIndex, index in selection
        */
        void DeleteFromSelection( TInt aIndex, TBool aUnread );
        
    private:
        CMsvOperation*          iOperation;
        CMsvEntry*              iEntry;
        TMsvEntry               iNewEntry;
        TMsvId                  iFolderId;
        CMsvEntrySelection*     iEntrySelection;
        TBuf8<1>                iBlank;
        TMceFlags               iRemoveFlags;
        TInt                    iCount;
    };

#endif // __MCECHANGEFLAGOPERATION_H__

// End of file
