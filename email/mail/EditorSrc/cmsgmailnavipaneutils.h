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
* Description:  Navi pane handling utilities
*
*/


#ifndef CMSGMAILNAVIPANEUTILS_H
#define CMSGMAILNAVIPANEUTILS_H

// INCLUDES
#include <e32base.h>
#include <msvstd.h>
#include <AknsItemID.h>

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknIndicatorContainer;
class CFbsBitmap;

/**
* Navi pane handling utilities.
*/
class CMsgMailNaviPaneUtils : public CBase
    {
    public: // Constructors and destructor
    
         /**
         * Creates a new instance of this class.
         * @return newly created instance
         */
        static CMsgMailNaviPaneUtils* NewL();

        /**
        * Destructor.
        */
        ~CMsgMailNaviPaneUtils();    
 
    public: //New functions
    
        /**
        * Set priority indicator (high/low or no indicator).
        * @param aPriority message priority
        */    
        void SetPriorityIndicator( TMsvPriority aPriority );
        
        /**
        * Method for setting size indicator with size data.
        * @param aSize size of message
        */        
        void SetMessageSizeIndicatorL( TInt aSize );
        
        /**
        * Method for setting attachment indicator on/off.
        * @param aHasAttachments
        */        
        void SetAttachmentIndicator( TBool aHasAttachments );

        /**        
        * Edwin/FEP/??? removes navi pane indicators if 
        * input field is not editable (like attachment field). 
        * To correct this, we must use our own navi pane content
        * to do the job. Own navi pane is also required to keep
        * priority indicator in correct place horizontally.
        * @param aDoEnable ETrue if own navi pane should be shown
        */        
        void EnableOwnNaviPaneL( TBool aDoEnable );

    private: // implementation
    
        void SetIndicatorState( TInt aIndicatorId, TBool aDoEnable );
        
        void SetIndicatorValueL( TInt aIndicatorId, const TDesC& aString );
        
        void CreateInvisibleIndicatorL( TInt aIndicatorId );
        
        void CreateIconLC( TInt aIndicatorId, CFbsBitmap*& aBitmap ) const;
                                   
        void GetIconIds( TInt aIndicatorId,
                         TAknsItemID& aSkinId,
                         TInt& aFileBitmapId,
                         TInt& aFileMaskId ) const;

    private:  // Constructor
        /**
        * Default constructor
        */
        CMsgMailNaviPaneUtils();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

    private: // Data
        
        /**
        * Navi pane container that holds navi pane
        * decorator. Not owned. 
        */        
        CAknNavigationControlContainer* iNaviPane;       
        
        /**
        * Our own navi pane decorator to hold message size. Own.
        */        
        CAknNavigationDecorator* iNaviDecorator;
        
        /**
        * Indicator container. Not owned.
        */
        CAknIndicatorContainer* iNaviIndi;
        
    };

#endif // CMSGMAILNAVIPANEUTILS_H

// End of File
