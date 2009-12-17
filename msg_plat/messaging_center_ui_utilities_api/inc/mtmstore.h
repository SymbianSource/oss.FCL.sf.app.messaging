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
* Description: mtmstore  declaration
*
*/



#if !defined(__MTMSTORE_H__)
#define __MTMSTORE_H__

//  INCLUDES
#if !defined(__MSVAPI_H__)
#include <msvapi.h>
#endif

// FORWARD DECLARATIONS
class CClientMtmRegistry;
class CMtmUiRegistry;
class CBaseMtmUi;
class CBaseMtm;
class CBaseMtmUiData;
class CMtmUiInfoItem;
class CMtmUiDataRegistry;
class CMtmStoreMtmReleaser;


// CLASS DECLARATION

/**
* MMtmUsageTimerObserver
*/
class MMtmUsageTimerObserver
    {
    public:
        virtual void MtmUsageTimedOut( TUid aMtm ) = 0;
    };


/**
* Defines the CMtmStore class
*/
class CMtmStore: public CBase, public MMtmUsageTimerObserver
    {
    public:
        /**
        * Two phase constuctor
        */
        IMPORT_C static CMtmStore* NewL( CMsvSession& aSession );

        /**
        * Destructor
        */
        IMPORT_C ~CMtmStore();

        /**
        * Loads mtm ui data module
        * @param aMtm: uid of the mtm to be loaded
        */
        IMPORT_C CBaseMtmUiData& MtmUiDataL( TUid aMtm );

        /**
        * Unloads mtm ui data module immediately
        * @param aMtm: uid of the mtm to be unloaded
        */
        IMPORT_C void ReleaseMtmUiData( TUid aMtm );

        /**
        * Loads mtm ui module
        * @param aMtm: uid of the mtm to be loaded
        */
        IMPORT_C CBaseMtmUi& ClaimMtmUiL( TUid aMtm );

        /**
        * Loads mtm ui module and sets context
        * @param aContext: aContext.iMtm is uid of the mtm to be loaded and
        *   after loading context is set to aContext
        */
        IMPORT_C CBaseMtmUi& ClaimMtmUiAndSetContextL( const TMsvEntry& aContext );

        /**
        * Unloads mtm ui module
        * @param aMtm: uid of the mtm to be unloaded
        */
        IMPORT_C void ReleaseMtmUi( TUid aMtm );

        /**
        * Loads mtm ui module
        * puts a releasing cleanup item on the stack
        * @param aMtm: uid of the mtm to be loaded
        */
        IMPORT_C CBaseMtmUi& GetMtmUiLC( TUid aMtm );

        /**
        * Loads mtm ui module and sets context
        * puts a releasing cleanup item on the stack
        * @param aContext: aContext.iMtm is uid of the mtm to be loaded and
        *   after loading context is set to aContext
        */
        IMPORT_C CBaseMtmUi& GetMtmUiAndSetContextLC( const TMsvEntry& aContext );

    private:
        /**
        * from MMtmUsageTimerObserver
        */
        virtual void MtmUsageTimedOut( TUid aMtm );

    private:
        /**
        * C++ constructor
        */
        CMtmStore( CMsvSession& aSession );
        /**
        * Symbian OS constructor
        */
        void ConstructL();

        /**
        * internal
        */
        CMtmUiInfoItem& DoClaimMtmUiL( TUid aMtm );

        /**
        * internal
        */
        TInt FindMtmUiIndex( TUid aMtm ) const;

        /**
        * internal
        */
        TInt FindMtmUiDataIndex( TUid aMtm ) const;

        /**
        * internal
        */
        void AddNewMtmUiHolderL( TUid aMtm, TInt& aNewIndex );

    public: // But not exported
    
        /**
        *
        */
        void SetMtmStoreReleaser( CMtmStoreMtmReleaser* aReleaser );
        
        /**
        *
        */
        inline TBool ReleaserIsSet() const { return iReleaser != NULL; }

    private:
        CMsvSession&                    iSession;
        CArrayPtrFlat<CMtmUiInfoItem>   iMtmUiInfoArray;
        CArrayPtrFlat<CBaseMtmUiData>   iMtmUiDataArray;
        CClientMtmRegistry*             iBaseMtmRegistry;
        CMtmUiRegistry*                 iMtmUiRegistry;
        CMtmUiDataRegistry*             iMtmUiDataRegistry;
        CMtmStoreMtmReleaser*           iReleaser;
    };

/**
* CMtmStoreMtmReleaser
*/
NONSHARABLE_CLASS( CMtmStoreMtmReleaser ) : public CBase
    {
    public:
        /**
        *
        */
        IMPORT_C static void CleanupReleaseMtmUiLC(CMtmStore& aMtmStore, TUid aMtm);
 
    public:
        /**
        *
        */
        ~CMtmStoreMtmReleaser();
       
        /**
        *
        */
        void FreeOfObligations();

    private:
        /**
        *
        */
        CMtmStoreMtmReleaser(CMtmStore& aMtmStore, TUid aMtm);
        
    private:
        CMtmStore*  iMtmStore;
        TUid        iMtm;
    };


#endif // __MTMSTORE_H__

// End of file
