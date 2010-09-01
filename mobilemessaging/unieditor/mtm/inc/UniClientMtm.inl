/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline functions for CUniClientMtm
*
*/



inline TUid CUniClientMtm::Type() const
    {
    return CBaseMtm::Type();
    }

inline CMsvSession& CUniClientMtm::Session()
    {
    return CBaseMtm::Session();
    }

inline void CUniClientMtm::SetCurrentEntryL( CMsvEntry * aEntry )
    {
    CBaseMtm::SetCurrentEntryL( aEntry );
    }

inline void CUniClientMtm::SwitchCurrentEntryL( TMsvId aId )
    {
    CBaseMtm::SwitchCurrentEntryL( aId );
    }

inline CMsvEntry& CUniClientMtm::Entry() const
    {
    return CBaseMtm::Entry();
    }

inline TBool CUniClientMtm::HasContext() const
    {
    return CBaseMtm::HasContext();
    }

inline CRichText& CUniClientMtm::Body()
    {
    return CBaseMtm::Body();
    }

inline const CRichText& CUniClientMtm::Body() const
    {
    return CBaseMtm::Body();
    }

// End of File
