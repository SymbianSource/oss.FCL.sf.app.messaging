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
* Description:  
*     Wrapper to make SMIL parsing synchronous.
*
*/



#ifndef __SMILSYNCPARSE_H
#define __SMILSYNCPARSE_H

// INCLUDES
class MXMLDtd;  // Must be before GMXMLParser.h

#include <GMXMLParser.h>


// FORWARD DECLARATIONS
class RFs;
class CMDXMLDocument;
class CActiveSchedulerWait;


/**
* CSmilSyncParse
*
* @lib mmsdatautils.lib
* @since 2.0
*/
NONSHARABLE_CLASS( CSmilSyncParse ) : public CBase, public MMDXMLParserObserver
    {
    public: // New

    /**
    * NewLC
    * Factory method that creates this object.
    *
    * @since    2.0
    * @return   Pointer to instance in cleanup stack
    */
    static CSmilSyncParse* NewLC();

    /**
    * Destructor.
    *
    * @since    2.0
    */
    ~CSmilSyncParse();

    /**
    * StartL.
    * Leaves if parse errors occur.
    *
    * @since    2.0
    * @param    aRFs    IN reference to file session
    * @param    aFileName   IN SMIL file to parse.
    * @return   DOM, ownership is transferred to caller.
    */
    CMDXMLDocument* StartL( RFs aRFs, const TDesC& aFileName );

    /**
    * StartL.
    * Leaves if parse errors occur.
    *
    * @since    3.0
    * @param    aRFile    IN smiltemplate inside RFile
    * @return   DOM, ownership is transferred to caller.
    */
    CMDXMLDocument* StartL( RFile& aFile );

    /**
    * Error
    *
    * @since    2.0
    * @return   Standard Symbian error code
    */
    inline TInt Error() const;

    private: // From MMDXMLParserObserver

    /**
    * ParseFileCompleteL
    * Call back function used to inform client of Parser when RunL function completes
    */
    void ParseFileCompleteL();

    private: 

    /**
    * Constructor.
    *
    * @since    2.0
    */
    CSmilSyncParse();

    /**
    * ConstructL. 
    * 2nd phase constructor.
    *
    * @since    2.0
    */
    void ConstructL();

    private: // Data

    CMDXMLParser*   iParser;
    CActiveSchedulerWait iWait;
    TInt* iWaiting;

    };

// ==========================================================

inline TInt CSmilSyncParse::Error() const
    {
    return ( !iParser ? KErrNotReady : iParser->Error() );
    }

#endif // __SMILSYNCPARSE_H
