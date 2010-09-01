/*
* Copyright (c) 2006,2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Uni Editor AppUi inline functions    
*
*/



// ---------------------------------------------------------
// CUniEditorAppUi::Document
// ---------------------------------------------------------
//
inline CUniEditorDocument* CUniEditorAppUi::Document() const
    {
    return static_cast<CUniEditorDocument*>( CMsgEditorAppUi::Document() );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::FsSession
// ---------------------------------------------------------
//
inline RFs& CUniEditorAppUi::FsSession() const
    {
    return ( iCoeEnv->FsSession() );
    }
    
// ---------------------------------------------------------
// CUniEditorAppUi::SmsPlugin
// ---------------------------------------------------------
//
inline CUniEditorPlugin* CUniEditorAppUi::SmsPlugin()
    {
    return Document()->SmsPlugin();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::MmsPlugin
// ---------------------------------------------------------
//
inline CUniEditorPlugin* CUniEditorAppUi::MmsPlugin()
    {
    return Document()->MmsPlugin();
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ImageCtrl
// ---------------------------------------------------------
//
inline CMsgImageControl* CUniEditorAppUi::ImageCtrl() const
    {
    return ( iView
        ? static_cast<CMsgImageControl*>( iView->ControlById( EMsgComponentIdImage ) )
        : NULL );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SubjectCtrl
// ---------------------------------------------------------
//    
inline CMsgExpandableControl* CUniEditorAppUi::SubjectCtrl() const
    {
    return ( iView
        ? static_cast<CMsgExpandableControl*>( iView->ControlById( EMsgComponentIdSubject ) )
        : NULL );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::BodyCtrl
// ---------------------------------------------------------
//   
inline CMsgBodyControl* CUniEditorAppUi::BodyCtrl() const
    {    
    return ( iView
        ? static_cast<CMsgBodyControl*>( iView->ControlById( EMsgComponentIdBody ) )
        : NULL );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::ToCtrl
// ---------------------------------------------------------
// 
inline CMsgAddressControl* CUniEditorAppUi::ToCtrl() const
    {
    return ( iView
        ? static_cast<CMsgAddressControl*>( iView->ControlById( EMsgComponentIdTo ) )
        : NULL );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::RecipientCtrlEditor
// ---------------------------------------------------------
// 
inline CEikRichTextEditor* CUniEditorAppUi::RecipientCtrlEditor( CUniBaseHeader::THeaderFields aField ) const
    {
    TMsgControlId id = EMsgComponentIdNull;
    if( !iView )
        {
        return NULL;
        }
        
    switch( aField )
        {
        case CUniBaseHeader::EHeaderAddressTo:
            {
            id = EMsgComponentIdTo;
            break;
            }
        case CUniBaseHeader::EHeaderAddressCc:
            {
            id = EMsgComponentIdCc;
            break;
            }
        case CUniBaseHeader::EHeaderAddressBcc:
            {
            id = EMsgComponentIdBcc;
            break;
            }
        default:
            {
            // It's none of the above so lets return NULL    
            return NULL;
            }
        }
        
    return iView->ControlById( id )
            ? &(static_cast<CMsgExpandableControl*>( iView->ControlById( id ) )->Editor( ) )
            : NULL;
    }

// ---------------------------------------------------------
// CUniEditorAppUi::SubjectCtrlEditor
// ---------------------------------------------------------
//
inline CEikRichTextEditor* CUniEditorAppUi::SubjectCtrlEditor() const
    {
    return ( SubjectCtrl( )
        ? &( SubjectCtrl( )->Editor() )
        : NULL );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::BodyCtrlEditor
// ---------------------------------------------------------
//
inline CEikRichTextEditor* CUniEditorAppUi::BodyCtrlEditor() const
    {
    return ( BodyCtrl()
        ? &( BodyCtrl()->Editor() )
        : NULL );
    }

// ---------------------------------------------------------
// CUniEditorAppUi::IsHeaderEmpty
// ---------------------------------------------------------
//
inline TBool CUniEditorAppUi::IsHeaderEmpty() const
    {
    return ( ( Document()->SubjectSize() + Document()->AddressSize() ) <= 0 );    
    }

// ---------------------------------------------------------
// CUniEditorAppUi::IsBodyEmpty
// ---------------------------------------------------------
//
inline TBool CUniEditorAppUi::IsBodyEmpty() const
    {
    return ( Document()->DataModel( )->ObjectList().Count() <= 0 );    
    }

// ---------------------------------------------------------
// CUniEditorAppUi::IsLaunched
// ---------------------------------------------------------
//
inline TBool CUniEditorAppUi::IsLaunched( ) const
	{
	return iEditorFlags & ELaunchSuccessful;
	}
	
// End of file

