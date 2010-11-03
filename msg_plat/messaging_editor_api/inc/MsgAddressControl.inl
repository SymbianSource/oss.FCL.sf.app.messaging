/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: MsgAddressControl.inl*
*/



// ========== INLINE METHODS ===============================

// ---------------------------------------------------------
// TParsingInfo::TParsingInfo
// ---------------------------------------------------------
//
inline TParsingInfo::TParsingInfo():
    iEntryNum( 0 ),
    iStartPos( 0 ),
    iEndPos  ( 0 )
    {
    }

// ---------------------------------------------------------
// TParsingInfo::TParsingInfo
// ---------------------------------------------------------
//
inline TParsingInfo::TParsingInfo( TInt aEntryNum, TInt aStartPos, TInt aEndPos ):
    iEntryNum( aEntryNum ),
    iStartPos( aStartPos ),
    iEndPos  ( aEndPos )
    {
    }

// ---------------------------------------------------------
// TParsingInfo::SetInfo
// ---------------------------------------------------------
//
inline void TParsingInfo::SetInfo( TInt aEntryNum, TInt aStartPos, TInt aEndPos )
    {
    iEntryNum = aEntryNum;
    iStartPos = aStartPos;
    iEndPos   = aEndPos;
    }

// ---------------------------------------------------------
// CMsgAddressControl::ResetParsingInfo
// ---------------------------------------------------------
//
inline void CMsgAddressControl::ResetParsingInfo()
    {
    iParsingInfo.SetInfo( 0, 0, 0 );
    }

// ---------------------------------------------------------
// CMsgAddressControl::SetParsingInfo
// ---------------------------------------------------------
//
inline void CMsgAddressControl::SetParsingInfo( TInt aEntryNum, TInt aStartPos, TInt aEndPos )
    {
    iParsingInfo.SetInfo( aEntryNum, aStartPos, aEndPos );
    }
               
//  End of File
