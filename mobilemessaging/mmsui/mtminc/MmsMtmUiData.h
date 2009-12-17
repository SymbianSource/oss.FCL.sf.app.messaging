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
*       CMmsMtmUiData - UI DATA part of the MMS MTM type to Symbian OS Messaging
*
*/



#ifndef MMSMTMUIDATA_H
#define MMSMTMUIDATA_H

//  INCLUDES
#include <mtudcbas.h>
#include <MtmExtendedCapabilities.hrh>


// CLASS DECLARATION

/**
 * CMmsMtmUiData - UI DATA part of the MMS MTM type to Symbian OS Messaging
 *
 * @since 0.9
 */
class CMmsMtmUiData :public CBaseMtmUiData
    {
    public:  // Constructors and destructor
        
        /**
         * Symbian OS constructor
         * @param aRegisteredMtmDll registry dll
         * @return a new MMS MTM UI object.
         */
        static CMmsMtmUiData* NewL(CRegisteredMtmDll& aRegisteredDll);
        
        /**
         * Destructor.
         */
        ~CMmsMtmUiData();

    public: // Functions from base classes

        /**
         * From CBaseMtmUiData - can the given operation be performed for the given entry
         */
        TInt OperationSupportedL(TInt aOperationId, const TMsvEntry& aContext) const;
        
        /**
         * From CBaseMtmUiData Capabilities - defines what we can do
         * @param aFunctionId asked capability
         * @param aResponse response depends on the capability
         * @return KErrNone if supported, otherwise KErrNotSupported
         */
        TInt QueryCapability(TUid aFunctionId, TInt& aResponse) const;

        /**
         * From CBaseMtmUiData
         */
        HBufC* StatusTextL(const TMsvEntry& aContext) const;

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanCreateEntryL(const TMsvEntry& aParent, 
            TMsvEntry& aNewEntry, 
            TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData
         * Check whether it is ok to reply to a message
         */
        TBool CanReplyToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanForwardEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;   

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanEditEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanViewEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanOpenEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanCloseEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanDeleteFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanDeleteServiceL(const TMsvEntry& aService, TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanCopyMoveToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanCopyMoveFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;  

        /**
         * From CBaseMtmUiData - not supported in Series 60
         */
        TBool CanCancelL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;

        /**
         * From CBaseMtmUiData Icons for the entry having context.
         * @param aContext entry in question
         * @param aStateFlags
         * @return array of bitmaps
         */
        const CBitmapArray& ContextIcon(const TMsvEntry& aContext, TInt aStateFlags) const;

    private:

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL();

        /**
         * C++ constructor
         * @param aRegisteredMtmDll registry dll
         */
        CMmsMtmUiData( CRegisteredMtmDll& aRegisteredDll );

        /**
         * Populate icon array
         */
        void PopulateArraysL();

        /**
         * Gives the resource file
         * @param storage for the filename.
         */
        void GetResourceFileName( TFileName& aFileName ) const;

        /**
         * 
         * 
         */
        void CreateSkinnedBitmapsL(
            const TDesC& aBitmapFile,
            TInt aStartBitmap,
            TInt aEndBitmap );
            
    private:
        TBool   iDeliveryStatusSupported;
        
   };

#endif      // MMSMTMUIDATA_H
            
// End of File
