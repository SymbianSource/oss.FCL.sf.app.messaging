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
* Description:  SelectionOperationBase.inl
*
*
*/


inline void CSelectionOperationBase::SetSelectionOwnershipExternal(TBool aExternalOwnership)
    {
    iSelectionOwnershipExternal=aExternalOwnership;
    }

inline TBool CSelectionOperationBase::SelectionOwnershipExternal() const
    {
    return iSelectionOwnershipExternal;
    }

inline TInt CSelectionOperationBase::NumItems() const
    {
    return (iEntrySelection) ? iEntrySelection->Count() : 0;
    }

inline TInt CSelectionOperationBase::CurrentItemIndex() const
    {
    return iCurrentEntryIndex;
    }
