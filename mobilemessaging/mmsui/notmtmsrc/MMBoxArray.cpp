/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of the CMMBoxArray class
*
*/



// INCLUDES
#include <msvapi.h>                     // for CMsvSession
#include <msvids.h>                     // for KMsvRootIndexEntryId
#include <gulutil.h>                    // for KColumnListSeparator
#include <txtetext.h>                  // for CEditableText
#include <mmsconst.h>                   // for KUidMsgMMSNotification
#include "MMBoxArray.h"
#include "NotMtmUiData.h"
#include "MmsMtmConst.h"                // For Logs

//  LOCAL CONSTANTS AND MACROS
const TInt KMMBoxBitmapIndex = 1;
const TInt KMMBoxHighPriorityIconIndex = 2; 
const TInt KMMBoxLowPriorityIconIndex = 3;
const TInt KMMBoxMaxDisplayTextLength = 80;
const TInt KMMBoxMaxVisibleChars = 30;

//
//
// CMMBoxArray
//
//

// ----------------------------------------------------
// CMMBoxArray::CMMBoxArray
// ----------------------------------------------------
CMMBoxArray::CMMBoxArray( CMsvSession& aSession, CNotMtmUiData& aUiData ):
    iSession( aSession ),
    iUiData( aUiData )
    {
    }

