/*
* Copyright (c) 1998 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  bioi  declaration
*
*/



#if !defined(__BIOI_H__)
#define __BIOI_H__

#include <msvstd.h>
#include <mtudcbas.h>

////////////////////////////////////////////////////////
// CBioMtmUiData - BIO  MTM UI data layer 
////////////////////////////////////////////////////////
NONSHARABLE_CLASS(CBioMtmUiData) : public CBaseMtmUiData
	{
public:
	// --- Construction/destruction ---
	static CBioMtmUiData* NewL(CRegisteredMtmDll& aRegisteredDll);
	virtual ~CBioMtmUiData();

protected:
	// --- Construction/destruction ---
	CBioMtmUiData(CRegisteredMtmDll& aRegisteredDll);
	virtual void ConstructL();

	// 
	// --- Function queries ---
	virtual TInt OperationSupportedL(TInt aOperationId, const TMsvEntry& aContext) const;
	//	
	// --- Capability checks ---
	virtual TInt QueryCapability(TUid aCapability, TInt& aResponse) const;
	//
	// --- Context icons ---
	virtual const CBitmapArray& ContextIcon(const TMsvEntry& aContext,TInt aStateFlags) const;
	//
	// --- Provide context-specific function information ---
	virtual TBool CanCreateEntryL(const TMsvEntry& aParent, TMsvEntry& aNewEntry, TInt& aReasonResourceId) const;
	virtual TBool CanDeleteFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	virtual TBool CanDeleteServiceL(const TMsvEntry& aService, TInt& aReasonResourceId) const;
	virtual TBool CanReplyToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	virtual TBool CanForwardEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	virtual TBool CanEditEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	virtual TBool CanViewEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	virtual TBool CanOpenEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	virtual TBool CanCloseEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	virtual TBool CanCopyMoveToEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	virtual TBool CanCopyMoveFromEntryL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	virtual TBool CanCancelL(const TMsvEntry& aContext, TInt& aReasonResourceId) const;
	//
	// --- From CBaseMtmUiData ---
	virtual void PopulateArraysL();
	virtual void GetResourceFileName(TFileName& aFileName) const;
	virtual HBufC* StatusTextL(const TMsvEntry& aContext) const;
	//
	// --- Private utility functions ---
	virtual TBool CheckEntry(const TMsvEntry& aEntry) const;

private: // new methods

	/**
     * Creates the needed icons
	 *
	 * @param aNumZoomStates the number of zoom states to be used
     */
	void CreateSkinnedBitmapsL( TInt aNumZoomStates );

	};

#endif 
