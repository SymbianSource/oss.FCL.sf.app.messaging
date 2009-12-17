/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides audiomessage appui inline functions
*
*/




inline CMsgAddressControl* CAudioMessageAppUi::ToCtrl() const
    {
    return ( iView
        ? static_cast<CMsgAddressControl*>( iView->ControlById( EMsgComponentIdTo ) )
        : NULL );
    }
    
inline CMsgAddressControl* CAudioMessageAppUi::FromCtrl() const
    {
    return ( iView
        ? static_cast<CMsgAddressControl*>( iView->ControlById( EMsgComponentIdFrom ) )
        : NULL );
    }    

inline CMsgExpandableControl* CAudioMessageAppUi::SubjectCtrl() const
    {
    if ( iSupportedFeatures & EUniFeatureSubject )

        {
        return ( iView
            ? static_cast<CMsgExpandableControl*>( iView->ControlById( EMsgComponentIdSubject ) )
            : NULL );
        }
    return NULL;
    }

inline CMsgBodyControl* CAudioMessageAppUi::BodyCtrl() const
    {
    return ( iView
        ? static_cast<CMsgBodyControl*>( iView->ControlById( EMsgComponentIdBody ) )
        : NULL );
    }

inline CAudioMessageDocument* CAudioMessageAppUi::Document() const
    {
    return static_cast<CAudioMessageDocument*>( CMsgEditorAppUi::Document() );
    }


inline TBool CAudioMessageAppUi::IsHeaderEmpty() const
    {   
    return ( Document()->AddressSize() <= 0 );
    }

inline TBool CAudioMessageAppUi::IsBodyEmpty() const 
    {
    return (Document()->AudioFileSize() == 0 );
    }

inline RFs& CAudioMessageAppUi::FsSession() const
    {
    return ( iCoeEnv->FsSession() );
    }
    
inline TAmsSenderType CAudioMessageAppUi::SenderType()
    {
    return iSenderType;
    }

    
