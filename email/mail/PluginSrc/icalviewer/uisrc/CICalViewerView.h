/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CICalViewerView
*
*/



#ifndef __CICalViewerView_H_
#define __CICalViewerView_H_

//  INCLUDES
#include <COECNTRL.H>
#include <MAgnEntryUi.h>

#include <e32std.h>

// FORWARD DECLARATIONS
class MAgnEntryUi;
class MAgnEntryUiCallback;
class CCalEntry;
class TAgnEntryUiInParams;
class TAgnEntryUiOutParams;
class CICalEngine;
// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ICalBC
*  @since Series 60 3.0
*/
class CICalViewerView : public CCoeControl
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CICalViewerView* NewL( TUid aMtmUid );

        /**
        * Destructor.
        */
        virtual ~CICalViewerView();

    public: // New functions

        /**
        * ExecuteViewL.
        * @since Series 60 3.0
        * @param ?arg1 ?description
        * @return ?description
        */
        TInt ExecuteViewL( RPointerArray<CCalEntry>& aEntries,
                                   const MAgnEntryUi::TAgnEntryUiInParams& aInParams,
                                   MAgnEntryUi::TAgnEntryUiOutParams& aOutParams,
                                   MAgnEntryUiCallback& aCallback );


    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // New functions

        /**
        * ?member_description.
        * @since Series ?XX ?SeriesXX_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CICalViewerView();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( TUid aMtmUid );

        // Prohibit copy constructor if not deriving from CBase.
        // CICalViewerView( const CICalViewerView& );
        // Prohibit assigment operator if not deriving from CBase.
        // CICalViewerView& operator=( const CICalViewerView& );

    private:    // Data
        // own: MR Viewers entry ui
        MAgnEntryUi* iEntryUI;

        // Own: engine
        CICalEngine* iEngine;

    };

#endif      // __CICalViewerView_H_

// End of File
