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



#ifndef SMILPLAYERMEDIAFACTORY_H
#define SMILPLAYERMEDIAFACTORY_H

//  INCLUDES
#include <smiltextbuf.h>
#include <smilmediafactoryinterface.h>
#include "SmilImageRenderer.h"
#include "SmilMediaRendererBase.h"

typedef CArrayPtrFlat<CSmilMediaRendererBase> CRendererArray;

// FORWARD DECLARATIONS
class MMediaFactoryFileInfo;
class DRMCommon;
class CDRMHelper;
class CMsgMediaResolver;

// CLASS DECLARATION

/**
* Factory class that creates SMIL Player media renderers
* accoring to the MIME type of the media file
*
* @lib smilmediarenderer.lib
* @since 2.0
*/
class CSmilPlayerMediaFactory : public CBase,
                                public MSmilMediaFactory 
    {
    public: // Constructors and destructor
        
        
        /**
        * Factory method that creates this object.
        *
        * @param    aCoeEnv      IN Control environment
        * @param    aGc          IN Graphical context used to draw
        *                           
        * @param    aFileInfo    IN Indicates whether only preview should be played. 
                                    Default is EFalse.
        *
        * @return   pointer to instance
        */
        IMPORT_C static CSmilPlayerMediaFactory* NewL( CCoeEnv& aCoeEnv,
                                                       CGraphicsContext* aGc,
                                                       MMediaFactoryFileInfo* aFileInfo );
        
        /**
        * Destructor.
        */
        virtual ~CSmilPlayerMediaFactory();

    public: // Functions from base classes
    
        /**
        * From MSmilMediaRenderer
        * See smilmediafactoryinterface.h for further information
        */
        void SetBaseUrlL( const TDesC& aBaseUrl );

        /**
        * From MSmilMediaRenderer
        * See smilmediafactoryinterface.h for further information
        */
        void RequestMediaL( const TDesC& aUrl, MSmilMedia* aMedia );

        /**
        * From MSmilMediaRenderer
        * See smilmediafactoryinterface.h for further information
        */
        void PrefetchMediaL( const TDesC& aUrl );

        /**
        * From MSmilMediaRenderer
        * See smilmediafactoryinterface.h for further information
        */
        TMediaFactoryError CreateRendererL( const TDesC& aUrl,
                                            MSmilMedia* aMedia, 
                                            MSmilMediaRenderer*& aRender );

        /**
        * From MSmilMediaRenderer
        * See smilmediafactoryinterface.h for further information
        */
        TPrefetchStatus PrefetchStatus( const TDesC& aUrl,
                                        TInt& aDown,
                                        TInt& aSize );

        /**
        * From MSmilMediaRenderer
        * See smilmediafactoryinterface.h for further information
        */
        void RendererDeleted( MSmilMediaRenderer* aRenderer );

        /**
        * From MSmilMediaRenderer
        * See smilmediafactoryinterface.h for further information
        */
        void PresentationReady();

        /**
        * From MSmilMediaRenderer
        * See smilmediafactoryinterface.h for further information
        */
        void PresentationEnd();

        /**
        * From MSmilMediaRenderer
        * See smilmediafactoryinterface.h for further information
        */
        TBool QueryContentType( const TDesC& aMimeType ) const;
        
    public: // New functions    

        /**
        * Returns all renderers in the presentation.
        *
        * @since 3.1
        * 
        * @return Renderer array
        */ 
        IMPORT_C CRendererArray* GetRenderers();

    private:
        
        /**
        * C++ default constructor.
        */
        CSmilPlayerMediaFactory();
        
        /**
        * Constructor.
        */
        CSmilPlayerMediaFactory( CCoeEnv& aCoeEnv,
                                 CGraphicsContext* aGc ,
                                 MMediaFactoryFileInfo* aFileInfo );
        
        /**
        * 2nd phase constructor
        */
        void ConstructL();
    
        /**
        * InstantiateRendererL
        */
        MSmilMediaRenderer* InstantiateRendererL( TMsgMediaType aMediaType, 
                                                  RFile& aFileHandle,
                                                  MSmilMedia* aMedia, 
                                                  TUint aCharSet );
        
        /**
        * ResolveErrorL
        */
        void ResolveErrorL( TMsgMediaType aMediaType, 
                            RFile& aFileHandle,
                            TInt aErrorCode,
                            MSmilMedia* aMedia,
                            MSmilMediaRenderer*& aRenderer ) const;
        
        /**
        * PlaybackAllowedL
        */
        TBool PlaybackAllowedL( const TDesC8& aMimeType, RFile& aFileHandle ) const;
                           
    private: //data
        
        // Control environment
        CCoeEnv& iCoeEnv;
        
        // Graphical context. Used to format all text.
        CGraphicsContext* iGc;
        
        // File info
        MMediaFactoryFileInfo* iFileInfo;

        // Base url used for resolving relative URLs.
        RSmilTextBuf iBaseUrl;
        
        // Renderer array
        CRendererArray* iRendererArray;
        
        // Flag indicating whether non-drm music playing should be allowed.
        TBool iProhibitNonDrmMusic;
        
        // DRM common
        DRMCommon* iDrmCommon;
        
        // DRM Helper
        CDRMHelper* iDrmHelper;
        
        // Contains prohibit mime types.
        HBufC* iProhibitMimeTypeBuffer;
        
        // Resolves media type for mime type.
        CMsgMediaResolver* iMediaResolver;
    };

#endif // SMILPLAYERMEDIAFACTORY_H
