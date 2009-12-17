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
* Description:  Engine for ICalViewer
*
*/



#ifndef CICALENGINE_H
#define CICALENGINE_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>

#include <bldvariant.hrh>

// FORWARD DECLARATIONS

class RFileReadStream;
class CRichText;
class CCalEntry;
class MMsvAttachmentManager;
class CMsvAttachment;
class MAgnEntryUi;

// CLASS DECLARATION
/**
*  Ical engine.
*
*  @lib ICalEngine.lib
*  @since Series 60 Series31
*/
NONSHARABLE_CLASS(CICalEngine) : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CICalEngine* NewL();

        /**
        * Destructor.
        */
        virtual ~CICalEngine();

    public: // New functions
        
        /**
        * ConvertRichTextToStreamL
        * Converts ritch text object to read stream.
        * @param aBuffer input buffer
        * @param aReadStream output stream.   
        * @return KErrNone if success.
        */
    	IMPORT_C TInt ConvertRichTextToStreamL(
    	    CRichText* aBuffer, RFileReadStream& aReadStream);
        
        /**
        * ConvertRichTextToStreamL
        * Does nothing.
        * @param aReadStream    
        * @return NULL            
        */
    	IMPORT_C CCalEntry* ParseStreamToCalEntryL(
    	    RFileReadStream& aReadStream);
        
        /**
        * ResolveAttachmentsVisibilityL
        * Does nothing.
        * @param aManager
        * @param aAttachementsArray    
        * @param aBodyHasOMR                   
        */
		IMPORT_C void ResolveAttachmentsVisibilityL(
		    MMsvAttachmentManager& aManager,
		    RPointerArray<CMsvAttachment>& aAttachementsArray, 
		    TBool aBodyHasOMR );
        
        /**
        * CreateEntryUIL
        * Create new agenda entry.
        * @param aMtmId mtm type
        * @return             
        */
		IMPORT_C MAgnEntryUi* CreateEntryUIL( const TDesC8& aMtmId );

    private:

        /**
        * C++ default constructor.
        */
        CICalEngine();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

	private:

    private:    // Data


    };

#endif      // CICALENGINE_H

// End of File
