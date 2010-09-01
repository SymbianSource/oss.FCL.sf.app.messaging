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
* Description: smilparserold  declaration
*
*/



#ifndef PARSESMIL_H
#define PARSESMIL_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "smilpresentation.h"
#include "parseelement.h"
#include "smilobject.h"

// FORWARD DECLARATIONS
class CNode;
class CResourceTestData;
class CSmilAnchor;
class CSmilTimeContainer;

// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*/
class CParseSmil : public CBase
    {
    public:  // Constructors and destructor
        

        /**
        * Two-phased constructor.
        */
        static CParseSmil* NewL( RFile& aFile, CSmilPresentation* aPresentation, RFs& aSession );
        
        /**
        * Destructor.
        */
        virtual	~CParseSmil();

    public: // New functions
        
        /**
        * Makes the syntax tree
        */
        void MakeTreeL(); 

		/**
        * This function carries out the actual parsing
        */
		void DoParseL();
	
		/**
        * Inserts a SMIL element into a node
        * @param TDesC8& aElement
        * @param CNode& aNode
        */
		void FoundElementL( const TDesC8& aElement, CNode& aNode );
		
		/**
        * Get the element node in the index given
        * @param TInt aIndex
        * @return CNode* pointer to the node
        */
		inline CNode* Element( TInt aIndex );

		/**
        * Return the number of elements in the syntax tree
        * @return TInt number of elements
        */
		inline TInt ElementCount(); 

		/**
        * Check that the elements match.
        */
		void CheckMatchingL();

    public: // Functions from base classes

        /**
        * From ?base_class ?member_description.
        * @param ?arg1 ?description
        * @return ?description
        */
        
    protected:  // New functions
        
        /**
        * ?member_description.
        * @param ?arg1 ?description
        * @return ?description
        */

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */

    private:

		/**
        * By default EPOC constructor is private.
        */
		void ConstructL();

		/**
        * By default C++ constructor is private.
        */
       	CParseSmil( RFile& aFile, CSmilPresentation* aPresentation, RFs& aSession );

		/**
        * Parses the <region> tag of the presentation
        * @param aPtr pointer to descriptor
        */
		void ParseRegionInfoL( TPtrC8& aPtr );
		
		/**
        * Parses the <smil> tag of the presentation
        * @param aPtr pointer to descriptor
        */
		void ParseSmilInfo( TPtrC8& aPtr );

		/**
        * Parses the <body> tag of the presentation
        * @param aPtr pointer to descriptor <- obsolete?
        */
		TBool ParseSwitchL( TInt& aSkip, CSmilTimeContainer* aParent, CSmilAnchor* aAnchor );
		
		/**
        * Parses the <body> tag of the presentation
        * @param aPtr pointer to descriptor <- obsolete?
        */
		TBool EvaluateTestAttribute( const TInt aIndex,
									 const TPtrC8& aValue,
									 CResourceTestData& aTestData );
		
		/**
        * Parses the <body> tag of the presentation
        * @param aPtr pointer to descriptor <- obsolete?
        */
		void IsContainer( TMediaSmil& aMedia );
		
		/**
        * Parses the <body> tag of the presentation
        * @param aPtr pointer to descriptor 
        */
		TBuf8<5> SelectEndingTag( const CSmilObject& aObject );
		
		/**
        * Parses the <body> tag of the presentation
        * @param aPtr pointer to descriptor 
        */
		TBuf8<7> SelectEndingTag( const TMediaSmil aMedia );
		
		/**
        * Parses the <body> tag of the presentation
        * @param aPtr pointer to descriptor 
        */
		TBuf8<7> SelectBeginTag( const TMediaSmil aMedia );

		/**
        * Parses the <body> tag of the presentation
        * @param aPtr pointer to descriptor 
        */
		void ParseHypeReferenceL( TPtrC8& aPtr, CSmilAnchor* aReference );
		
		/**
        * Parses the "I18n" collection of the presentation
        * @param aPtr pointer to descriptor 
        */
		void ParseI18n( TPtrC8& aPtr );
		
		/**
        * Parses the "core" collection of the presentation
        * @param aPtr pointer to descriptor 
        */
		void ParseCore( TPtrC8& aPtr, CSmilObject* aObject );

		/**
        * Parses the <root-layout> tag of the presentation
        * @param aPtr pointer to descriptor
        */
		void ParseRootLayoutInfo( TPtrC8& aPtr );

		/**
        * Parses the <root-layout> tag of the presentation
        * @param aPtr pointer to descriptor
        */
		void ParseLayoutInfo( TPtrC8& aPtr );

		/**
        * Parses the <img> and <text> tag of the presentation
        * @param aPtr pointer to descriptor
		* @param aObject media object
        */
		void ParseObjectL( TPtrC8& aPtr, CSmilMedia* aObject );
		
		/**
        * Parses the <ref> tag of the presentation
        * @param aPtr pointer to descriptor
        */
		void ParseRef( TPtrC8& aPtr, CSmilMedia* aObject );
		
		/**
        * Parses the <seq> tag of the presentation
        * @param aPtr pointer to descriptor
        */
		void ParseContainerL( TPtrC8& aDes, CSmilTimeContainer* aContainer );

		/**
        * Parses the <seq> tag of the presentation
        * @param aPtr pointer to descriptor
        */
		void ParseTimingAttributes( TPtrC8& aPtr, CSmilObject* aObject );
		
		/**
        * Parses the <seq> tag of the presentation
        * @param aPtr pointer to descriptor
        */
		void ParseRestartAndFill( TPtrC8& aPtr, CSmilObject* aObject );
		
		/**
        * Parses the <seq> tag of the presentation
        * @param aPtr pointer to descriptor
        */
		void ParseEndSync( TPtrC8& aPtr, CSmilTimeContainer* aContainer );

		/**
        * Searches for a text string located between "quotation marks".
        * @param aInt indicates the point where the reading starts
		* @param aPtr a pointer to the descriptor to search
        */
		const TPtrC8 TraverseTextBuffer( TInt aInt, TPtrC8& aPtr );
		
		/**
        * Searches for a text string located between "quotation marks".
        * @param aInt indicates the point where the reading starts
		* @param aPtr a pointer to the descriptor to search
        */
		const TPtrC TraverseTextBuffer( TInt aInt, TPtr& aPtr ); 

		/**
        * Searches for a text string located between "quotation marks",
		* and converts the string into the corresponding numeric value.
        * @param aInt indicates the point where the reading starts
		* @param aPtr a pointer to descriptor
		* @return the converted value as integer
		* or -1 if the search fails
        */
		TInt TraverseDimension( TInt aInt, TPtrC8& aPtr );

		/**
        * A tag was found.
		* @param TLex8 aLex
		* @param CNode* aParent
        */
		void FoundTagL( TLex8 aLex, CNode* aParent );

		/**
        * If FindTagL() finds a valid tag it calls this
		* to carry on with the processing
		* @param TLex8 aLex
		* @param CNode* aParent
        */
		void ContinueL( TLex8 aLex, CNode* aParent );
		
		/**
        * Find the start of the tag and check if it's a comment
		* @param TLex8 aLex
		* @return TPtrC8 - descriptor to a started tag
        */
		const TPtrC8 FindStartOfTagL( TLex8 aLex );

		/**
        * Check the validity of the element
		* @param TDesC8& aTag
		* @return TInt true or false
        */
		TInt IsValid( const TDesC8& aTag );

		/**
        * Walk through the presentation and check the 
		* matching of the elements
        */
		void WalkL();

		/**
        * Read from file
		* @param TInt& aRet - error code
		* @return TLex8 - presentation
        */
		TLex8 ReadNext( TInt& aRet );

    private:     // Data

		enum ParseState {
			EInitial, ESmil, EHead, EBody, ELayout
		};

		RFile&						iFile;
		CSmilPresentation*			iPresentation;
		RFs&						iSession;
		TInt						iParse;
		HBufC8*						iBuf;
		HBufC8*						iBuffer;
		CNode*						iNode;
		CArrayPtrFlat<CNode>*		iArray;
		CSmilObject*				iCurrent;
		
		ParseState					iState;

    };

#include "parsesmil.inl"

#endif      // ?INCLUDE_H   
            
// End of File

