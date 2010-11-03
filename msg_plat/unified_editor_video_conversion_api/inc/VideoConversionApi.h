/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*       MMS Video Conversion API
*
*/



#ifndef __MMSVIDEOCONVERSIONPLUGINAPI_H
#define __MMSVIDEOCONVERSIONPLUGINAPI_H

// INCLUDES
#include <e32base.h>
#include <ecom/ecom.h>

// DATA TYPES

// FORWARD DECLARATIONS

//constants

//Interface UID
const TUid KCVideoConversionInterface = 
	{
	0x102824B6
	};


// CLASS DECLARATIONS

/**
* User of the video conversion plugin must implement this
*/
class MVideoConversionCallback
	{
	public:
	/*
	* Video conversion plugin calls this when the conversion has been done.
	* @param status value of the conversion, KErrNone if success, other in case of failure
	*/
	virtual void VideoConversionDone( TInt aStatus ) = 0;
	
	};

 
class CVideoConversionPlugin : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Factory method of a specific implementation.
        * Use ListImplementationsL to find out if the implementations exist
        * and what are the Uids of the implementations.
        * @return Pointer to object of CVideoConversionPlugin
        */
        IMPORT_C static CVideoConversionPlugin* NewL( const TUid aImplUid );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CVideoConversionPlugin();
        
        /**
        * List all iplmementations of this interface.
        */
        inline static void ListImplementationsL( RImplInfoPtrArray& aImplInfoArray  );

    public: 

        /*
        * Converts a video from mp4 format to 3GPP format. Client is responsible for opening a file handle also for 
        * destination file. Function is asynchronous and returns immediately. Plugin uses callback 
        * interface MVideoConversionCallback to inform completion.
        * @param aSourceFile Handle to source file
        * @param aDestinationFile Handle to destination file
        * @param aTargetSize Maximum target size in bytes
        * @param aCallBack Reference to caller
        
        */
        virtual void ConvertL( RFile& aSourceFile, RFile& aDestinationFile, TInt aTargetSize, MVideoConversionCallback& aCallBack  ) = 0;
        
        /*
        * Cancels video conversion
        */
        virtual void CancelConvert() = 0;
       

    private: 
        // Identification on cleanup
        TUid iDtor_ID_Key;
    };

inline void CVideoConversionPlugin::ListImplementationsL( RImplInfoPtrArray& aImplInfoArray  )
	{
	REComSession::ListImplementationsL(KCVideoConversionInterface, aImplInfoArray);
	}


#endif      // __MMSVIDEOCONVERSIONPLUGINAPI_H

// End of File
