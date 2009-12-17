/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View for setting additional mail headers
*
*/



#ifndef CMSGADDMAILHEADERSDLG_H
#define CMSGADDMAILHEADERSDLG_H

// INCLUDES
#include <e32base.h>
#include <s32std.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class TAdditionalHeaderStatus;

// CLASS DECLARATION

/**
*  Dialog for configuring additional mail headers
*
*  @since Series 3.0
 */
NONSHARABLE_CLASS(CMsgAddMailHeadersDlg) : public CBase
	{

	public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aHeaders headers array
        */
        static CMsgAddMailHeadersDlg* NewLC(
            RPointerArray<TAdditionalHeaderStatus>& aHeaders );

        /**
        * Destructor.
        */
        virtual ~CMsgAddMailHeadersDlg();

    public: // New functions

        /**
        * Launches the view for additional headers
        * @since Series 3.0
        * @return value of executed view
        */
        TBool ExecuteDialogL();
    
    private: // implementation
        
        CDesCArrayFlat* CreateItemTextArrayLC(
            CArrayFixFlat<TInt>* aVisibleItems );
        void UpdateHeadersArrayL(
            CArrayFixFlat<TInt>* aSelectedItems );    

    private:

        /**
        * C++ constructor.
        */
        CMsgAddMailHeadersDlg(
            RPointerArray<TAdditionalHeaderStatus>& aHeaders );

    private: //data

    // refs: selection data headers
    RPointerArray<TAdditionalHeaderStatus> iHeaders;

	};
#endif      //CMSGADDMAILHEADERSDLG_H

// End of File
