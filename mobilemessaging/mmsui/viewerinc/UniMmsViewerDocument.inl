/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*           MMS Viewer Document class. File of inline functions
*
*/



// ----------------------------------------------------
// CMmsViewerDocument::DataModel
// ----------------------------------------------------
//
inline CUniDataModel& CMmsViewerDocument::DataModel() const
    {
    return *iDataModel;
    }
   
// ----------------------------------------------------
// CMmsViewerDocument::SmilModel
// ----------------------------------------------------
//
inline CUniSmilModel* CMmsViewerDocument::SmilModel() const
    {
    return &( iDataModel->SmilModel() );
    }
    
// ----------------------------------------------------
// CMmsViewerDocument::DataUtils
// ----------------------------------------------------
//
inline CUniDataUtils* CMmsViewerDocument::DataUtils() const
    {
    return &( iDataModel->DataUtils() );
    }

    
// ----------------------------------------------------
// CMmsViewerDocument::CurrentSlide
// ----------------------------------------------------
//
inline TInt CMmsViewerDocument::CurrentSlide()
    {
    return iCurrentSlide;
    }

// ----------------------------------------------------
// CMmsViewerDocument::SetCurrentSlide
// ----------------------------------------------------
//
inline void CMmsViewerDocument::SetCurrentSlide( TInt aSlideNum )
    {
    iCurrentSlide = aSlideNum;
    }

// ----------------------------------------------------
// CMmsViewerDocument::SmilType
// ----------------------------------------------------
//
inline TUniSmilType CMmsViewerDocument::SmilType()
    {
    return iSmilType;
    }

// ---------------------------------------------------------
// CMmsEditorDocument::CreationMode
//
// Accessor.
// ---------------------------------------------------------
//
inline TInt CMmsViewerDocument::CreationMode() const
    {
    return iCreationMode;
    }

// ---------------------------------------------------------
// CMmsEditorDocument::MaxMessageSize
//
// Accessor.
// ---------------------------------------------------------
//
inline TUint32 CMmsViewerDocument::MaxMessageSize() const
    {
    return iMaxMessageSize;
    }
    
// ---------------------------------------------------------
// CMmsEditorDocument::ParseResult
//
// ---------------------------------------------------------
//
inline TInt CMmsViewerDocument::ParseResult() const
    {
    return iParseResult;
    }
       

// ---------------------------------------------------------
// IsUpload
// ---------------------------------------------------------
inline TBool CMmsViewerDocument::IsUpload() const
    {
    return ( CMmsMtmUi::IsUpload( Entry() ) );
    }

inline CUniObjectList* CMmsViewerDocument::ObjectList() const
    {    
    return & ( iDataModel->ObjectList() );
    }
inline CUniObjectList* CMmsViewerDocument::AttachmentList() const
    {    
    return & ( iDataModel->AttachmentList() );
    }
inline CUniSmilList* CMmsViewerDocument::SmilList() const
    {    
    return & ( iDataModel->SmilList() );
    }

inline RFile CMmsViewerDocument::GetAttachmentFileL( CUniObject& aUniObject )
    {
    return iDataModel->DataUtils( ).GetAttachmentFileL( Mtm(), aUniObject.AttachmentId() );
    }

inline void CMmsViewerDocument::DeleteModel( )
    {
    delete iDataModel;  
    iDataModel = NULL;  
    }

