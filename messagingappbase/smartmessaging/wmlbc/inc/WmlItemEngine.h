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
*     Item Engine owns the setting items.
*
*/



#ifndef CWMLITEMENGINE_H
#define CWMLITEMENGINE_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CParsedField;
class CWmlSubItemBase;
class CAknWaitDialog;
class MProgressDialogCallback;

// CLASS DECLARATION

/**
* CWmlItemEngine owns the individual setting items.
* It provides methods for manipulating them.
*/
NONSHARABLE_CLASS( CWmlItemEngine )
: public CBase
    {
    public: // construction / destruction

        /**
        * First stage constructor.
        */
        static CWmlItemEngine* NewL();

        /**
        * Destructor.
        */  
        ~CWmlItemEngine();
    
    public:

        /**
        * Create the item arrays.
        * @param aParseFields reference to the parsedfields array.
        */
        void CreateItemArraysL( const CArrayPtrSeg<CParsedField>& aParsedFields );

    public:
                
      
        /**
        * Get the bookmark count.
        * @return Number of Bookmark setting items.
        */
        TInt BookmarkCount() const;

        /**
        * Get the Current item.
        * @return reference to the current item.
        */
        CWmlSubItemBase& CurrentItem() const;

        /**
        * Set the current item.
        * @param aIndex index of the current item.
        */
        void SetCurrentItem( const TInt aIndex );

        void SaveCurrentItemL();

    private:
        
        /**
        * The second phase constructor.
        */        
        void ConstructL();

		/**
		* Checks the available disk space.
		* If the current disk space is below critical level
		* leaves with KErrDiskFull
		*/
		void CheckDiskSpaceL();

    private: // Data
        
        /// Pointer to the current item
        CWmlSubItemBase* iCurrentItem;
        
    private:

        /// This arrays own the objects.
        CArrayPtrSeg<CWmlSubItemBase>* iBMArray;

        // This indexes point to to corresponding array.
        TInt iCurrentBMIndex;

        /// Pointer to the itemarray.
        CArrayPtrSeg<CWmlSubItemBase>* iItemArray;

		/// Current operation state
		TInt iOperationState;

        /// Pointer to the wait dialog.
        CAknWaitDialog* iWaitDialog;

		/// Pointer to dialog callback object.
		MProgressDialogCallback* iDialogCallback;
    };

#endif // CWMLITEMENGINE_H

// End of file