// ----------------------------------------------------
// CMMBoxArray::NewL
// ----------------------------------------------------
CMMBoxArray* CMMBoxArray::NewL( CMsvSession& aSession, CNotMtmUiData& aUiData )
    {
    CMMBoxArray* self = new (ELeave) CMMBoxArray(aSession, aUiData);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CMMBoxArray::ConstructL
// ----------------------------------------------------
void CMMBoxArray::ConstructL()
    {
    LOGTEXT(_L8("MMBoxArray: Construct start"));
    TMsvSelectionOrdering ordering = TMsvSelectionOrdering(
        KMsvNoGrouping,
        EMsvSortByDateReverse,
        EFalse );

    iFolderEntry = CMsvEntry::NewL( 
        iSession, 
        KMsvRootIndexEntryId, 
        ordering );

    iListBoxText = HBufC::NewL( KMMBoxMaxDisplayTextLength );
    LOGTEXT(_L8("MMBoxArray: Construct end"));
    }

// ----------------------------------------------------
// CMMBoxArray::~CMMBoxArray
// ----------------------------------------------------
CMMBoxArray::~CMMBoxArray()
    {
    LOGTEXT(_L8("MMBoxArray: Destructor"));
    delete iFolderEntry;
    delete iListBoxText;
    }

// ----------------------------------------------------
// CMMBoxArray::MdcaCount
// ----------------------------------------------------
TInt CMMBoxArray::MdcaCount() const
    {
    LOGTEXT(_L8("MMBoxArray: MdcaCount"));
    TInt retVal = 0;
    CMsvEntrySelection* msgSel = NULL;
    TRAP_IGNORE( msgSel = iFolderEntry->ChildrenWithMtmL( KUidMsgMMSNotification ) );
    retVal = msgSel->Count( );
    delete msgSel;
    return retVal;
    }

// ----------------------------------------------------
// CMMBoxArray::MdcaPoint
// ----------------------------------------------------
TPtrC CMMBoxArray::MdcaPoint(TInt aIndex) const
    {
    LOGTEXT(_L8("MMBoxArray: MdcaPoint"));
    TPtr tempText = iListBoxText->Des();
    tempText.Zero();

    TMsvId childId = EntryId( aIndex );

    TMsvId service=-1;
    TMsvEntry child;
    TInt error = iSession.GetEntry( childId, service, child );

	if( error )
		{
	    tempText.AppendNum( KMMBoxBitmapIndex ); // index of the bitmap to be shown
    	tempText.Append( KColumnListSeparator );
        tempText.Append( CEditableText::ESpace );
	    tempText.Append( KColumnListSeparator );
        tempText.Append( CEditableText::ESpace );
        return tempText;	
		}
	else
		{
	    // construct listbox-string
	    // bitmap
	    LOGTEXT(_L8("MMBoxArray: MdcaPoint Add bitmapindex"));
	    tempText.AppendNum( KMMBoxBitmapIndex ); // index of the bitmap to be shown

	    tempText.Append( KColumnListSeparator );

	    // sender-information
	    if ( child.iDetails.Length() > KMMBoxMaxVisibleChars )
	        {
	        LOGTEXT(_L8("MMBoxArray: MdcaPoint Add sender1"));
	        tempText.Append( child.iDetails.Left( KMMBoxMaxVisibleChars-1 ) );
	        tempText.Append( CEditableText::EEllipsis );
	        }
	    else if ( child.iDetails.Length( ) > 0 )
	        {
	        LOGTEXT(_L8("MMBoxArray: MdcaPoint Add sender2"));
	        tempText.Append( child.iDetails );
	        }
	    else
	        {
	        // append space, needed by list box
	        LOGTEXT(_L8("MMBoxArray: MdcaPoint Add sender3"));
	        tempText.Append( CEditableText::ESpace );
	        }

	    tempText.Append( KColumnListSeparator );

	    HBufC* statusText = NULL;

	    TRAPD( err, statusText = iUiData.StatusTextL( child ) );
	    if( !err )
	        { // No error, status is fine
	        TRAP_IGNORE( CleanupStack::PushL( statusText ) );
	        if ( statusText->Length( ) > KMMBoxMaxVisibleChars )
	            {
	            LOGTEXT(_L8("MMBoxArray: MdcaPoint Add status1"));
	            tempText.Append( statusText->Left( KMMBoxMaxVisibleChars-1 ) );
	            tempText.Append( CEditableText::EEllipsis );
	            }
	        else if ( statusText->Length( ) > 0 )
	            {
	            LOGTEXT(_L8("MMBoxArray: MdcaPoint Add status2"));
	            tempText.Append( statusText->Des() );
	            }
	        else
	            {
	            LOGTEXT(_L8("MMBoxArray: MdcaPoint Add status3"));
	            tempText.Append( CEditableText::ESpace );
	            }
	        CleanupStack::PopAndDestroy( statusText );
	        }
	    else
	        { // status leaved -> enter just space
	        tempText.Append( CEditableText::ESpace );
	        }

        if ( child.Priority() == EMsvHighPriority )
            {
	        tempText.Append( KColumnListSeparator );
	        tempText.AppendNum( KMMBoxHighPriorityIconIndex );
            }
        else if ( child.Priority() == EMsvLowPriority )
            {
	        tempText.Append( KColumnListSeparator );
            tempText.AppendNum( KMMBoxLowPriorityIconIndex );
            }
		}
    LOGTEXT( _L8("MMBoxArray: MdcaPoint result") );

    return tempText;
    }


// ----------------------------------------------------
// CMMBoxArray::EntryId
// ----------------------------------------------------
TMsvId CMMBoxArray::EntryId( TInt aIndex ) const
    {
    LOGTEXT(_L8("MMBoxArray: EntryId"));
    TMsvId retVal = 0;
    CMsvEntrySelection* msgSel = NULL;
    TRAP_IGNORE(msgSel = iFolderEntry->ChildrenWithMtmL( KUidMsgMMSNotification ) );
    retVal = msgSel->At( aIndex );
    delete msgSel;
    return retVal;
    }

// ----------------------------------------------------
// CMMBoxArray::Entry
// ----------------------------------------------------
TMsvEntry CMMBoxArray::Entry( TInt aIndex ) const
    {
    LOGTEXT(_L8("MMBoxArray: Entry"));
    TMsvId service=-1;
    TMsvId indexId = EntryId( aIndex );
    TMsvEntry child;
    iSession.GetEntry( indexId, service, child );
    return child;
    }

// ----------------------------------------------------
// CMMBoxArray::RefreshArrayL
// ----------------------------------------------------
void CMMBoxArray::RefreshArrayL( TMsvId aFolderId )
    {
    LOGTEXT(_L8("MMBoxArray: RefreshArray"));
    iFolderEntry->SetEntryL( KMsvDraftEntryId );
    iFolderEntry->SetEntryL( aFolderId );
    }

// ----------------------------------------------------
// CMMBoxArray::SortType
// ----------------------------------------------------
TMMBoxSortType CMMBoxArray::SortType( ) const
    {
    LOGTEXT(_L8("MMBoxArray: SortType"));
    TMsvSelectionOrdering ordering = iFolderEntry->SortType( );

    TMMBoxSortType type = EMMBoxSortByDate;

    switch( ordering.Sorting( ) )
        {
        case EMsvSortByDate:
        case EMsvSortByDateReverse:
            break;
        case EMsvSortByDetails:
        case EMsvSortByDetailsReverse:
            type = EMMBoxSortByName;
            break;
        case EMsvSortByDescription:
        case EMsvSortByDescriptionReverse:
        default:
            type = EMMBoxSortBySubject;
            break;
        }
    return type;
    }

// ----------------------------------------------------
// CMMBoxArray::SetSortType
// ----------------------------------------------------
TMsvSelectionOrdering CMMBoxArray::SetSortType( const TMMBoxSortType& aType )
    {
    LOGTEXT(_L8("MMBoxArray: SetSortType start"));
    TMsvSorting type;

    switch ( aType )
        {
        case EMMBoxSortByDate:
            type = EMsvSortByDateReverse;
            break;
        case EMMBoxSortByName:
            type = EMsvSortByDetails;
            break;
        case EMMBoxSortBySubject:
        default:
            type = EMsvSortByDescription;
            break;
        }

    if( aType == SortType( ) )
        {
        TRAP_IGNORE( ReverseSortingL( ) );
        }
    else
        {
        TMsvSelectionOrdering ordering = TMsvSelectionOrdering(
            KMsvNoGrouping,
            type,
            EFalse );
        TRAP_IGNORE( iFolderEntry->SetSortTypeL( ordering ) );
        }
    LOGTEXT(_L8("MMBoxArray: SetSortType end"));
    return iFolderEntry->SortType( );
    }

// ----------------------------------------------------
// CMMBoxArray::ReverseSorting
// ----------------------------------------------------
void CMMBoxArray::ReverseSortingL( )
    {
    LOGTEXT(_L8("MMBoxArray: ReverseSorting"));
    TMsvSelectionOrdering ordering = iFolderEntry->SortType( );
    TMsvSorting type = ordering.Sorting( );

    switch( type )
        {
        case EMsvSortByDate:
            type = EMsvSortByDateReverse;
            break;
        case EMsvSortByDateReverse:
            type = EMsvSortByDate;
            break;
        case EMsvSortByDetails:
            type = EMsvSortByDetailsReverse;
            break;
        case EMsvSortByDetailsReverse:
            type = EMsvSortByDetails;
            break;
        case EMsvSortByDescription:
            type = EMsvSortByDescriptionReverse;
            break;
        case EMsvSortByDescriptionReverse:
            type = EMsvSortByDescription;
            break;
        default: // If not found -> return from the function
            return;            
        }
    ordering.SetSorting( type );
    iFolderEntry->SetSortTypeL( ordering );
    }

//  End of File
