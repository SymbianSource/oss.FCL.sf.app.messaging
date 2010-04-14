/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   UniEditor Document inline functions    
*
*/



// ---------------------------------------------------------
// CUniEditorDocument::NonConformantCount
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::NonConformantCount() const
    {
    return iNonConfCount;
    }

// ---------------------------------------------------------
// CUniEditorDocument::CurrentSlide
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::CurrentSlide()
    {
    return iCurrentSlide;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SetCurrentSlide
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetCurrentSlide( TInt aSlideNum )
    {
    iCurrentSlide = aSlideNum;
    }

// ---------------------------------------------------------
// CUniEditorDocument::Saved
// ---------------------------------------------------------
//
inline TBool CUniEditorDocument::Saved() const
    {
    return ( CurrentEntry().Entry().Visible() );
    }

// ---------------------------------------------------------
// CUniEditorDocument::MessageType
// ---------------------------------------------------------
//
inline TUniMessageType CUniEditorDocument::MessageType() const
    {
    return iMessageType;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SetBodyModified
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetBodyModified( TBool aModified )
    {
    if( aModified )
        {
        iFlags |= EUniDocumentBodyModified;
        }
    else
        {
        iFlags &= ~EUniDocumentBodyModified;        
        }
    }

// ---------------------------------------------------------
// CUniEditorDocument::BodyModified
// ---------------------------------------------------------
//
inline TBool CUniEditorDocument::BodyModified() const
    {
    return iFlags & EUniDocumentBodyModified;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SetHeaderModified
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetHeaderModified( TBool aModified )
    {
    if( aModified )
        {
        iFlags |= EUniDocumentHeaderModified;
        }
    else
        {
        iFlags &= ~EUniDocumentHeaderModified;        
        }
    }

// ---------------------------------------------------------
// CUniEditorDocument::HeaderModified
// ---------------------------------------------------------
//
inline TBool CUniEditorDocument::HeaderModified() const
    {
    return iFlags & EUniDocumentHeaderModified;
    }

// ---------------------------------------------------------
// CUniEditorDocument::Modified
// ---------------------------------------------------------
//
inline TBool CUniEditorDocument::Modified() const
    {
    return ( BodyModified() || HeaderModified() );
    }

// ---------------------------------------------------------
// CUniEditorDocument::CreationMode
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::CreationMode() const
    {
    return iCreationMode;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SetCreationMode
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetCreationMode( TInt aCreationMode )
    {
    iCreationMode = aCreationMode;
    }

// ---------------------------------------------------------
// CUniEditorDocument::CreationModeUserChangeable
// ---------------------------------------------------------
//
inline TBool CUniEditorDocument::CreationModeUserChangeable() const
    {
    return iFlags & EUniDocumentCreationModeUserChangeable;
    }

// ---------------------------------------------------------
// CUniEditorDocument::MaxMessageSize
// ---------------------------------------------------------
//
inline TUint CUniEditorDocument::MaxMessageSize() const
    {
    return iMaxMessageSize;
    }

// ---------------------------------------------------------
// CUniEditorDocument::AddressSize
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::AddressSize() const
    {
    return ( iAddressSize >= 0 ) ? iAddressSize : 0;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SetAddressSize
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetAddressSize( TInt aAddressSize )
    {
    iAddressSize = aAddressSize;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SubjectSize
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::SubjectSize() const
    {
    return ( iSubjectSize >= 0 ) ? iSubjectSize : 0;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SetSubjectSize
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetSubjectSize( TInt aSubjectSize )
    {
    iSubjectSize = aSubjectSize;
    }
    
// ---------------------------------------------------------
// CUniEditorDocument::PrevSaveType
// ---------------------------------------------------------
//
inline TUniSaveType CUniEditorDocument::PrevSaveType() const
    {
    return iPrevSaveType;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SetPrevSaveType
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetPrevSaveType( TUniSaveType aPrevSaveType )
    {
    iPrevSaveType = aPrevSaveType;
    }
                
// ---------------------------------------------------------
// CUniEditorDocument::UniBodyState
// ---------------------------------------------------------
//
inline TUniState CUniEditorDocument::UniBodyState() const
    {
    return iBodyState;    
    }
    
// ---------------------------------------------------------
// CUniEditorDocument::SetBodyUniState
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetBodyUniState( TUniState aState )        
    {
    iBodyState = aState;
    }
    
// ---------------------------------------------------------
// CUniEditorDocument::UniHeaderState
// ---------------------------------------------------------
//
inline TUniState CUniEditorDocument::UniHeaderState() const
    {
    return iHeaderState;
    }
    
// ---------------------------------------------------------
// CUniEditorDocument::SetHeaderUniState
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetHeaderUniState( TUniState aState )
    {
    iHeaderState = aState;
    }
    
// ---------------------------------------------------------
// CUniEditorDocument::SmsPlugin
// ---------------------------------------------------------
//
inline CUniEditorPlugin* CUniEditorDocument::SmsPlugin()
    {
    return iSmsPlugin;
    }

// ---------------------------------------------------------
// CUniEditorDocument::MmsPlugin
// ---------------------------------------------------------
//
inline CUniEditorPlugin* CUniEditorDocument::MmsPlugin()
    {
    return iMmsPlugin;
    }

// ---------------------------------------------------------
// CUniEditorDocument::DataModel
// ---------------------------------------------------------
//
inline CUniDataModel* CUniEditorDocument::DataModel()
    {
    return iDataModel;
    }

// ---------------------------------------------------------
// CUniEditorDocument::MaxMmsRecipients
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::MaxMmsRecipients() const
    {
    return iMaxMmsRecipients;
    }

// ---------------------------------------------------------
// CUniEditorDocument::MaxSmsRecipients
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::MaxSmsRecipients() const
    {
    return iMaxSmsRecipients;
    }

// ---------------------------------------------------------
// CUniEditorDocument::MaxSmsParts
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::MaxSmsParts() const
    {
    return iMaxConcatenatedSms;
    }

// ---------------------------------------------------------
// CUniEditorDocument::AbsoluteMaxSmsParts
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::AbsoluteMaxSmsParts() const
    {
    return iAbsMaxConcatenatedSms;
    }

// ---------------------------------------------------------
// CUniEditorDocument::CharConverter
// ---------------------------------------------------------
//
inline CCnvCharacterSetConverter* CUniEditorDocument::CharConverter()
    {
    return iCharConverter;
    }

// ---------------------------------------------------------
// CUniEditorDocument::CharSetSupport
// ---------------------------------------------------------
//
inline TUniMessageCharSetSupport CUniEditorDocument::CharSetSupport()
    {
    return iCharSetSupport;
    }

// ---------------------------------------------------------
// CUniEditorDocument::EmailOverSmsSupported
// ---------------------------------------------------------
//
inline TBool CUniEditorDocument::EmailOverSmsSupported() const
    {
    return iFlags & EUniDocumentEMailOverSms;
    }

// ---------------------------------------------------------
// CUniEditorDocument::CSPBits
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::CSPBits() const
    {
    return iCSPBits;
    }

// ---------------------------------------------------------
// CUniEditorDocument::EikonEnv
// ---------------------------------------------------------
//
inline CEikonEnv* CUniEditorDocument::EikonEnv()
    {
    return iEnvironment;
    }

// ---------------------------------------------------------
// CUniEditorDocument::AbsoluteMaxSmsCharacters
// ---------------------------------------------------------
//
inline TInt CUniEditorDocument::AbsoluteMaxSmsCharacters() const
    {
    return iAbsMaxSmsCharacters;
    }

// ---------------------------------------------------------
// CUniEditorDocument::SetUnicodeCharacterMode
// Changes the unicode mode if maximum character limit is not 
// set. In that case unicode mode is always enabled.
// ---------------------------------------------------------
//
inline void CUniEditorDocument::SetUnicodeCharacterMode( TBool aEnabled )
    {
    if ( aEnabled || 
         iAbsMaxSmsCharacters > 0 )
        {
        iFlags |= EUniDocumentUnicodeCharacterMode;
        }
    else
        {
        iFlags &= ~EUniDocumentUnicodeCharacterMode;
        }
    }

// ---------------------------------------------------------
// CUniEditorDocument::UnicodeCharacterMode
// ---------------------------------------------------------
//
inline TBool CUniEditorDocument::UnicodeCharacterMode() const
    {
    return iFlags & EUniDocumentUnicodeCharacterMode;
    }

// ---------------------------------------------------------
// CUniEditorDocument:: AlternativeEncodingType
// Returns current alternate encoding type
// Turkish SMS-PREQ2265 specific
//---------------------------------------------------------
inline TSmsEncoding CUniEditorDocument:: AlternativeEncodingType()
    {
    return iAlternativeEncodingType;
    }

// ---------------------------------------------------------
// CUniEditorDocument:: SetAlternativeEncodingType
// Sets the current alternate encoding type to input value
// Turkish SMS-PREQ2265 specific
//---------------------------------------------------------
inline void CUniEditorDocument::SetAlternativeEncodingType( TSmsEncoding aAlternateEncoding )
    {
    iAlternativeEncodingType = aAlternateEncoding;
    }

// ---------------------------------------------------------
// CUniEditorDocument:: SmsSizeWarningBytes
// Returns number of bytes to show warning notification when SMS size sweeps the boundary value
// Korean operator req : 415-5434
//  ---------------------------------------------------------
inline TInt CUniEditorDocument::SmsSizeWarningBytes()
    {
    return iSmsSizeWarningBytes;
    }

// ---------------------------------------------------------
// CUniEditorDocument:: SetLaunchFromCvAttachment
// Set/Reset the  attachment Flag 
//  ---------------------------------------------------------

inline void  CUniEditorDocument::SetLaunchFromCvAttachment(TBool aVal)
    {
    if(aVal)
        {
        iFlags |=  ELaunchFromCvAttachment;
        }
    else
        {
        iFlags &= ~ELaunchFromCvAttachment;
        }
    }

// ---------------------------------------------------------
// CUniEditorDocument:: IsLaunchFromCvAttachment
// Verify is attachment Flag set and reset it.
//---------------------------------------------------------
     
 inline TBool CUniEditorDocument::IsLaunchFromCvAttachment()
   {
   TBool x =iFlags & ELaunchFromCvAttachment;
   iFlags &= ~ELaunchFromCvAttachment;
   return(x);
   }
// End of file
