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
* Description: smilparser  declaration
*
*/



#ifndef SMILPARSER_H
#define SMILPARSER_H


#include <e32base.h>
#include <s32strm.h>
#include <gdi.h>

class CSmilTimeContainer;
class CSmilMedia;
class CSmilRegion;
class CSmilPresentation;
class CSmilAnchor;
class CSmilArea;
class CSmilTransition;
class CSmilObject;
class MSmilPlayer;

class CMDXMLDocument;

template <class T> class CLinkedList;

/**
 * SMIL2 semantical parser class
 */

class CSmilParser : public CBase
	{	
	public:
        /**
        * Parser phase one constructor
		* 
		* Takes the player interface object as a parameter. This interface 
		* is used by the parser for reporting parse errors and for getting values 
		* of the content control attributes. It is also passed to the created 
		* presentation object. 
        */
        IMPORT_C static CSmilParser* NewL(MSmilPlayer* aPlayer);
       
        /**
        * Destructor.
        */
        virtual ~CSmilParser();

		/**
		* Parses the given SMIL source, constructing the presentation object. 
		* The given base URL is used for resolving the URLs in the presentation. 
		* 
		* Verbose parse error messages are available through the MSmilPlayer interface.
		*/
		IMPORT_C CSmilPresentation* ParseL(const TDesC& aSmil, const TDesC& aBaseUrl);

		/**
		* Parses the given SMIL document, constructing the presentation object. 
		* The given base URL is used for resolving the URLs in the presentation. 
		*/
		IMPORT_C CSmilPresentation* ParseL(const CMDXMLDocument* aSmilDoc, const TDesC& aBaseUrl);

		/**
		* Parses a CSS2 style color value ("red", "green", "#ffcc44", ...)
		* The aTransparent flag is set if the parsing fails
		*/
		IMPORT_C 
		static TRgb ParseColor( const TDesC& aString, TBool& aTransparent );
		
		/**
		* Enables the layout scaling. This functionality shrinks the layout 
		* so that it fits the screensize while maintaining the original aspect
		* ratio. Images and other media is not scaled unless scaling is
		* is enabled by the fit attribute of the region element of the source
		* document.
		*/
		//inline void SetLayoutScalingEnabled(TBool aScale);
		IMPORT_C void SetMaxDownUpScaling( TReal32 aDown, TReal32 aUp );

	public:

		void BeginElementL(const TDesC& aName);

		void EndElementL(const TDesC& aName);

		void AttributeValueL(const TDesC& aName, const TDesC& aValue);

		void AttributesEndL(TInt aCount);

		TBool CheckSystemAttribute(const TDesC& aName, const TDesC& aValue);

	private:
	
		virtual void ConstructL(MSmilPlayer* aPlayer);

		void Init();

		void ParseTimeListL( const TDesC& aString, TBool aBegin );

		static TReal32 StringToRealValue( const TDesC& aString, TReal32 aDefault );
		static TInt StringToIntValue( const TDesC& aString, TInt aDefault );		

		enum TParseState 
			{
			EInitial, ESmil, EHead, ELayout, ELayoutFinished, EHeadFinished, EBody, EBodyFinished, ESmilFinished
			};


		CSmilParser();

		CSmilObject* iCurrent;		
		CSmilTimeContainer* iTimeContainer;
		CSmilRegion* iCurrentRegion;
		CSmilTransition* iCurrentTransition;
		CSmilAnchor* iAnchor;
		CSmilArea* iArea;

		TParseState iState;

		MSmilPlayer* iPlayer;
		CSmilPresentation* iPresentation;

		TPtrC iTag;

		struct Switch { TInt depth; TBool done; };

		CLinkedList<Switch>* iSwitchStack;

		TInt iDepth;
		TInt iIgnoreDepth;
		TBool iSkipContent;
		TBool iUnknownElement;

		struct Namespace { HBufC* name; HBufC* uri;};

		CLinkedList<Namespace>* iNamespaces;

		TPtrC iParamName;
		TPtrC iParamValue;

		TBool iScalingEnabled;

		TInt iTimegraphSize;
		
		TReal iDown;
		TReal iUp;

	};


#include <smilparser.inl>
#include <smilplayerinterface.h>
#endif
