/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       CMmsViewerChangeSlideOperation, asynchronous operation for object saving
*
*/




#ifndef __MMSVIEWERCHANGESLIDEOPERATION_H
#define __MMSVIEWERCHANGESLIDEOPERATION_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

#include <unislideloader.h>
#include "UniMmsViewerDocument.h"

#include "MmsViewerOperation.h"


// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class CMmsViewerHeader;
class CMsgEditorView;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CMmsViewerChangeSlideOperation, Storage for single attachment in presentation. 
*
* @since 2.0
*/
class CMmsViewerChangeSlideOperation : public CMmsViewerOperation,
                                       public MUniSlideLoaderObserver
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @since    3.0
        */
        CMmsViewerChangeSlideOperation(
            MMmsViewerOperationObserver& aOperationObserver,
            CMmsViewerDocument& aDocument,
            CMmsViewerHeader& aHeader,
            CUniSlideLoader& aSlideLoader,
            CMsgEditorView& aView,
            RFs& aFs );

        /**
        * Destructor
        *
        * @since    3.0
        */
        virtual ~CMmsViewerChangeSlideOperation();

        /**
        *
        */
        void ChangeSlide( TInt aNextNum );

        /** 
        * Reloads control
        */
        void ReLoadControlL( CMsgMediaControl* aControl, CUniObject* aObject );

    private:

        /**
        * From MMms/UniSlideLoaderObserver
        */
        void SlideLoadReady( TInt aError );

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        void RunL();
    
        /**
        * From CActive
        */
        TInt RunError( TInt aError );

        /**
        *
        */
        void DoChangeSlideStepL();

        /**
        *
        */
        void DoResetViewL();

        /**
        *
        */
        void DoLoadL();

        /**
        *
        */
        void DoFinalizeL();

    protected: 
       
        /**
        * 2nd phase constructor.
        *
        * @since    2.0
        */
        void ConstructL();

    protected: // data

        enum TMmsViewerChangeSlideState
            {
            EMmsViewerChangeSlideResetView,
            EMmsViewerChangeSlideLoad,
            EMmsViewerChangeSlideFinalize,
            EMmsViewerChangeSlideEnd
            };

        CMmsViewerHeader&           iHeader;
        CUniSlideLoader&            iSlideLoader;
        CMsgEditorView&             iView;
        TInt                        iNextNum;

        TMmsViewerChangeSlideState  iChangeSlideState;
    };

#endif // __MMSVIEWERCHANGESLIDEOPERATION_H
