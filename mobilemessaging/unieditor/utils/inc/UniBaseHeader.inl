/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: UniBaseHeader.inl*
*/



// ========== INLINE METHODS ===============================

// ---------------------------------------------------------
// CUniBaseHeader::AddressControl
// ---------------------------------------------------------
//
inline CMsgAddressControl* CUniBaseHeader::AddressControl( THeaderFields aType ) const
    {
    return static_cast<CMsgAddressControl*>( iHeaders[aType].iControl );
    }

// ---------------------------------------------------------
// CUniBaseHeader::AddressControlId
// ---------------------------------------------------------
//
inline TMsgControlId CUniBaseHeader::AddressControlId( THeaderFields aType ) const   
    {
    return iHeaders[aType].iControlType;
    }

// ---------------------------------------------------------
// CUniBaseHeader::SubjectControl
// ---------------------------------------------------------
//
inline CMsgExpandableControl* CUniBaseHeader::SubjectControl() const
    {
    return iHeaders[EHeaderSubject].iControl;
    }

// ---------------------------------------------------------
// CUniBaseHeader::AttachmentControl
// ---------------------------------------------------------
//
inline CMsgAttachmentControl* CUniBaseHeader::AttachmentControl() const
    {
    return static_cast<CMsgAttachmentControl*>( iHeaders[EHeaderAttachment].iControl );
    }

// ---------------------------------------------------------
// CUniBaseHeader::AddHeadersVariation
// ---------------------------------------------------------
//
inline TInt CUniBaseHeader::AddHeadersVariation() const
    {
    return iAddHeadersVariation & ( EUniFeatureSubject | 
                                    EUniFeatureCc | 
                                    EUniFeatureSubjectConfigurable | 
                                    EUniFeatureBcc );
    }
               
//  End of File
