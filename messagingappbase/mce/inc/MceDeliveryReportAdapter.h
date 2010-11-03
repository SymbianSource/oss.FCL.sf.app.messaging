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
*     This class formats data from engine to listbox
*
*/



#ifndef CMceDeliveryReportAdapter_H
#define CMceDeliveryReportAdapter_H

//  INCLUDES
#include <bamdesca.h>  // MDesCArray
#include <e32base.h>
#include <ConeResLoader.h>
#include <cntdef.h>
#include <logwrap.h>
#include <bldvariant.hrh>

// FORWARD DECLARATIONS
class CMceLogEngine;
class CEikFormattedCellListBox;

class CPNGNumberGrouping;

// CLASS DECLARATION

class CMceDeliveryReportAdapter : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor

        /**
         *  Two-phased constructor.
         */
        static CMceDeliveryReportAdapter* NewL( CMceLogEngine* aModel, CEikFormattedCellListBox *aListBox );

        /**
         *  Destructor.
         */
        virtual ~CMceDeliveryReportAdapter();

    private:
        /**
         *  C++ default constructor.
         */
        CMceDeliveryReportAdapter( CMceLogEngine* aModel, CEikFormattedCellListBox *aListBox );

        /**
         *  Symbian OS constructor.
         */
        void ConstructL();

    private: // from MDesCArray

        /**
         *  Returns number of entries
         *
         *  @return TInt
         */
        TInt MdcaCount() const;

        /**
         *  Returns a TPtrC related with the position given by aIndex
         *
         *  @param  TInt    aIndex
         *  @return TPtrC16 pointer to element
         */
        TPtrC16 MdcaPoint( TInt aIndex ) const;

    private: //data

        /// Ref: Reference to model
        CMceLogEngine* iModel;

        /// Own: Resource loader
        RConeResourceLoader iResourceLoader;

        /// Own: Conversion buffer
        HBufC*  iBuffer;
        /// Own: pointer to time buffer
        HBufC*  iTimeText;
        /// Own: pointer to "pending" buffer
        HBufC*  iPendingText;       
        /// Own: pointer to "read" buffer
        HBufC*  iRead;       
        /// Own: pointer to "failed" buffer
        HBufC*  iFailedText;
        /// Own: pointer to "delivered" buffer
        HBufC*  iDeliveredText;
        /// Own: pointer to time format buffer
        HBufC*  iTimeFormat;
        /// Own: pointer to time date format buffer
        HBufC*  iTimeDateFormat;
        /// Own: buffer for standard string
        TLogString iFailed;
        /// Own: buffer for standard string
        TLogString iPending;
        /// Own: array of conc. items
        CArrayFixFlat<TInt>* iConcItems;
        /// Own: buffer for conc. items
        HBufC* iConcText;
        /// Own: interval between home and universal time
        TTimeIntervalMinutes iInterval;

        /// Own: pointer to number grouping class
        CPNGNumberGrouping* iNumberGrouping;

        /// Ref: reference to listbox
        CEikFormattedCellListBox *iListBox;
    };

#endif  // CMceDeliveryReportAdapter_H

// End of File
