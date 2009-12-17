/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: smilparser_dom implementation
*
*/



#include "smilparser.h"

#include <gmxmldocument.h>
#include <gmxmlelement.h>

class DOMParserPrivate
	{
public:
	DOMParserPrivate(CSmilParser* p) { parser=p; }
	void OpenNodeL(CMDXMLNode* n);
	void CloseNodeL(CMDXMLNode* n);
	CSmilParser* parser;
	};

void DOMParserPrivate::OpenNodeL(CMDXMLNode* n)
	{
	if (n->NodeType()==CMDXMLNode::EElementNode)
		{
		CMDXMLElement* el = static_cast<CMDXMLElement*>(n);
		parser->BeginElementL(el->NodeName());

		for (TInt i = 0; i<el->NumAttributes(); ++i)
			{
			TPtrC name, value;
			el->AttributeDetails(i,name,value);
			parser->AttributeValueL(name,value);
			}

		parser->AttributesEndL(el->NumAttributes());
		}			
	return;
	}


void DOMParserPrivate::CloseNodeL(CMDXMLNode* n)
	{
	if (n->NodeType()==CMDXMLNode::EElementNode)
		{
		parser->EndElementL(n->NodeName());
		}			
	return;
	}


void ParseDomL(CSmilParser* parser, CMDXMLDocument* doc)
	{

	CMDXMLElement* root = doc->DocumentElement();

	CMDXMLNode* n = root;

	DOMParserPrivate* dp = new (ELeave) DOMParserPrivate(parser);
	CleanupStack::PushL(dp);

	while (n)
		{
		dp->OpenNodeL(n);

		if (n->FirstChild())
			n = n->FirstChild();
		else if (n->NextSibling())
			{
			dp->CloseNodeL(n);
			n = n->NextSibling();
			}
		else 
			{
			while (n)
				{
				dp->CloseNodeL(n);
				if (n->NextSibling())
					{
					n=n->NextSibling();
					break;
					}				
				n = n->ParentNode();			
				}
			}
		}
	CleanupStack::PopAndDestroy();
	return;
	}
