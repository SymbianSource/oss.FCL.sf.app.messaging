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
* Description:   Provides audiomessage document inline functions
*
*/



inline TInt CAudioMessageDocument::AddressSize() const
    {
    return ( iAddressSize >= 0 ) ? iAddressSize : 0;
    }

inline TBool CAudioMessageDocument::IsForward() const
    {
    TMmsMsvEntry entry = static_cast<TMmsMsvEntry>( Entry() );
    return entry.IsForwardedMessage();
    }

// ---------------------------------------------------------
// CAudioMessageDocument::DefaultMsgFolder
// ---------------------------------------------------------
//
inline TMsvId CAudioMessageDocument::DefaultMsgFolder() const
    {
    return KMsvDraftEntryIdValue;
    }
    
// ---------------------------------------------------------
// CAudioMessageDocument::DefaultMsgService
// ---------------------------------------------------------
//
inline TMsvId CAudioMessageDocument::DefaultMsgService() const
    {
    return KMsvLocalServiceIndexEntryId;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::MtmUi
// ---------------------------------------------------------
//
inline CBaseMtmUi& CAudioMessageDocument::MtmUi()
    {
    return CMsgEditorDocument::MtmUi();
    }

// ---------------------------------------------------------
// CAudioMessageDocument::SetAddressSize
// ---------------------------------------------------------
//
inline void CAudioMessageDocument::SetAddressSize( TInt aAddressSize )
    {
    iAddressSize = aAddressSize;
    }
    
// ---------------------------------------------------------
// CAudioMessageDocument::AudioFileSize
// ---------------------------------------------------------
//
inline TUint CAudioMessageDocument::AudioFileSize() const
    {
    return iAudioFileSize;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::SetAudioFileSize
// ---------------------------------------------------------
//
inline void CAudioMessageDocument::SetAudioFileSize( TUint aAudioFileSize )
    {
    iAudioFileSize = aAudioFileSize;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::MaxMessageSize
// ---------------------------------------------------------
//
inline TUint32 CAudioMessageDocument::MaxMessageSize() const
    {
    return iMaxMessageSize;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::Saved
// ---------------------------------------------------------
//
inline TBool CAudioMessageDocument::Saved() const
    {
    return ( CurrentEntry().Entry().Visible() );
    }

// ---------------------------------------------------------
// CAudioMessageDocument::SetBodyModified
// ---------------------------------------------------------
//
inline void CAudioMessageDocument::SetBodyModified( TBool aModified )
    {
    iBodyModified = aModified;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::BodyModified
// ---------------------------------------------------------
//
inline TBool CAudioMessageDocument::BodyModified() const
    {
    return iBodyModified;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::SetHeaderModified
// ---------------------------------------------------------
//
inline void CAudioMessageDocument::SetHeaderModified( TBool aModified )
    {
    iHeaderModified = aModified;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::HeaderModified
// ---------------------------------------------------------
//
inline TBool CAudioMessageDocument::HeaderModified() const
    {
    return iHeaderModified;
    }

// ---------------------------------------------------------
// CAudioMessageDocument::Modified
// ---------------------------------------------------------
//
inline TBool CAudioMessageDocument::Modified() const
   {
    return ( BodyModified() || HeaderModified() );
    }

// ---------------------------------------------------------
// CAudioMessageDocument::SetDuration
// ---------------------------------------------------------
//
inline void CAudioMessageDocument::SetDuration(TTimeIntervalMicroSeconds aDuration)
	{
	iDuration = aDuration;
	}

// ---------------------------------------------------------
// CAudioMessageDocument::GetDuration
// ---------------------------------------------------------
//
inline TTimeIntervalMicroSeconds CAudioMessageDocument::GetDuration()
	{
	return iDuration;	
	}
	

// ---------------------------------------------------------
// CAudioMessageDocument::MessageType
// ---------------------------------------------------------
//
inline TAmsUiMessageType CAudioMessageDocument::MessageType() const
	{
	return iMessageType;
	}
	
inline CMsgMediaResolver* CAudioMessageDocument::MediaResolver( ) 
    { 
    return iMediaResolver;
    }

inline void CAudioMessageDocument::SetAppMode(TAmsAppStatus aStatus )
    { 
    iAppStatus= aStatus;
    } 

inline TAmsAppStatus CAudioMessageDocument::GetAppMode()
    { 
    return iAppStatus; 
    }   		

inline void CAudioMessageDocument::SetClipStatus(TAmsClipStatus aStatus )
    { 
    iClipStatus= aStatus; 
    }

inline TAmsClipStatus CAudioMessageDocument::GetClipStatus()
    { 
    return iClipStatus; 
    }

inline void CAudioMessageDocument::SetInsertedStatus(TAmsInsertedStatus aStatus )
    { 
    iInsertedStatus = aStatus; 
    }

inline TAmsInsertedStatus CAudioMessageDocument::GetInsertedStatus()
    { 
    return iInsertedStatus; 
    }
   		
inline TAmsRecordIconStatus CAudioMessageDocument::GetRecordIconStatus() 
    {
    return iRecordIconStatus;
    }
   		
inline void CAudioMessageDocument::SetRecordIconStatus ( TAmsRecordIconStatus aRecordIconStatus )
    { 
    iRecordIconStatus = aRecordIconStatus;
    }	
	
	