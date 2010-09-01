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
*     HTML converter for mail viewer, inline functions
*
*/


#ifndef MSGMAILVIEWERHTMLCONV_INL
#define MSGMAILVIEWERHTMLCONV_INL

// INLINE FUNCTIONS

inline HBufC* CStringPair::GetName() const
    {
    return iName;
    }

inline HBufC* CStringPair::GetValue() const
    {
    return iValue;
    }

inline TInt CStringPair::Compare(const CStringPair& aPair1, 
                                 const CStringPair& aPair2)
    {
    return aPair1.iName->Compare(*(aPair2.iName));
    }

inline HBufC* CMsgMailViewerHtmlConv::GetText() const
    {
    return iOrigText;
    }

inline TInt CMsgMailViewerHtmlConv::HandleUListL(const TInt aIndent)
    {
    return HandleListL(aIndent, EFalse);
    }

inline TInt CMsgMailViewerHtmlConv::HandleOListL(const TInt aIndent)
    {
    return HandleListL(aIndent, ETrue);
    }


#endif      // MSGMAILEDITORDOCUMENT_INL

// End of File
