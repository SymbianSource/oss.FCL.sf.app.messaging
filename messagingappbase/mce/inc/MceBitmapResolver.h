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



#ifndef MCEBITMAPRESOLVER_H
#define MCEBITMAPRESOLVER_H

//  INCLUDES

#include "mcetemplate.h"
#include <AknsUtils.h>

// CONSTANTS
const TInt KMceMmsPriority = 1;	// For loading the MMS priority icons

// DATA TYPES
// these should correspond to the icon array created in CMceBitmapResolver::ConstructL
enum TMceBitmapArrayIndexes
    {
    EMceBitmapIndexMessageSelected = 0,
    EMceBitmapIndexWrite,
    EMceBitmapIndexRemote,
    EMceBitmapIndexRemoteNew,
    EMceBitmapIndexDr,
    EMceBitmapIndexInbox,
    EMceBitmapIndexInboxNew,
    EMceBitmapIndexDocuments,
    EMceBitmapIndexDocumentsNew,
    EMceBitmapIndexDrafts,
    EMceBitmapIndexSent,
    EMceBitmapIndexOutbox,
    EMceBitmapIndexUnknown,
    EMceBitmapIndexFolderTemp,
    EMceBitmapIndexFolderSmall,
    EMceBitmapIndexFolderSmallNew,
    EMceBitmapIndexAttachment,
    EMceBitmapIndexMailboxConnectionOn,
    EMceBitmapIndexAlwaysAdd,
    EMceBitmapIndexInactiveAdd,
    EMceBitmapIndexAlwaysRoaming,
    EMceBitmapIndexPriorityHigh,
    EMceBitmapIndexPriorityLow,
    EMceBitmapIndexReplaceMtmIcon // this MUST be last
    };

// FORWARD DECLARATIONS
class CMsvSession;
class MMtmUiDataRegistryObserver;

// CLASS DECLARATION

/**
*  Helper class for bitmap resolver.
*/
class TMceBitmapListItem
    {
    public:                     // Data
        TBool       iExtraItem;
        TUid        iType;  // if extra item, includes application uid to load correct application icon...
        TUid        iMtm;
        TInt        iIndex; // probably not neccessary
        TMsvId      iId;    // needed with inbox, outbox, etc.
    };

/**
*  Handles different mce bitmaps.
*  First collects all the bitmaps from different mtm's and also local
*  folder's bitmaps to icon array.
*/
class CMceBitmapResolver : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMceBitmapResolver* NewL(
            CMsvSessionPtr aSession,
            MMtmUiDataRegistryObserver& aRegObserver,
            TBool aLargeIcons );

        /**
        * Destructor.
        */
        ~CMceBitmapResolver();

        /**
        * Returns pointer to icon array.
        * @return Pointer to icon array.
        */
        CArrayPtrFlat<CGulIcon>* IconArray();

        /**
        * Can be used to get index of the bitmap of the message.
        * @param aItem: Fill TMceBitmapListItem fields before calling this.
        * @return Index of the bitmap requested.
        */
        TInt BitmapIndex( const TMceBitmapListItem& aItem ) const;

        /**
        * Can be used to get index of the bitmap of the message.
        * Loads bitmap from the mtm ui data and changes it to be last icon in icon array
        * @param aEntry: Fill TMsvEntry fields before calling this.
        * @return Index of the bitmap requested.
        */
        TInt BitmapIndex( const TMsvEntry& aEntry );

#ifdef RD_MSG_NAVIPANE_IMPROVEMENT
        /**
        * Can be used to get index of the bitmap of the message.
        * Loads bitmap from the mtm ui data and changes it to be last icon in icon array
        * Since 5.0
        * @param aEntry: Fill TMsvEntry fields before calling this.
        * @param aMessageCount: number of messages under the entry (0 if entry is not folder)
        * @return Index of the bitmap requested.
        */
        TInt BitmapIndex( const TMsvEntry& aEntry, TInt& aMessageCount );

        /**
        * Checks if entry aId has unread children.
        * Since 5.0
        * @param aId: folder or service to be checked.
        * @param aMessages:number of messages under the folder or service.
        * @param aUnreadMessages: number of unread messages under the folder or service.
        * @return ETrue if unread children found.
        */
        TBool HasUnreadMessagesL( const TMsvId aId, TInt& aMessages, TInt& aUnreadMessages );
#else

        /**
        * Checks if entry aId has unread children.
        * @param aId: folder or service to be checked.
        * @return ETrue if unread children found.
        */
        TBool HasUnreadMessagesL( const TMsvId aId );
#endif // RD_MSG_NAVIPANE_IMPROVEMENT

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
        * @param aHasUnreadMessages ETrue if current service/folder has unread messages, othervise EFalse.
        *        this is needed because BitmapIndex is already doing this calculation so prevent duplicate calculation.
        * @return Index of the bitmap requested.
        */
#ifndef RD_MSG_NAVIPANE_IMPROVEMENT
        TInt LoadBitmapL( const TMsvEntry& aEntry, TBool aPriority = EFalse );
#else
        TInt LoadBitmapL( const TMsvEntry& aEntry, TBool aHasUnreadMessages );
#endif
        /**
        * Loads and adds local bitmap to icon array.
        * @param aId
        * @param aFileName
        * @param aBitmapIndex
        * @param aListItem
        */
        void CreateAndAppendIconL(
            TAknsItemID aId,
            const TDesC& aFileName,
            TInt aBitmapIndex,
            const TMceBitmapListItem* aListItem = NULL,
            TBool aColorSkinnedIcon = EFalse );

                    
        /**
        * Returns bitmap index which is used as placeholder for
        * mtm icons
        * @since 3.0
        * @return index in the icon array
        */
        TInt MtmIconIndex();
        
        /**
        * description length defined in muiu cenrep file.
        * KCRUidMuiuSettings / KMuiuDescriptionLength
        */
        TInt DescriptionLength();
        
        
    private:

        /**
        * Constructor.
        */
        CMceBitmapResolver(
            CMsvSessionPtr aSession,
            MMtmUiDataRegistryObserver& aRegObserver,
            TBool aLargeIcons );

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Loads and adds local bitmap to icon array. Uses \system\data\muiu.mbm
        * @param aId
        * @param aBitmapIndex
        * @param aListItem
        */
        void CreateAndAppendIconL(
            TAknsItemID aId,
            TInt aBitmapIndex,
            const TMceBitmapListItem* aListItem = NULL );

        /**
        * Returns bitmap index of the entry
        * @param aEntryId: id of the entry to be returned
        * @return index of the bitmap
        */
        TInt LocalEntryBitmapIndex( TMsvId aEntryId ) const;

    private:    // Data
        MMtmUiDataRegistryObserver& iRegObserver;
        CMsvSessionPtr              iSession;
        TBool                       iLargeIcons; // ETrue if large icons == main view

        CArrayPtrFlat<CGulIcon>*    iIcons;
        CArrayFixFlat<TMceBitmapListItem>* iIconIndexes;

        // security data caging
        TFileName                   iFilenameMuiu;
        TFileName                   iFilenameAvkon;
        TInt                        iBitmapsLoaded;
        
        TInt                        iDescriptionLength;


    };


#endif


// End of file
