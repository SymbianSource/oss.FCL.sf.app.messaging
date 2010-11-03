/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   TestSmilEngine  declaration*
*/




#ifndef TESTSMILENGINE_H
#define TESTSMILENGINE_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include "smiltransitionfilter.h"
#include <GMXMLDocument.h>
#include <GMXMLParser.h>
#include <smilmediafactoryinterface.h>
#include <smilmediainterface.h>
#include <smiltime.h>
#include <smilobject.h>
#include <smilanchor.h>

#include <coecntrl.h>
// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
#define TEST_CLASS_VERSION_MAJOR 0
#define TEST_CLASS_VERSION_MINOR 0
#define TEST_CLASS_VERSION_BUILD 0

// Logging path
_LIT( KTestSmilEngineLogPath, "\\logs\\testframework\\TestSmilEngine\\" ); 
// Log file
_LIT( KTestSmilEngineLogFile, "TestSmilEngine.txt" ); 
_LIT( KTestSmilEngineLogFileWithTitle, "TestSmilEngine_[%S].txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CTestSmilEngine;
class CSmilTransitionFilter;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CTestSmilEngine test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class CDerSmilTransitionFilter : public CSmilTransitionFilter
{
	public: 
	
				CDerSmilTransitionFilter();
				
				virtual void Draw(CGraphicsContext& aGc, const TRect& aRect, const CFbsBitmap* aTargetBitmap, const TPoint& aTarget, const CFbsBitmap* aMaskBitmap = 0);

				virtual TInt ResolutionMilliseconds() const;
				
				void TransitionsL();
	
	};
	
	
class CDerSmilAnchor:public CSmilAnchor
{
	public:
			
			CDerSmilAnchor()
			{
				
				
			}
			
			void Initialise(TDesC& aDes)
			{
				
				SetHrefL( aDes );
				
			}
			
};

    
    class SmilPlayer: public MSmilPlayer
{
    public:
        
        SmilPlayer();
        
        static MSmilPlayer* NewLC();
        
               /* Return the media factory object that is responsible of 
                * creating the media renderers
                */
                MSmilMediaFactory* GetMediaFactory(const CSmilPresentation* aPres) const;

                /** 
                * Return the transition factory object that is responsible of 
                * creating the transition renderers
                */
                MSmilTransitionFactory* GetTransitionFactory(const CSmilPresentation* aPres) const;

                /** 
                * Evaluate a content control attribute for true or false. 
                * Attributes 
                *       systemOperatingSystem
                *       systemCPU
                *       systemScreenSize 
                *       systemScreenDepth 
                * are handled inside the SMIL engine, the rest are passed to this method.
                */
                TBool EvaluateContentControlAttribute(const TDesC& aName, const TDesC& aValue);

                /** 
                * Return the dimensions of the drawing surface
                */
                TRect GetDimensions(const CSmilPresentation* aPres);

                /** 
                * Return the EIKON GUI control that is used as the view for the given 
                * presentation.
                *
                * SMIL Engine never calls this method. It might be used by 
                * implementations of MSmilMediaRenderer interface.
                */
                CCoeControl* GetControl(const CSmilPresentation* aPres) ;

                /** 
                * The engine calls this method to initiate redrawing of the given area. 
                * Implementation of this method should in turn call Draw method of the 
                * given presentation object.
                */
                void Redraw(TRect aArea, CSmilPresentation* aPres) ;

                /** 
                * This method is called by the engine as a result of hyperlink activation.
                * Player should open the given (relative to the presentation base) url. 
                * However, the current presentation MUST NOT be deleted during execution 
                * of this method, as this method is invoked by the current presentation,
                * 
                * aAnchor - contains addition information about the link. can be 0.
                *
                * source/destination states: 
                * EPlay - (continue) play the presentation
                * EStop - stop the source presentation or open destination presentation in stopped state
                * EPause - pause the source presentation, continue it after, destination finishes
                *
                * 3GPP profile does not require respecting these states, aAnchor may be ignored
                */
                
                void OpenDocumentL(const TDesC& aUrl, const CSmilAnchor* aAnchor, const CSmilPresentation* aPres);

                enum TEventType
                    {
                    EStarted,
                    EStopped,
                    EPaused,
                    EResumed,
                    ESeeked,
                    EStalled,
                    EMediaNotFound,
                    EEndReached,
                    ELastSlideReached
                    };

                /** 
                * Invoked when a presentation state changes. aEvent is 
                * EStarted - presentation started playing,
                * EStopped - presentation was stopped,
                * EPaused - presentation was paused
                * EResumed - paused presentation was resumed
                * ESeeked - presentation was seeked to a new position on timeline
                * EMediaNotFound - opening media failed, aText contains the media url
                * or
                * EEndReached - presentation has reached its end time. whether it is paused or stopped depends
                * on EndAction setting.
                * one of the negative system wide error codes (from a trapped leave inside engine RunL() method). 
                * In this case the playing presentation is stopped automatically and the presentation can not
                * be restarted.
                */

                void PresentationEvent(TInt aEvent, const CSmilPresentation* aPres, const TDesC& aText = TPtrC());

    private:
    
    			MSmilMediaFactory* iMediaFactory;
    			
    			void ConstructL();
    
    
    };
    
class SmilMediaFactory:public MSmilMediaFactory
{
public:

	SmilMediaFactory()
	{
		
		
	}
	/**
	* Sets the base URL that is to be used for resolving relative URLs
	*/
    void SetBaseUrlL(const TDesC& aBaseUrl)
    {
    	
    	//empty definition
    }

	/**
	* Instructs the factory that a media component from the given URL will be
	* needed later. The factory should proceed to fetch at least enough of 
	* the media to be able to instantiate a correct renderer object for it later. 
	* Some factory implementation might start downloading and caching some media 
	* types already because of this call.
	*
	* This method should return fast. If there is need for slow downloads, they should be performed asynchronously.
	* This method is called when the engine sees a new URL, typically during parsing 
	* of SMIL file.
	*/
    void RequestMediaL(const TDesC& aUrl, MSmilMedia* aMedia) 
	{
		    	//empty definition

	}

	/**
	* As above, except with a strong hint that the media should be prefetched 
	* and cached.
	*/
	void PrefetchMediaL(const TDesC& aUrl) 
	{
		
		    	//empty definition

	}

	/**
	 * Called by the engine to signal the factory that a renderer is being deleted.
	 * If the factory maintains any references to the renderer, they must be removed here.
	 * (DEPRECATED)
	 */
	void RendererDeleted(MSmilMediaRenderer* aRenderer)
	{
		
		
	}

	/**
	* The parsing of presentation has finished and initial
	* layout has been calculated.
	*/
	void PresentationReady() 
	{
		
			//empty definition

	}

	/**
	* The presentation has ended (though it might restart).
	* The factory may free some resources, cut connections, etc. 
	*/
	void PresentationEnd() 
	{
				    	//empty definition

	}

	/**
	* Query for supported content type. Returns ETrue if
	* the given type (for example "application/xhtml+xml") is 
	* supported by this factory implementation.
	*
	* The method is used by the SMIL engine for resolving values
	* like "ContentType:application/xhtml+xml" of the 
	* systemComponent content control attribute.
	*/
	 TBool QueryContentType(const TDesC& aMimeType) const 
	{
		
		return ETrue;
	}
	
	TPrefetchStatus PrefetchStatus(const TDesC& aUrl, TInt& aDown, TInt& aSize) ;
	
	TMediaFactoryError CreateRendererL(const TDesC& aUrl, MSmilMedia* aMedia, MSmilMediaRenderer*& aRender); 

};

class SmilMedia:public MSmilMedia
    {
    public:  
    
    	SmilMedia()
    	{
    		
    		
    	}
    

		CSmilPresentation* Presentation() const 
		{
			
			return NULL;
		}

		/**
		* Returns the region where this media is supposed to be rendered on. May be NULL.
		*/ 
		MSmilRegion* GetRegion() const 
		{
			return NULL;
			
		}

		/**
		* Returns the rectangle (region) where the media is supposed to be rendered on.
		*/
		TRect GetRectangle() const
		{
			TRect rect;
			return rect;
		}

		/**
		* Invoked by renderer to ask redrawing of area covered by the media. 
		* This should be invoked when (visual) media appears, there are changes to it, and 
		* when it is removed.
		*/
		void Redraw() 
		{
			
			
		}

		/** 
		* Invoked by a MSmillMediaRenderer object when there is a change 
		* in (known) state of its intrinsic values. For example, duration of 
		* video clip is not typically known until it is at least partially 
		* decoded. This method should be invoked by the renderer when this 
		* information becomes available.
		*/
		void RendererDurationChangedL() 
		{
			
		}

		/**
		* Invoked by a MSmillMediaRenderer object when the associated media 
		* ends (end of the file or stream is reached). Renderer should also 
		* move to frozen state at this point (that is, to keep showing the last 
		* frame). 
		*/
		void RendererAtEndL() 
		{
			
			
		}

		/**
		 * Media URL
		 */
		TPtrC Src() const 
		{
			return TPtrC();

		}

		/**
		 * Alt tag
		 */
		TPtrC Alt() const 
		{
			
			return TPtrC();
		}
		
		/**
		 * Type attribute. Should be used to override the media type 
		 * checking in the media factory, for example
		 * <img src="jee.jpg" type="image/gif"/>
		 * should cause factory to try opening the image as gif.
		 */
		TPtrC Type() const 
		{
			
			return TPtrC();
		}

		/**
		 * Invoke the ActivateL() method of iActive when delay aDelay has passed.
		 */
	void AfterL(MSmilActive* iActive, const TSmilTime& aDelay, TBool aLazy = EFalse) 
	{
		
		
	}

		/**
		 * Cancel a pending request
		 */
	 void CancelActive(MSmilActive* aActive) 
	 {
	 	
	 	
	 }

		/**
		 * Get media parameter, or empty TPtrC if not found
		 */
	TPtrC GetParam(const TDesC& aName) const 
	{
	    
		return TPtrC();
	}
		/**
		 * Returns begin time of media relative to presentation begin time.
		 */
	TSmilTime MediaBegin() const 
	
	{
		TSmilTime smilTime;
		return smilTime;
		
	}
        
        /**
		 * Returns end time of media relative to presentation begin time.
		 */
	TSmilTime MediaEnd() const 
		{
			
			TSmilTime smilTime;
			return smilTime;
		}
    };
    
 class CDerSmilObject:public CSmilObject
    {
           	
           	  
    };
    
    



    
    
    

NONSHARABLE_CLASS(CTestSmilEngine) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestSmilEngine* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTestSmilEngine();
        CDerSmilTransitionFilter *iCDerSmilTransitionFilter;

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );
        
        TInt CTestSmilEngine::RunMethodL( 
    CStifItemParser& aItem );


    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        
			    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CTestSmilEngine( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below. 
        */

        /**
        * Example test method.
        * @since ?Series60_version
        * @param aItem Script line containing parameters.
        * @return Symbian OS error code.
        */
       
         TInt DefaultCtor();
        
         TInt CtorSmilTimeArg();
        
         TInt CtorTIntArg( CStifItemParser& aItem );
         
         TInt EqualityOptr();
        
         TInt NotEqualityOptr();
        
         TInt StarOptr(CStifItemParser& aItem );  
        
         TInt StarRealOptr();
        
         TInt MinusOptr(CStifItemParser& aItem );  
        
         TInt GreaterThanOptr();
        
         TInt LessThanOptr();
        
         TInt GtrEqualThanOptr();
        
         TInt LessThanEqualOptr();
        
         TInt PlusPlusOptr();
				
		 TInt EqualOptr();

		 TInt TIntEqualOptr();
				
         TInt PlusOptr(CStifItemParser& aItem );  
        
         TInt ToMicroSeconds();
        
         TInt FromMicroSeconds();
        
         TInt IntFromMicroSeconds();
        
         TInt SmilParserL();
        
         TInt CreateCSmilTransitionFilterL();
        
         TInt SmilPresentationL();
         
         TInt ConformanceCheckL();
         
         TInt SmilUIPresentationL(CStifItemParser& aItem ); //Added for SMIL UI Player Dialog
        
  //      virtual TInt SmilAnchorAPIs();
        /**
         * Method used to log version of test class
         */
        void SendTestClassVersion();

        //ADD NEW METHOD DEC HERE
        //[TestMethods] - Do not remove

    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
        
        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;

    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // TESTSMILENGINE_H

// End of File
