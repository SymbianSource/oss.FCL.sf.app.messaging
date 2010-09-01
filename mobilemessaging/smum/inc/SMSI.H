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
*     Sms Mtm Ui Data.
*
*/



#if !defined(__SMSI_H__)
#define __SMSI_H__

#if !defined(__MTUDBAS_H__)
#include <mtudcbas.h>                    // CBaseMtmUiData
#endif

//  CLASS DEFINITIONS

/**
*  CSmsMtmUiData
*  Inherited from CBaseMtmUiData
*/
NONSHARABLE_CLASS( CSmsMtmUiData ) : public CBaseMtmUiData
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CSmsMtmUiData* NewL(CRegisteredMtmDll& aRegisteredDll);

        /**
        * Destructor.
        */    
        virtual ~CSmsMtmUiData();

    protected: // Functions from base classes
    
        /**
        * From CBaseMtmUiData
        * service: set aStateFlags & EMtudEntryStateOpen if service includes
        *   unread messages, otherwise aStateFlags should be 0
        * @return array of two bitmaps: first icon and second mask for the icon
        */
        virtual const CBitmapArray& ContextIcon(const TMsvEntry& aContext, TInt aStateFlags) const;

        /**
        * From CBaseMtmUiData
        * returns 0 if operation *IS* supported, 
        * otherwise R_EIK_TBUF_NOT_AVAILABLE
        */
        virtual TInt OperationSupportedL(TInt aOperationId, const TMsvEntry& aContext) const;
    
        /**
        * From CBaseMtmUiData
        */
        virtual TInt QueryCapability(TUid aFunctionId, TInt& aResponse) const;
    
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
        virtual TBool CanDeleteFromEntryL(
            const TMsvEntry& aContext, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanDeleteServiceL(
            const TMsvEntry& aService, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanReplyToEntryL(
            const TMsvEntry& aContext, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanForwardEntryL(
            const TMsvEntry& aContext, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanEditEntryL(
            const TMsvEntry& aContext, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanViewEntryL(
            const TMsvEntry& aContext, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanOpenEntryL(
            const TMsvEntry& aContext, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanCloseEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanCopyMoveToEntryL(
            const TMsvEntry& aContext, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanCopyMoveFromEntryL(
            const TMsvEntry& aContext, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        * @param aReasonResourceId set to 0 if can do operation,
        *    otherwise R_EIK_TBUF_NOT_AVAILABLE
        * @return ETrue if can do operation, EFalse otherwise
        */
        virtual TBool CanCancelL( 
            const TMsvEntry& aContext, 
            TInt& aReasonResourceId ) const;
        
        /**
        * From CBaseMtmUiData
        */
        virtual HBufC* StatusTextL( const TMsvEntry& aContext ) const;

        /**
        * From CBaseMtmUiData
        */
        virtual void PopulateArraysL();

        /**
        * From CBaseMtmUiData
        */
        virtual void GetResourceFileName( TFileName& aFileName ) const;

    protected: // Constructors
        
        /**
        * By default Symbian OS constructor is private.
        */
        virtual void ConstructL();

        /**
        * Constructor (accessible only through factory function)
        */
        CSmsMtmUiData(CRegisteredMtmDll& aRegisteredDll);

        /**
        * Creates skinned icons. Fallbacks are normal icons
        * @param aNumZoomStates: amount of elements in vector
        *        ( bitmap & bitmapmask )
        */
        void CreateSkinnedBitmapsL( TInt aNumZoomStates );
    
	};

#endif // __SMSI_H__



//  End of File
