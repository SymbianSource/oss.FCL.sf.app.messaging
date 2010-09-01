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
* Description: MsgEditorView.inl*
*/



// ========== INLINE METHODS ===============================

// ---------------------------------------------------------
// CMsgEditorView::SetScrollParts
// ---------------------------------------------------------
//
inline void CMsgEditorView::SetScrollParts( TInt aScrollParts )
    {
    iScrollParts = aScrollParts;
    }

// ---------------------------------------------------------
// CMsgEditorView::SetCurrentPart
// ---------------------------------------------------------
//
inline void CMsgEditorView::SetCurrentPart( TInt aCurrentPart )
    {
    iVisiblePart = aCurrentPart;
    }

// ---------------------------------------------------------
// CMsgEditorView::AknScrollBarModel
// ---------------------------------------------------------
//
inline const TAknDoubleSpanScrollBarModel* CMsgEditorView::AknScrollBarModel() const
    {
    return static_cast<const TAknDoubleSpanScrollBarModel*>( iScrollBar->VerticalScrollBar()->Model() );
    }

// ---------------------------------------------------------
// CMsgEditorView::IsReadOnly
// ---------------------------------------------------------
//
inline TBool CMsgEditorView::IsReadOnly() const
    {
    return iEditorModeFlags & EMsgReadOnly;
    }
        
//  End of File
