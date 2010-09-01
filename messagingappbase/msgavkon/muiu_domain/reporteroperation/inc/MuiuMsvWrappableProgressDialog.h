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
*     Operation progress reporting dialog class.
*
*/



#ifndef __MSVWRAPPABLEPROGRESSDIALOG_H__
#define __MSVWRAPPABLEPROGRESSDIALOG_H__

//  INCLUDES
#include "MuiuMsvRemoteOperationProgress.h"
#include "MsvPrgReporter.h"
#include "MuiuMsvProgressReporterOperation.h"
#include <eikdialg.h>
#include <ConeResLoader.h>
#include <AknProgressDialog.h>
#include <AknsItemID.h>  // TAknsItemID    


// CLASS DECLARATION
/**
* The progress dialog class.
* Encapsulates the display and update of progress information.
* Used by CMsvProgressReporterOperation
*/
class CMsvWrappableProgressDialog : 
    public CAknProgressDialog,
    public MMsvRemoteOperationProgressObserver,
    public MMsvProgressDecoder
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CMsvWrappableProgressDialog* NewL( 
                                    CMtmStore& aMtmStore, 
                                    TInt aImageIndex = KErrNotFound, 
                                    TBool aProgressVisibilityDelayOff = EFalse,
                                    TBool aDisplayWaitAnimation = ETrue );
                                    
        /**
        * Two-phased constructor.
        * This method is used only when progress dialog is shown 
        * also in cover side.
        */
        static CMsvWrappableProgressDialog* NewL( 
                                    CMtmStore& aMtmStore, 
                                    TInt aDialogIndex,
                                    TUid aCategoryUid, 
                                    TInt aImageIndex = KErrNotFound, 
                                    TBool aProgressVisibilityDelayOff = EFalse,
                                    TBool aDisplayWaitAnimation = ETrue );

        /**
        * Destructor
        */
        virtual ~CMsvWrappableProgressDialog();

        /**
        *
        */
        void SetOperationL( CMsvOperation* aOp );

        /**
        * 
        */
        void SetTitleTextL( const TDesC& aText );

        /**
        * 
        */
        void SetTitleTextL( TInt aResource );

        /**
        * 
        */
        void SetCancel( TBool aAllowCancel );

        /**
        * 
        */
        inline void SetProgressDecoder( MMsvProgressDecoder& aDecoder );

        /**
        * 
        */
        inline void UnsetProgressDecoder();

        /**
        * 
        */
        inline void SetHelpContext( const TCoeHelpContext& aContext );

        /**
        * 
        */
        void SetAnimationL( TInt aResourceId ) const;

        /**
        * 
        */
        inline void SetSeeding( TBool aSeed );

    public:
        /**
        * SetVisibilityDelay
        * This function can set visibility delay on or off after construction
        * @param aDelay, boolean to set the delay:
        */
        void SetVisibilityDelay( TBool aDelay );


        /**
        * SetProgressReporter
        * This function sets CMsvProgressReporterOperation
        * @param aProgressReporter, sets CMsvProgressReporterOperation
        */
        void SetProgressReporter( CMsvProgressReporterOperation* aProgressReporter );

    public:

        /**
        * From CEikDialog
        */
        virtual SEikControlInfo CreateCustomControlL( TInt aControlType );

        /**
        * From CEikDialog
        */
        virtual void GetHelpContext( TCoeHelpContext& aContext ) const;

    public: 
        /**
        * From CAknProgressDialog 
        */
        void ProcessFinishedL();

    protected:
        /**
        * From CEikDialog
        */
        virtual TBool OkToExitL( TInt aButtonId );

        /**
        * From CEikDialog
        */
        virtual void PostLayoutDynInitL();

        /**
        * From CEikDialog
        */
        virtual void PreLayoutDynInitL();

    protected: 
        /** 
        * From MMsvRemoteOperationProgressObserver
        */ 
        virtual void UpdateRemoteOpProgressL();

    private:
        /**
        * C++ constructors
        */
        CMsvWrappableProgressDialog(
            CMtmStore& aMtmStore, 
            TInt aNoteControlId, 
            TInt aImageIndex );

        CMsvWrappableProgressDialog(
            CMtmStore& aMtmStore, 
            TInt aNoteControlId, 
            TInt aImageIndex,
            TBool aProgressVisibilityDelayOff,
            TBool aDisplayWaitAnimation );

        /**
        * internal, updates texts
        */
        void DoUpdateRemoteOpProgressL();

        /**
        * internal
        */
        void UpdateLabelText( const TDesC& aProgressString );

        /**
        * internal
        */
        void UpdateProgressBarL( TInt aTotal, TInt aCurrent );

        /**
        * from MMsvProgressDecoder
        */
        TInt DecodeProgress(
            const TDesC8& aProgress, 
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString, 
            TInt& aTotalEntryCount, 
            TInt& aEntriesDone, 
            TInt& aCurrentEntrySize, 
            TInt& aCurrentBytesTrans, 
            TBool aInternal );

        /**
        * Leavable version of DecodeProgress. TRAP'ed in DecodeProgress.
        */
        TInt DoDecodeProgressL(
            const TDesC8& aProgress, 
            TBuf<CBaseMtmUi::EProgressStringMaxLen>& aReturnString, 
            TInt& aTotalEntryCount, 
            TInt& aEntriesDone, 
            TInt& aCurrentEntrySize, 
            TInt& aCurrentBytesTrans );

        /**
        * internal
        */
        void CleanBeforeFinish();

        /**
        * SkinItemId
        * Return TAknsItemID of given bitmap index.
        * @param aBitmapIndex, Index in enum TMbmAvkon(inside avkon.mbg)
        * @return Returns TAknsItemID of given bitmap index.
        */
        const TAknsItemID SkinItemId( const TInt aBitmapIndex )  const;

    private: // Data
        CMtmStore& iMtmStore;
        CBaseMtmUi* iMtmUi;             // No custody
        CMsvRemoteOperationProgress* iTimer;
        CMsvOperation* iOperation;      // No custody
        MMsvProgressDecoder* iDecoder;
        RConeResourceLoader iResources;
        TBool* iDialogDeletedFlag;
        TBool iAllowCancel;
        TBool iSeed;
        TCoeHelpContext iHelpContext;
        TInt iImageIndex;
        CMsvProgressReporterOperation* iProgressReporter;
        TBool iDisplayWaitAnimation;
        TBool iWaitAnimationStarted;
    };

//
//  Inline definitions
//
inline void CMsvWrappableProgressDialog::SetProgressDecoder( MMsvProgressDecoder& aDecoder )
    {
    iDecoder=&aDecoder;
    }

inline void CMsvWrappableProgressDialog::UnsetProgressDecoder()
    {
    iDecoder = this;
    }

inline void CMsvWrappableProgressDialog::SetHelpContext( const TCoeHelpContext& aContext )
    {
    iHelpContext = aContext;
    }

inline void CMsvWrappableProgressDialog::SetSeeding( TBool aSeed )
    {
    iSeed = aSeed;
    }

#endif // __MSVWRAPPABLEPROGRESSDIALOG_H__

// End of file
