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
*     Sms Mtm Ui Data for Cdma
*
*/



#if !defined(__SMSICDMA_H__)
#define __SMSICDMA_H__

#if !defined(__MTUDBAS_H__)
#include <MTUDCBAS.H>                    // CBaseMtmUiData
#endif

#include <FeatMgr.h>                    // FeatureManager
#include <bldvariant.hrh>
#include "Smsi.h"


//  CLASS DEFINITIONS

/**
*  CSmsMtmUiDataCdma
*  Inherited from CSmsMtmUiData
*  Defines actions/data for UI Data elements related to SMS 
*  CDMA Version
*/
NONSHARABLE_CLASS(CSmsMtmUiDataCDMA) : public CSmsMtmUiData
{
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    */
    static CSmsMtmUiDataCDMA* NewL(CRegisteredMtmDll& aRegisteredDll);

    /**
    * Destructor.
    */    
    virtual ~CSmsMtmUiDataCDMA();

    private: // Functions from base classes

	/**
	* From CSmsMtmUiData
	* service: set aStateFlags & EMtudEntryStateOpen if service includes
	*   unread messages, otherwise aStateFlags should be 0
	* @return array of two bitmaps: first icon and second mask for the icon
	*/
	const CBitmapArray& ContextIcon(const TMsvEntry& aContext, TInt aStateFlags) const;

    /**
	* From CSmsMtmUiData
    */
    TInt QueryCapability(TUid aFunctionId, TInt& aResponse) const;

    /**
	* From CSmsMtmUiData
	*/
	void PopulateArraysL();

	/**
    * Creates skinned icons. Fallbacks are normal icons
    * @param aNumZoomStates: amount of elements in vector
    *        ( bitmap & bitmapmask )
    */
    void CreateSkinnedBitmapsL( TInt aNumZoomStates );

private:

		/**
		* Constructor (accessible only through factory function)
		*/
		CSmsMtmUiDataCDMA(CRegisteredMtmDll& aRegisteredDll);

private: // Data 

  };

#endif // __SMSICDMA_H__



//  End of File
