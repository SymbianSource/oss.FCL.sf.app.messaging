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
*       Factory class that creates SMIL Player media renderers
*       accoring to the MIME type of the media file.
*
*/


#include "SmilPlayerMediaFactory.h"

// DRM
#include <DRMCommon.h>
#include <DRMHelper.h>

#include <centralrepository.h>		// Central Repository
#include <MusicPlayerInternalCRKeys.h> // for Central Repository keys

#include <MsgMediaResolver.h>

#include <MMediaFactoryFileInfo.h>

#include "SmilAudioRenderer.h"
#include "SmilImageRenderer.h"
#include "SmilDefaultRenderer.h"
#include "SmilTextRenderer.h"
#include "SmilVideoRenderer.h"

#include "SmilSVGRenderer.h"
#ifdef FACTORY_DEBUG
    #include "SmilMediaLogging.h"
#endif

const TInt KRendererArrayGranularity = 10;
_LIT( KSmilPlayerSpace, " " );

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::CSmilPlayerMediaFactory
// C++ constructor. Initializes class member variables.
// ---------------------------------------------------------
//
CSmilPlayerMediaFactory::CSmilPlayerMediaFactory( CCoeEnv& aCoeEnv,
                                                  CGraphicsContext* aGc,
                                                  MMediaFactoryFileInfo* aFileInfo ) :
    iCoeEnv( aCoeEnv ),
    iGc( aGc ),
    iFileInfo( aFileInfo )
    {
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::ConstructL
// ---------------------------------------------------------
//
void CSmilPlayerMediaFactory::ConstructL()
    {
    iRendererArray = new( ELeave ) CRendererArray( KRendererArrayGranularity );
    
    iDrmCommon = DRMCommon::NewL();
    iDrmHelper = CDRMHelper::NewL();
    
    iMediaResolver = CMsgMediaResolver::NewL();
        
	CRepository* repository = CRepository::NewLC( KCRUidMusicPlayerFeatures );
    
    TInt value( 0 );
	User::LeaveIfError( repository->Get( KRequireDRMInPlayback, value ) );
	
    if ( value )
        {
        iProhibitNonDrmMusic = ETrue;
        
        TBuf<1> tmp; // Magic: 1 char to get length of actual value
        TInt realLen = 0;
        TInt err = repository->Get( KPlaybackRestrictedMimeTypes,
                                    tmp,
                                    realLen );
                                    
        if ( err == KErrOverflow )
            {
            // Prepare list of blocked MIME types
            iProhibitMimeTypeBuffer = HBufC::NewL( realLen + KSmilPlayerSpace().Length() );
            TPtr bufferPtr = iProhibitMimeTypeBuffer->Des();
            
            User::LeaveIfError( repository->Get( KPlaybackRestrictedMimeTypes, bufferPtr ) );
            bufferPtr.Append( KSmilPlayerSpace );
            }
        else
            {
            User::LeaveIfError( err );
            }
        }
    
    CleanupStack::PopAndDestroy( repository );
    }

// ----------------------------------------------------------------------------
// CSmilPlayerMediaFactory::NewL
// ----------------------------------------------------------------------------
EXPORT_C CSmilPlayerMediaFactory* CSmilPlayerMediaFactory::NewL( CCoeEnv& aCoeEnv,
                                                                 CGraphicsContext* aGc,
                                                                 MMediaFactoryFileInfo* aFileInfo )
    {
    CSmilPlayerMediaFactory* self = new( ELeave ) CSmilPlayerMediaFactory( aCoeEnv, 
                                                                           aGc, 
                                                                           aFileInfo );       
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::~CSmilPlayerMediaFactory
// Destructor
// ---------------------------------------------------------
//
CSmilPlayerMediaFactory::~CSmilPlayerMediaFactory()
    {
    delete iRendererArray;
    delete iDrmCommon;
    delete iDrmHelper;
    delete iProhibitMimeTypeBuffer;
    delete iMediaResolver;
        
    iFileInfo = NULL; // For LINT
    iGc = NULL; // For LINT
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::SetBaseUrlL
// Sets Base URL that is not currently used.
// ---------------------------------------------------------
//
void CSmilPlayerMediaFactory::SetBaseUrlL(const TDesC& aBaseUrl)
    {
#ifdef FACTORY_DEBUG
    SMILUILOGGER_WRITEF( _L("CSmilPlayerMediaFactory ::SetBaseUrl(%S,...)"), &aBaseUrl );
#endif

    iBaseUrl.SetTextL(aBaseUrl);
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::RequestMediaL
// Requests media from given URL. Not supported currently.
// ---------------------------------------------------------
//
void CSmilPlayerMediaFactory::RequestMediaL( const TDesC& /*aUrl*/,
                                            MSmilMedia* /*aMedia*/ )
    {
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::PrefetchMediaL
// Called to prefetch media from given URL. Not supported currently.
// ---------------------------------------------------------
//
void CSmilPlayerMediaFactory::PrefetchMediaL( const TDesC& /*aUrl*/ ) 
    {
 
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::CreateRendererL
// Creates given type media renderer. Retrieves media file handle
// and MIME type for the given media URL from client. Initializes
// media factory if not initialized already. Checks the given
// MIME type against supported MIME types and if flag to prohibit
// all non-DRM audio files is specified then checks if audio is
// DRM protected. After this retrieves character set for text type
// media renderers and tried to instantiate media renderer. If 
// there was error on instantiation then calls ResolveError to
// handle error.
// ---------------------------------------------------------
//
MSmilMediaFactory::TMediaFactoryError CSmilPlayerMediaFactory::CreateRendererL( const TDesC& aUrl,
                                                                                MSmilMedia* aMedia,
                                                                                MSmilMediaRenderer*& aRender )
    {
#ifdef FACTORY_DEBUG
    SMILUILOGGER_WRITEF( _L("CSmilPlayerMediaFactory::CreateRenderer(%S,...)"), &aUrl );
#endif
    
    if ( !aMedia )
        {
        User::Leave( KErrArgument );
        }
        
    TMediaFactoryError returnValue = ENoRenderer;
    TInt err( KErrNone );
    
    RFile file;
    TRAP( err, iFileInfo->GetFileHandleL( aUrl, file ) );
    
    if ( err != KErrNone ||
         file.SubSessionHandle() == KNullHandle )
        {
        file.Close();
        
        // File not found. Don't create a renderer.
        return returnValue;
        }
    
    CleanupClosePushL( file );        
    
    TPtrC8 mimeType = iFileInfo->GetMimeTypeL( aUrl );
    
#ifdef FACTORY_DEBUG
    TBuf16<100> buffer;
    buffer.SetLength( mimeType.Length() );
    buffer.Copy( mimeType );
    SMILUILOGGER_WRITEF( _L("Mimetype = %S"), &buffer );
#endif

    TMsgMediaType mimeEnum = iMediaResolver->MediaType( mimeType );
        
#ifdef FACTORY_DEBUG
    SMILUILOGGER_WRITEF( _L("mimeEnum = %d"), mimeEnum );
#endif

    User::LeaveIfError( mimeEnum ); // if not found -1 == KErrNotFound

    //if audio and variatedfeature and ( !DrmProtected )
    if( iProhibitNonDrmMusic &&
        mimeEnum == EMsgMediaAudio )
        {
        if ( !PlaybackAllowedL( mimeType, file ) )
            {
            CleanupStack::PopAndDestroy( &file );
            return returnValue;
            }
        }

    TUint charset( 0 );
    if ( mimeEnum == EMsgMediaText ) 
        {
        charset = iFileInfo->GetCharacterSetL( aUrl );
        }
    
    TRAP( err, aRender =InstantiateRendererL( mimeEnum, 
                                     file, 
                                     aMedia, 
                                     charset ) );

#ifdef FACTORY_DEBUG
    SMILUILOGGER_WRITEF( _L("InstantiateRendererL = %d"), err );
#endif

    if ( err != KErrNone )
        {
        TRAPD( err2, ResolveErrorL( mimeEnum, file, err, aMedia, aRender ) );
        if ( err2 != KErrNone )
            {
            returnValue = ENoRenderer;
            }
        else
            {
            returnValue = EOk;    
            }
        }
    else
        {
        returnValue = EOk;
        }
    
    CleanupStack::PopAndDestroy( &file );
    return returnValue;
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::PrefetchStatus
// Returns current prefetch status. Not supported currently.
// ---------------------------------------------------------
//  
MSmilMediaFactory::TPrefetchStatus CSmilPlayerMediaFactory::PrefetchStatus(
        const TDesC& /*aUrl*/,
        TInt& /*aDown*/,
        TInt& /*aSize*/ )
    {
    return EPrefetchNone;
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::PresentationEnd
// Notifies about presentation end.
// ---------------------------------------------------------
//
void CSmilPlayerMediaFactory::PresentationEnd()
    {
#ifdef FACTORY_DEBUG
    SMILUILOGGER_WRITEF( _L("CSmilPlayerMediaFactory::PresentationEnd()") );
#endif
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::RendererDeleted
// Notifies about specific renderer deletion.
// ---------------------------------------------------------
//
void CSmilPlayerMediaFactory::RendererDeleted( MSmilMediaRenderer* /*aRenderer*/ )
    {
#ifdef FACTORY_DEBUG
    SMILUILOGGER_WRITEF( _L("CSmilPlayerMediaFactory::RendererDeleted()") );
#endif
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::PresentationReady()
// Notifies that presentation is ready.
// ---------------------------------------------------------
//
void CSmilPlayerMediaFactory::PresentationReady()
    {
#ifdef FACTORY_DEBUG
    SMILUILOGGER_WRITEF( _L("CSmilPlayerMediaFactory::PresentationReady()") );
#endif
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::QueryContentType
// Checks if specific MIME type is supported.
// ---------------------------------------------------------
//
TBool CSmilPlayerMediaFactory::QueryContentType( const TDesC& aMimeType ) const
    {
#ifdef FACTORY_DEBUG
    SMILUILOGGER_WRITEF( _L("CSmilPlayerMediaFactory::QueryContentType(%S,...)"), &aMimeType );
#endif

    TBool result( EFalse );
    
    HBufC8* mimeType8 = NULL;
    TRAPD( error, mimeType8 = HBufC8::NewL( aMimeType.Length() ) );
    
    if ( error == KErrNone )
        {
        TPtr8 mimeTypePtr8 =  mimeType8->Des();
                
        mimeTypePtr8.Copy( aMimeType );
            
        if ( iMediaResolver->MediaType( *mimeType8 ) != EMsgMediaUnknown )
            {
            result = ETrue;
            }
    
        delete mimeType8;
        }
        
    return result;
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::GetRenderers()
// Returns an array containing all created renderers.
// ---------------------------------------------------------
EXPORT_C CRendererArray* CSmilPlayerMediaFactory::GetRenderers()
    {
    return iRendererArray;
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::InstantiateRendererL
// Tries to instantiate specified type media renderer.
// ---------------------------------------------------------
MSmilMediaRenderer* CSmilPlayerMediaFactory::InstantiateRendererL( TMsgMediaType aMediaType,
                                                                   RFile& aFileHandle,
                                                                   MSmilMedia* aMedia,
                                                                   TUint aCharSet )
    {
#ifdef FACTORY_DEBUG
    SMILUILOGGER_ENTERFN( "[SMILUI] Factory: InstantiateRendererL" )
#endif

    CSmilMediaRendererBase* renderer = NULL;
    
    switch ( aMediaType )
        {
        // audio types
        case EMsgMediaAudio:
            {
#ifdef FACTORY_DEBUG
            SMILUILOGGER_WRITEF( _L("[SMILUI] Factory: creating audio renderer") );
#endif
            renderer = CSmilAudioRenderer::NewL( aFileHandle, 
                                                 aMedia, 
                                                 *iDrmCommon, 
                                                 *iDrmHelper );
            break;
            }
        //image types
        case EMsgMediaImage:
            {
#ifdef FACTORY_DEBUG
            SMILUILOGGER_WRITEF( _L("[SMILUI] Factory: creating image renderer") );
#endif

            renderer = CSmilImageRenderer::NewL( aFileHandle, 
                                                 aMedia, 
                                                 *iDrmCommon, 
                                                 *iDrmHelper );
            break;
            }
        //text types
        case EMsgMediaText:
        case EMsgMediaXhtml:
            {
#ifdef FACTORY_DEBUG
            SMILUILOGGER_WRITEF( _L("[SMILUI] Factory: creating text renderer") );
#endif

            renderer = CSmilTextRenderer::NewL( aFileHandle, 
                                                aMedia,
                                                *iDrmCommon, 
                                                *iDrmHelper,
                                                aCharSet,
                                                *iGc,
                                                aMediaType
                                                 );
            break;
            }
        //video types
        case EMsgMediaVideo:
            {
#ifdef FACTORY_DEBUG
            SMILUILOGGER_WRITEF( _L("[SMILUI] Factory: creating video renderer") );
#endif

            renderer = CSmilVideoRenderer::NewL( aFileHandle, 
                                                 aMedia,
                                                 *iDrmCommon, 
                                                 *iDrmHelper );
            break;
            }
        // SVG types
        case EMsgMediaSvg:
            {
#ifdef FACTORY_DEBUG
            SMILUILOGGER_WRITEF( _L("[SMILUI] Factory: creating svg renderer") );
#endif

            renderer = CSmilSVGRenderer::NewL( aFileHandle, 
                                               aMedia, 
                                               iFileInfo, 
                                               *iDrmCommon,
                                               *iDrmHelper );
            break;
            }
        case EMsgMediaUnknown:
        default:
            {
#ifdef FACTORY_DEBUG
            SMILUILOGGER_WRITEF( _L("[SMILUI] Factory: creating default renderer") );
#endif

            renderer = CSmilDefaultRenderer::NewL( aFileHandle, 
                                                   aMedia, 
                                                   *iDrmCommon, 
                                                   *iDrmHelper,
                                                   EMsgMediaUnknown, 
                                                   EFalse );
            break;
            }
        }
    
    CleanupStack::PushL( renderer );
    iRendererArray->AppendL( renderer );
    CleanupStack::Pop( renderer );
    
#ifdef FACTORY_DEBUG  
    SMILUILOGGER_LEAVEFN( "[SMILUI] Factory: InstantiateRendererL" )
#endif

    return renderer;
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::ResolveErrorL
// Handles error on media renderer creation.
// ---------------------------------------------------------
//
void CSmilPlayerMediaFactory::ResolveErrorL( TMsgMediaType aMediaType, 
                                             RFile& aFileHandle,
                                             TInt aErrorCode,
                                             MSmilMedia* aMedia,
                                             MSmilMediaRenderer*& aRenderer ) const
    {
#ifdef FACTORY_DEBUG
    SMILUILOGGER_WRITEF( _L("CSmilPlayerMediaFactory::ResolveErrorL(...,%d)"), aErrorCode );
#endif

    switch ( aMediaType )
        {
        case EMsgMediaAudio:
        case EMsgMediaImage:
        case EMsgMediaText:
        case EMsgMediaVideo:
        case EMsgMediaSvg:
        case EMsgMediaXhtml:
            {
            switch( aErrorCode )
                {
                case DRMCommon::EGeneralError:
                case DRMCommon::EUnknownMIME:
                case DRMCommon::EVersionNotSupported:
                case DRMCommon::ESessionError:
                case DRMCommon::ENoRights:
                case DRMCommon::ERightsDBCorrupted:
                case DRMCommon::EUnsupported:
                case DRMCommon::ERightsExpired: 
                case DRMCommon::EInvalidRights:
                case DRMCommon::EPaddingFailed:
                case DRMCommon::EFileError:
                    {
                    aRenderer = CSmilDefaultRenderer::NewL( aFileHandle, 
                                                            aMedia, 
                                                            *iDrmCommon, 
                                                            *iDrmHelper,
                                                            aMediaType, 
                                                            ETrue );
                    break;
                    }
                default:
                    {
                    aRenderer = CSmilDefaultRenderer::NewL( aFileHandle, 
                                                            aMedia, 
                                                            *iDrmCommon, 
                                                            *iDrmHelper,
                                                            aMediaType );
                    break;
                    }
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CSmilPlayerMediaFactory::PlaybackAllowedL
// Determines whether file playing is allowed.
// ---------------------------------------------------------
//
TBool CSmilPlayerMediaFactory::PlaybackAllowedL( const TDesC8& aMimeType, RFile& aFileHandle ) const
    {
    TBool result( EFalse );
    
    // Prepare buffer for aMimeType
    HBufC* mimeBuffer = HBufC::NewLC( aMimeType.Length() + KSmilPlayerSpace().Length() );
    TPtr mimeBufferPtr = mimeBuffer->Des();
    mimeBufferPtr.Copy( aMimeType );

    // FindF() would find "audio/3gpp" in "audio/3gpp2" without
    // the added space.
    mimeBufferPtr.Append( KSmilPlayerSpace );

    // If result is not KErrNotFound, this MIME-type is indeed on blocked list.
    if ( iProhibitMimeTypeBuffer->FindF( mimeBufferPtr ) >= 0 ) 
        {
        TBool value( EFalse );
        iDrmCommon->IsProtectedFile( aFileHandle, value);            
        
        if( value )
            {
            result = ETrue;
            }
        }
    else
        {
        result = ETrue;
        }
    
    CleanupStack::PopAndDestroy( mimeBuffer );
    
    return result;
    }


// End of file
