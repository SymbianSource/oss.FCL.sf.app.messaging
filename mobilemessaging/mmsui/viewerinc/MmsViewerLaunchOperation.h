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
*       CMmsViewerLaunchOperation, asynchronous operation for object saving
*
*/




#ifndef __MMSVIEWERLAUNCHOPERATION_H
#define __MMSVIEWERLAUNCHOPERATION_H

// INCLUDES

#include <e32base.h>
#include <e32std.h>

#include <msgasynccontrolobserver.h>    
#include <unislideloader.h>

#include "UniMmsViewerDocument.h"
#include "MmsViewerOperation.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class CMsgEditorView;
class CMmsViewerHeader;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CMmsViewerLaunchOperation, Storage for single attachment in presentation. 
*
* @since 2.0
*/
class CMmsViewerLaunchOperation :   public CMmsViewerOperation,
                                    public MMmsDocumentObserver,
                                    public MUniSlideLoaderObserver
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @since    3.0
        */
        CMmsViewerLaunchOperation(
            MUniObjectObserver*         aUniObjectObserver,
            MMsgAsyncControlObserver&   aControlObserver,
            MMmsViewerOperationObserver& aOperationObserver,
            CMmsViewerDocument& aDocument,
            CMsgEditorView& aView,
            RFs& aFs );

        /**
        * Destructor
        *
        * @since    3.0
        */
        virtual ~CMmsViewerLaunchOperation();

        /**
        *
        */
        void Launch();

        inline CMmsViewerHeader* DetachHeader();
        inline CUniSlideLoader* DetachSlideLoader();
        inline TInt ParseResult();

    private:

        void DoLaunchStepL();

        void DoInitializeL();
        void DoPrepareHeaderL();
        void DoPrepareBodyL();
        void DoPrepareErrorResponseMessageL();
        void DoPrepare3GPPBodyL();
        
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

        void InitializeReady( TInt aParseResult, TInt aError );
        void SlideLoadReady( TInt aError );

    protected: 

        /**
        * 2nd phase constructor.
        *
        * @since    2.0
        */
        void ConstructL();

    private:

    protected: // data

        enum TMmsLaunchState
            {
            EMmsLaunchInitialize = 0,
            EMmsLaunchPrepareHeader,
            EMmsLaunchPrepareBody,
            EMmsLaunchEnd
            };

        MMsgAsyncControlObserver&   iControlObserver;
        CUniSlideLoader*            iSlideLoader;
        CMsgEditorView&             iView;
        CMmsViewerHeader*           iHeader;

        TInt                        iLaunchState;
        TInt                        iParseResult;
        TInt                        iError;
        TBool                       iIsInitialised;
        MUniObjectObserver*         iUniObjectObserver;
    };

inline CMmsViewerHeader* CMmsViewerLaunchOperation::DetachHeader()
    {
    CMmsViewerHeader* header = iHeader;
    iHeader = NULL;
    return header;
    }

inline CUniSlideLoader* CMmsViewerLaunchOperation::DetachSlideLoader()
    {
    CUniSlideLoader* slideLoader = iSlideLoader;
    iSlideLoader = NULL;
    return slideLoader;
    }

inline TInt CMmsViewerLaunchOperation::ParseResult()
    {
    return iParseResult;
    }

#endif // __MMSVIEWERLAUNCHOPERATION_H
