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
*     Handles different mce bitmaps.
*     First collects all the bitmaps from different mtm's and also local
*     folder's bitmaps to icon array.
*
*/



#ifndef MCEONEROWBITMAPRESOLVER_H
#define MCEONEROWBITMAPRESOLVER_H

//  INCLUDES
#include "mcetemplate.h"
#include <msvapi.h>
#include <AknsUtils.h>

// CONSTANTS

// DATA TYPES
// these should correspond to the icon array created in CMceOneRowBitmapResolver::ConstructL
enum TMceBitmapArrayIndexes
    {
    EMceBitmapIndexUnknown = 0,
    EMceBitmapIndexFolderTemp,
    EMceBitmapIndexFolderSmall,
    EMceBitmapIndexFolderSmallNew,
    EMceBitmapIndexAttachment,
    EMceBitmapIndexAttachmentFocus,
    EMceBitmapIndexPriorityHigh,
    EMceBitmapIndexPriorityHighFocus,
    EMceBitmapIndexPriorityLow,
    EMceBitmapIndexPriorityLowFocus,
    EMceBitmapIndexSubSender,
    EMceBitmapIndexSubSenderUnread,
    EMceBitmapIndexSubSubject,
    EMceBitmapIndexSubSubjectUnread,
    EMceBitmapIndexSubTime,
    EMceBitmapIndexSubTimeUnread,
    EMceBitmapIndexLastIconIndex
    };
    

/**
* this class stores icon id in the tree list and bitmap pointer returned by the mtm ui data.
* more detailed explanation here...
*/    
class TMceHierListIconEntry
    {
    public:
    TInt        iIconId;
    TUid        iMtmUid;
    CFbsBitmap* iBitmapPointer;
    };

typedef RArray<TMceHierListIconEntry> RMceListIconArray;

    

// FORWARD DECLARATIONS
class CMsvSession;
class MMtmUiDataRegistryObserver;
class CAknSingleColumnStyleTreeList;

// CLASS DECLARATION



/**
*  Handles different mce bitmaps.
*  First collects all the bitmaps from different mtm's and also local
*  folder's bitmaps to icon array.
*/
class CMceOneRowBitmapResolver : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMceOneRowBitmapResolver* NewL(
            CMsvSessionPtr aSession,
            MMtmUiDataRegistryObserver& aRegObserver );

        /**
        * Destructor.
        */
        virtual ~CMceOneRowBitmapResolver();

        /**
        * Returns pointer to icon array.
        * @return Pointer to icon array.
        */
        CArrayPtrFlat<CGulIcon>* IconArray();

        /**
        * Can be used to get index of the bitmap of the message.
        * Loads bitmap from the mtm ui data and changes it to be last icon in icon array
        * @param aEntry: Fill TMsvEntry fields before calling this.
        * @return Index of the bitmap requested.
        */
        TInt BitmapIndex( const TMsvEntry& aEntry );

        /**
        * Checks if entry aId has unread children.
        * @param aId: folder or service to be checked.
        * @return ETrue if unread children found.
        */
        TBool HasUnreadMessagesL( const TMsvId aId );

        /**
        * Changes the bitmaps in icon array, if skin is changed
        */
        void ChangeBitmapInIconArrayL(  );

        /**
        * Loads correct bitmap of the message from the mtm ui data and changes
        * it to the last of the iIcons array. In ConstructL dummy icon is added to the
        * last so that it can be changed.
        * @param aEntry: entry to be loaded.
        *        aPriority ETrue if the method is called to retrieve the icon of
        *                  high or low priority of a MMS
        * @return Index of the bitmap requested.
        */
        TInt LoadBitmapL( const TMsvEntry& aEntry );

        /**
        * Returns bitmap index which is used as placeholder for
        * mtm icons
        * @since 3.0
        * @return index in the icon array
        */
        //TInt MtmIconIndex();
        
        
        /**
        * @return: Icon id in the tree list. KErrNotFound if icon is not added to tree list...
        */        
        TInt GetBitmapL( TMsvEntry& aEntry, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );
        
        /**
        *
        */        
        TInt GetBitmapL( TMceBitmapArrayIndexes aBitmapIndex, CFbsBitmap*& aBitmap, CFbsBitmap*& aMask );
        
        /**
        *
        */
        TInt AddTreeIconIdL( 
            CFbsBitmap* aBitmap, 
            TInt aTreeIconIndex,
            TUid aMtmUid );
        
        /**
        *
        */
        void SetTreeIconId( TMceBitmapArrayIndexes aBitmapIndex, TInt aTreeIconIndex );
        
        /**
        *
        */
        TInt TreeIconId( TMceBitmapArrayIndexes aBitmapIndex );
        
        /**
        *
        */
        void SkinChangedL( CAknSingleColumnStyleTreeList* aListbox );
        
        /**
        * Updates icon array
        * @param aListbox
        */
        void UpdateIconArrayL( CAknSingleColumnStyleTreeList* aListbox );
        
    private:

        /**
        * Constructor.
        */
        CMceOneRowBitmapResolver(
            CMsvSessionPtr aSession,
            MMtmUiDataRegistryObserver& aRegObserver );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Returns bitmap index of the entry
        * @param aEntryId: id of the entry to be returned
        * @return index of the bitmap
        */
        TInt LocalEntryBitmapIndex( TMsvId aEntryId ) const;
        
        enum TMceOneRowBitmapLoadType
            {
            EMceOneRowBitmapLoadNormal = 0,
            EMceOneRowBitmapLoadSkinned,
            EMceOneRowBitmapLoadSkinnedHighlight
            };


        /**
        * Loads and adds local bitmap to icon array.
        * @param aId
        * @param aFileName
        * @param aBitmapIndex
        * @param aBitmapType: 0 = 'normal', 1 = skinned, 2 = skinned highlighted
        */
        void CreateAndAppendIconL( 
            MAknsSkinInstance* aSkin,
            TAknsItemID aId,
            const TDesC& aFileName,
            TInt aBitmapIndex, 
            TMceOneRowBitmapLoadType aBitmapType );

    private:    // Data
        MMtmUiDataRegistryObserver& iRegObserver;
        CMsvSessionPtr              iSession;
        CArrayPtrFlat<CGulIcon>*    iIcons;

        // security data caging
        TFileName                   iFilenameMuiu;
        TFileName                   iFilenameAvkon;
        TInt                        iBitmapsLoaded;
        RMceListIconArray           iIconArray;


    };


#endif


// End of file
