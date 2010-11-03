/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of class MsgMailDRMHandler
*
*/


#ifndef MSGMAILDRMHANDLER_H
#define MSGMAILDRMHANDLER_H

// INCLUDES
#include <e32base.h>
#include <MsgAttachmentInfo.h>
// FORWARD DECLARATIONS
class TContentProtection;
class CMsgAttachmentInfo;
class CDRMHelper;

// CLASS DECLARATION

/**
*  MsgMailDRMHandler: provides DRM functions.
*
*  @lib    MsgMaliUtils.dll
*  @since  2.1
*/
NONSHARABLE_CLASS(MsgMailDRMHandler): public CBase
    {

public:  // Constructors and destructor
        
    /**
    * Two-phased constructor.
    */
    IMPORT_C static MsgMailDRMHandler* NewL();
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~MsgMailDRMHandler();

public: // New functions
        
    /**
    * IsProtectedFile
    *
    * Takes a file name and checks if it is a DCF file or a regular file.
    *
    * @since  2.1
    * @param  aFileName: the name of the file to be tested.
    * @return ETrue if the content is ForwardLocked.
    */
    IMPORT_C TBool IsForwardLockedFileL ( RFile& aFileHandle );
    
    /**
    * IsSuperDistributableFileL
    *
    * Takes a file name and checks if it is a DCF SuperDistributable file.
    *
    * @since  2.1
    * @param  aFileName: the name of the file to be tested.
    * @return ETrue if the content is SuperDistributable.
    */
    IMPORT_C TBool IsSuperDistributableFileL ( RFile& aFileHandle );
    
    /**
    * SetDRMDataTypeL
    * Set correct DRM Data type to attachment info.
    * @param aAttachmentInfo refefence to attachment info.
    * @param aFileHandle open Handle to conten file. Handle is left open
    */
    IMPORT_C void SetDRMDataTypeL ( 
    	CMsgAttachmentInfo& aAttachmentInfo, 
    	RFile& aFileHandle );

    /**
    * GetDRMDataTypeL
    * Get DRM Data type from a file handle.
    * @param aFileHandle open Handle to conten file. Handle is left open
    * @return Editor base compatible drm type.
    */    	
    IMPORT_C CMsgAttachmentInfo::TDRMDataType GetDRMDataTypeL( RFile aFileHandle );
    
private: // implementation
    
    void GetFileInfoL ( RFile& aFileHandle, 
        TBool& aIsForwardable,
        TBool& aValidRights,
        TBool& aProtection );
    
private:

    /**
    * C++ default constructor.
    */
    MsgMailDRMHandler();

    /**
    * 2nd phase constructor.
    */
    void ConstructL();



protected:  // Data
    
    /// Own: DRM helper
    CDRMHelper* iDRMHelper;
    };

#endif      // MsgMailDRMHandler_H   
            
// End of File
