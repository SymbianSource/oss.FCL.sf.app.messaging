/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Smil reader class with simple API.
 * This is a wrapper class on top of Symbian GMXML parser. This class
 * provides simple APIs for reading smil DOM generated by the parser.
 */

#ifndef SMILXMLREADER_H_
#define SMILXMLREADER_H_

//INCLUDES
#include <e32base.h>
#include <GMXMLParser.h>
#include <GMXMLDocument.h>
#include <GMXMLNode.h>
#include <GMXMLElement.h>

//Forward Declarations
class CSMILDtd;

/**
 * SmilXmlReader
 * Used to read DOM generated by parser
 */
NONSHARABLE_CLASS( SmilXmlReader ) : public CBase,
                                     public MMDXMLParserObserver
    {
public:
    /**
     * Allocate and construct a new SmilXmlReader
     * @return new SmilXmlReader instance
     */
    static SmilXmlReader* NewL( RFile& filehandle );

    /**
     * Destructor
     */
    virtual ~SmilXmlReader();

public:  //Functions from base classes
    /**
     * From MMDXMLParserObserver. See GMXMLParser.h
     */
    void ParseFileCompleteL();

public: //New Functions
    /**
     * Fetch next node from the DOM.
     * Follows preorder traversal algorithm for fetching next node.
     * @return Next node from the DOM
     */
    CMDXMLNode* nextNodeL();

    /**
     * Check if the current node is the last node in traversal.
     * @return EFalse/ETrue
     */
    TBool atEndL();

    /**
     * Reset the reader state to initial and currently traversed node to root
     */
    void reset();

protected:
    /**
     * Constructor
     */
    SmilXmlReader();

private:
    /*
     * Second stage constructor
     * @param filehandle SMIL file handle
     */
    void ConstructL( RFile& filehandle );

    /**
     * Parse SMIL file to create SMIL DOM.
     * @param filehandle SMIL file handle
     */
    void ParseL( RFile& filehandle );

    /**
     * Process the node starts. E.g. <body>
     * @param node
     */
    void openNodeL( CMDXMLNode* node );

    /**
     * Process the node ends. E.g. </body>
     * @param node
     */
    void closeNodeL( CMDXMLNode* node );

    /**
     * Set appropriate parsing state when encountered with a tag start.
     * @param tagname
     */
    void setOpenStateL( const TDesC& tagname );

    /**
     * Set appropriate parsing state when encountered with a tag end.
     * @param tagname
     */
    void setCloseStateL( const TDesC& tagname );

private:  //Data
    /**
     * TParseState Reader's Parsing states
     */
    enum TParseState
        {
        EInitial, ESmil, EHead, ELayout, ELayoutFinished, EHeadFinished, EBody, EBodyFinished, ESmilFinished
        };

    /**
     * iState Reader's current parsing state
     */
    TParseState iState;

    /**
     * iRootNode Dom's root node
     */
    CMDXMLNode* iRootNode;

    /**
     * iCurrentNode Reader's currently traversed node
     */
    CMDXMLNode* iCurrentNode;

    CActiveSchedulerWait iWaitScheduler;
    
    CMDXMLParser* xmlparser;
    };

#endif /* SMILXMLREADER_H_ */
