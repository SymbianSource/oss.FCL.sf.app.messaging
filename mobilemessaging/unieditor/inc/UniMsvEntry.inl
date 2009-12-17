/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline methods for TUniMsvEntry
*
*/




// ---------------------------------------------------------
// TUniMsvEntry::CurrentMessageType
// ---------------------------------------------------------
//
inline TUniMessageCurrentType TUniMsvEntry::CurrentMessageType( const TMsvEntry& aEntry )
    {
    // Current type bits are the "lowest" bits in iMtmData1.
    // -> No need to shift the bits.
    return TUniMessageCurrentType( aEntry.iMtmData1 & KUniMessageCurrentTypeMask );
    }

// ---------------------------------------------------------
// TUniMsvEntry::SetCurrentMessageType
// ---------------------------------------------------------
//
inline void TUniMsvEntry::SetCurrentMessageType( TMsvEntry& aEntry, TUniMessageCurrentType aParam )
    {
    aEntry.iMtmData1 &= ~KUniMessageCurrentTypeMask;
    // Current type bits are the "lowest" bits in iMtmData1.
    // -> No need to shift the bits.
    aEntry.iMtmData1 |= aParam;
    }

// ---------------------------------------------------------
// TUniMsvEntry::CharSetSupport
// ---------------------------------------------------------
//
inline TUniMessageCharSetSupport TUniMsvEntry::CharSetSupport( const TMsvEntry& aEntry )
    {
    return TUniMessageCharSetSupport(
        ( aEntry.iMtmData1 & KUniMessageCharSetSupportMask ) >>
          KUniMessageCharSetSupportShift );
    }

// ---------------------------------------------------------
// TUniMsvEntry::SetCharSetSupport
// ---------------------------------------------------------
//
inline void TUniMsvEntry::SetCharSetSupport( TMsvEntry& aEntry, TUniMessageCharSetSupport aParam )
    {
    aEntry.iMtmData1 &= ~KUniMessageCharSetSupportMask;
    aEntry.iMtmData1 |= ( TInt32( aParam ) << KUniMessageCharSetSupportShift );
    }

// ---------------------------------------------------------
// TUniMsvEntry::IsMessageTypeLocked
// ---------------------------------------------------------
//
inline TBool TUniMsvEntry::IsMessageTypeLocked( const TMsvEntry& aEntry )
    {
    return aEntry.iMtmData1 & KUniMessageTypeLocked;
    }
        
// ---------------------------------------------------------
// TUniMsvEntry::SetMessageTypeLocked
// ---------------------------------------------------------
//
inline void TUniMsvEntry::SetMessageTypeLocked( TMsvEntry& aEntry, TBool aParam )
    {
    if ( aParam )
        {
        aEntry.iMtmData1 |= KUniMessageTypeLocked;
        }
    else
        {
        aEntry.iMtmData1 &= ~KUniMessageTypeLocked;
        }
    }

// ---------------------------------------------------------
// TUniMsvEntry::IsForwardedMessage
// ---------------------------------------------------------
//
inline TBool TUniMsvEntry::IsForwardedMessage( const TMsvEntry& aEntry )
    {
    return aEntry.iMtmData1 & KUniMessageForwarded;
    }
        
// ---------------------------------------------------------
// TUniMsvEntry::SetForwardedMessage
// ---------------------------------------------------------
//
inline void TUniMsvEntry::SetForwardedMessage( TMsvEntry& aEntry, TBool aParam )
    {
    if ( aParam )
        {
        aEntry.iMtmData1 |= KUniMessageForwarded;
        }
    else
        {
        aEntry.iMtmData1 &= ~KUniMessageForwarded;
        }
    }

// ---------------------------------------------------------
// TUniMsvEntry::IsEditorOriented
// ---------------------------------------------------------
//
inline TBool TUniMsvEntry::IsEditorOriented( const TMsvEntry& aEntry )
    {
    return aEntry.iMtmData1 & KUniMessageEditorOriented;
    }

// ---------------------------------------------------------
// TUniMsvEntry::SetEditorOriented
// ---------------------------------------------------------
//
inline void TUniMsvEntry::SetEditorOriented( TMsvEntry& aEntry, TBool aParam )
    {
    if ( aParam )
        {
        aEntry.iMtmData1 |= KUniMessageEditorOriented;
        }
    else
        {
        aEntry.iMtmData1 &= ~KUniMessageEditorOriented;
        }
    }    

// ---------------------------------------------------------
// TUniMsvEntry::IsMmsUpload
// ---------------------------------------------------------
//
inline TBool TUniMsvEntry::IsMmsUpload( const TMsvEntry& aEntry )
    {
    return ( aEntry.iBioType == KUidMsgSubTypeMmsUpload.iUid );
    }

// ---------------------------------------------------------
// TUniMsvEntry::SetMmsUpload
// ---------------------------------------------------------
//
inline void TUniMsvEntry::SetMmsUpload( TMsvEntry& aEntry, TBool aParam )
    {
    aEntry.iBioType = aParam
        ? KUidMsgSubTypeMmsUpload.iUid
        : 0;
    }
    
// End of File
