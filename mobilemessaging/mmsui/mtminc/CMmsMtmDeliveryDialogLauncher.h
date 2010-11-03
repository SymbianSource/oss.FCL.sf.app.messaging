/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CMmsMtmDeliveryDialogLauncher  declaration
*
*/



#ifndef DELIVERYDIALOGLAUNCHER_H
#define DELIVERYDIALOGLAUNCHER_H

//  INCLUDES
#include <e32base.h>

#include "MMmsMtmDeliveryDataContainer.h"



class CMsgDeliveryItem;
class CMmsMTMLogData;
class TMmsMsvEntry;
class CMmsClientMtm;
class CMsgInfoDeliveryStatusPopup;

// CLASS DECLARATION

/**
 * 
 *
 * @since 3.0
 */
class CMmsMtmDeliveryDialogLauncher :public CBase, public MMmsMtmDeliveryDataContainer
    {
    public:  // Constructors and destructor

        /**
         * Symbian OS constructor
         * @param none
         * @return a new CMmsMtmDeliveryDialogLauncher object.
         */
        IMPORT_C static CMmsMtmDeliveryDialogLauncher* NewL(TMmsMsvEntry aEntry, CMmsClientMtm* aMmsClient);

        /**
         * Destructor.
         */
        ~CMmsMtmDeliveryDialogLauncher();

    public: // Functions from MMmsMtmDeliveryDataContainer

         /**
         * "Callback" method which receives an data array to be used in the delivery status dialog.
         *
         * @param aDataArray: The data array to be forwarded to the dialog
         */
        void SetDataArrayL(CArrayPtrFlat< CMsgDeliveryItem >* aDataArray);

    public: // new functions

         /**
         * Starts the Delivery status dialog
         */
        IMPORT_C TBool StartDialogL();	

    private:

        /**
         * By default Symbian OS constructor is private.
         */
        void ConstructL(TMmsMsvEntry aEntry, CMmsClientMtm* aMmsClient);

        /**
         * C++ constructor
         * @param aRegisteredMtmDll registry dll
         */
        CMmsMtmDeliveryDialogLauncher( );

    private:
        CMmsMTMLogData* iLogData;
        TMmsMsvEntry iEntry;
        CMmsClientMtm* iMmsClient;
        CMsgInfoDeliveryStatusPopup* iDialog;
    };

#endif      // MMSMTMUIDATA_H
            
// End of File
