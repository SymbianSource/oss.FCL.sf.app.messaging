/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: SmilPlayerSynchXmlParser  declaration
*
*/



#ifndef SMILPLAYERSYNCHXMLPARSER_H 
#define SMILPLAYERSYNCHXMLPARSER_H 

// INCLUDES
#include <e32base.h>
#include <gmxmlparser.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
* CSmilPlayerSynchXmlParser XML Parser utility.
* Used to parse SMIL document in DoParsingL()
*
* @lib smilplayer.lib
* @since 3.1
*/
NONSHARABLE_CLASS( CSmilPlayerSynchXmlParser ) : public CBase,
                                                 public MMDXMLParserObserver
                                  
    {
    public:  // Constructors and destructor
    
        /**
        * C++ default constructor.
        */
        CSmilPlayerSynchXmlParser();
    
        /**
        * Destructor.
        */
        virtual ~CSmilPlayerSynchXmlParser();
        
    public: // New functions
        
        /**
        * Parses XML file to XML DOM document.
        *
        * @since S60 3.1
        *
        * @param aFileHandle XML file handle.
        *
        * @return Parsed XML DOM document.
        */
        CMDXMLDocument* ParseL( RFile& aFileHandle );
   
    public: // Functions from base classes        
    
        /**
        * From MMDXMLParserObserver. See GMXMLParser.h
        */
        void ParseFileCompleteL();
      
    private: // Data  
        
        // Scheduler.
        CActiveSchedulerWait iWaitScheduler;
    };

#endif  // SMILPLAYERSYNCHXMLPARSER_H

// End of File
