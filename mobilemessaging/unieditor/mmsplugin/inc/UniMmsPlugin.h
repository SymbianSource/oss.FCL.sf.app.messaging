/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
*       UniEditor Mms Plugin definition.
*
*/



#ifndef __UNIMMSPLUGIN_H
#define __UNIMMSPLUGIN_H

// INCLUDES
#include "UniPluginApi.h"

// DATA TYPES

// FORWARD DECLARATIONS

class CClientMtmRegistry;
class CMmsClientMtm;
class CMmsHeaders;
class CMmsSettings;
class CCommsDatabase;

// CLASS DECLARATION

/**
*
*/ 
class CUniMmsPlugin : public CUniEditorPlugin
	{
	public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CUniMmsPlugin* NewL( TAny* aConstructionParameters );
 		
        /**
        * Destructor.
        */
        virtual ~CUniMmsPlugin();

    public: // From CUniEditorPlugin

        void LoadHeadersL( CMsvStore* aStore );

        void SaveHeadersL( CMsvStore& aStore );

        TMsvId ConvertFromL( TMsvId aId );

        TMsvId ConvertToL( TMsvId aId );
        
        TMsvId CreateReplyL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts );

        TMsvId CreateForwardL( TMsvId aSrc, TMsvId aDest, TMsvPartList aParts );
        
        void SendL( TMsvId aId );

        TBool ValidateServiceL( TBool aEmailOverSms = EFalse );

        void GetSendingSettingsL( TUniSendingSettings& aSettings );

        void SetSendingSettingsL( TUniSendingSettings& aSettings );
        
        TBool IsServiceValidL();

        // Turkish SMS-PREQ2265 Specific
        void SetEncodingSettings(TBool aUnicodeMode, TSmsEncoding aAlternativeEncodingType, TInt aCharSupportType);
       
        void GetNumPDUsL (
               TDesC& aBuf,
               TInt& aNumOfRemainingChars,
               TInt& aNumOfPDUs,
               TBool& aUnicodeMode, 
               TSmsEncoding& aAlternativeEncodingType);
        
	private: // Constructors
	
	    /**
        * C++ default constructor.
        */
        CUniMmsPlugin( CMsvSession& aSession, CUniClientMtm& aUniMtm );

        /**
        * Returns pointer to iMmsMtm member variable  
        */
        CMmsClientMtm* MmsMtmL();

        TMsvId DoCreateReplyOrForwardL( TBool aReply, TMsvId aSrc, TMsvId aDest, TMsvPartList aParts );
        
        /**
        * Generates details for TMsvEntry object
        */
        void MakeDetailsL( TDes& aDetails );
        
        void EditAccessPointL();

    private:  // Data

        CMsvSession& iSession;
        CUniClientMtm& iUniMtm;
        
        CClientMtmRegistry* iMtmRegistry;
        CMmsClientMtm* iMmsMtm;
        CMmsSettings* iMmsSettings;
        CCommsDatabase* iCommsDb;
        
        CMmsHeaders* iMmsHeader;
        
        TParse iFileParse;
        TFileName iFileName;
        TInt iResourceOffset;
    };

#endif      // __UNIMMSPLUGIN_H

// End of File
