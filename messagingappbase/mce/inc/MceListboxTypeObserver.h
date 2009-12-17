/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Active object to listen list type settings from CenRep 
*     (KCRUidMuiuSettings / KMuiuMailMessageListType )
*
*/



#ifndef __MCELISTBOXTYPEOBSERVER_H__
#define __MCELISTBOXTYPEOBSERVER_H__

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION


class MMceListboxTypeObserver
    {
    public:
        /**
        * Called when setting value changes
        * @param aListboxType: new value
        */
       virtual void ListboxTypeChangedL() = 0;
    };

enum TMceMessageListType
    {
    EMceListTypeOneRow = 0,
    EMceListTypeTwoRow
    };

/**
*  CMceListboxTypeObserver
*
*
*/
class CMceListboxTypeObserver : public CActive
    {
    public:  // Constructors and destructor
    
        /**
        * Constructor
        * @param aObserver: observer, which is notified
        *    when setting value changes.
        */
        static CMceListboxTypeObserver* NewL(
            MMceListboxTypeObserver& aObserver,
            TUint32 aSetting );
    private:

        /**
        * C++ constructor.
        */
        CMceListboxTypeObserver(
            MMceListboxTypeObserver& aObserver,
            TUint32 aSetting );
            
        /**
        * Symbian constructor
        */
        void ConstructL( );
        
        
    public:
            
        /**
        * Destructor.
        */
        virtual ~CMceListboxTypeObserver();

    public: // New functions
        /**
        * @return: current listbox type (one row / two row)
        * TODO: enum for that?
        */
        TInt ListboxType() const;

    private:  // Functions from CActive
        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void RunL();
        
        /**
        * From CActive
        */
        virtual TInt RunError( TInt aError );

    private: //Data
        MMceListboxTypeObserver&    iObserver;
        TInt                        iListType;
        CRepository*                iRepository;
        TUint32                     iSetting;
    };

#endif      // __MCELISTBOXTYPEOBSERVER_H__

// End of File
