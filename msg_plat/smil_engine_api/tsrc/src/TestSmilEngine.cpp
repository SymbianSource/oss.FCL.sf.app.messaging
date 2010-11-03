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
* Description:   TestSmilEngine implementation*
*/




// INCLUDE FILES
#include <Stiftestinterface.h>
#include "TestSmilEngine.h"
#include <SettingServerClient.h>
#include <GMXMLDocument.h>
#include <GMXMLParser.h>
#include <smilmediafactoryinterface.h>


CTestSmilEngine::CTestSmilEngine( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    }

// -----------------------------------------------------------------------------
// CTestSmilEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTestSmilEngine::ConstructL()
    {
    //Read logger settings to check whether test case name is to be
    //appended to log file name.
    RSettingServer settingServer;
    TInt ret = settingServer.Connect();
    if(ret != KErrNone)
        {
        User::Leave(ret);
        }
    // Struct to StifLogger settigs.
    TLoggerSettings loggerSettings; 
    // Parse StifLogger defaults from STIF initialization file.
    ret = settingServer.GetLoggerSettings(loggerSettings);
    if(ret != KErrNone)
        {
        User::Leave(ret);
        } 
    // Close Setting server session
    settingServer.Close();

    TFileName logFileName;
    
    if(loggerSettings.iAddTestCaseTitle)
        {
        TName title;
        TestModuleIf().GetTestCaseTitleL(title);
        logFileName.Format(KTestSmilEngineLogFileWithTitle, &title);
        }
    else
        {
        logFileName.Copy(KTestSmilEngineLogFile);
        }

    iLog = CStifLogger::NewL( KTestSmilEngineLogPath, 
                          logFileName,
                          CStifLogger::ETxt,
                          CStifLogger::EFile,
                          EFalse );
    
    SendTestClassVersion();
    }

// -----------------------------------------------------------------------------
// CTestSmilEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTestSmilEngine* CTestSmilEngine::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CTestSmilEngine* self = new (ELeave) CTestSmilEngine( aTestModuleIf );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;

    }

// Destructor
CTestSmilEngine::~CTestSmilEngine()
    { 

    // Delete resources allocated from test methods
    Delete();

    // Delete logger
    delete iLog; 

    }

//-----------------------------------------------------------------------------
// CTestSmilEngine::SendTestClassVersion
// Method used to send version of test class
//-----------------------------------------------------------------------------
//
void CTestSmilEngine::SendTestClassVersion()
	{
	TVersion moduleVersion;
	moduleVersion.iMajor = TEST_CLASS_VERSION_MAJOR;
	moduleVersion.iMinor = TEST_CLASS_VERSION_MINOR;
	moduleVersion.iBuild = TEST_CLASS_VERSION_BUILD;
	
	TFileName moduleName;
	moduleName = _L("TestSmilEngine.dll");

	TBool newVersionOfMethod = ETrue;
	TestModuleIf().SendTestModuleVersion(moduleVersion, moduleName, newVersionOfMethod);
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {

    return ( CScriptBase* ) CTestSmilEngine::NewL( aTestModuleIf );

    }
    
 void CDerSmilTransitionFilter::Draw(CGraphicsContext& aGc, const TRect& aRect, const CFbsBitmap* aTargetBitmap, const TPoint& aTarget, const CFbsBitmap* aMaskBitmap)
{
	
	
	// definition for pure virtual functions
	
}

TInt CDerSmilTransitionFilter::ResolutionMilliseconds()const
{
					return KErrNone;
  				// definition for pure virtual functions
}

CDerSmilTransitionFilter::CDerSmilTransitionFilter()
{
	
}

void CDerSmilTransitionFilter::TransitionsL()
{
	TransitionStartingL();
	TransitionEnding();
}


SmilPlayer::SmilPlayer()
{
    
       
}

MSmilPlayer* SmilPlayer::NewLC()
{
	SmilPlayer* self = new ( ELeave ) SmilPlayer();
	
	self->ConstructL();
    
    return self;
	
}



void SmilPlayer::ConstructL()
{
	
	iMediaFactory=new(ELeave)SmilMediaFactory;
	
}

// ----------------------------------------------------------------------------
// SmilPlayer::GetMediaFactory
// Returns media factory (implementation from SMIL Engine).
// ----------------------------------------------------------------------------
 MSmilMediaFactory* SmilPlayer::GetMediaFactory(
                            const CSmilPresentation* /*aPres*/) const
    {
    	return iMediaFactory;
    }

// ----------------------------------------------------------------------------
// SmilPlayer::GetTransitionFactory
// Returns transition effect factory (implementation from SMIL Engine).
// ----------------------------------------------------------------------------
//
 MSmilTransitionFactory* SmilPlayer::GetTransitionFactory( 
                             const CSmilPresentation* /*aPres*/ ) const
    {
    return NULL;//MSmilTransitionFactory* TransitionFactory;
    }

// ----------------------------------------------------------------------------
// SmilPlayer::EvaluateContentControlAttribute
// ----------------------------------------------------------------------------
 TBool SmilPlayer::EvaluateContentControlAttribute(
                            const TDesC& /*aName*/, const TDesC& /*aValue*/)
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
// SmilPlayer::GetDimensions
// ----------------------------------------------------------------------------
 TRect SmilPlayer::GetDimensions(
                            const CSmilPresentation* /*aPres*/ )
    {
    TRect Rect;
    return Rect;
    }

// ----------------------------------------------------------------------------
// SmilPlayer::GetControl
// ----------------------------------------------------------------------------
 CCoeControl* SmilPlayer::GetControl(
                            const CSmilPresentation* /*aPres*/ )
    {
    return NULL;//CCoeControl* PresentationControl;
    }

// ----------------------------------------------------------------------------
// SmilPlayer::Redraw
// ----------------------------------------------------------------------------
 void SmilPlayer::Redraw(TRect aArea, CSmilPresentation* aPres )
    {
    
    }



void SmilPlayer::OpenDocumentL(const TDesC& aUrl, const CSmilAnchor* aAnchor, const CSmilPresentation* aPres)

{
	//No definiton
}

void SmilPlayer::PresentationEvent(TInt aEvent, const CSmilPresentation* aPres, const TDesC& aText)

{
	//No definiton
}

	/**
	* Returns the current status of media prefetching of given URL. 
	* This includes amount downloaded so far (aDown) and total size 
	* (aSize, if known, 0 otherwise) in bytes.
	* 
	* Return values:
	* ENone - no prefetch has been asked
	* EWaiting - prefetch is waiting, for example to establish network connection
	* EPrefeching - prefetch is in progress
	* EReady - prefetch has completed
	* ENotFound - given URL was not found
	* EConnectionError - getting the URL failed, possibly by timeout
	*/
MSmilMediaFactory::TPrefetchStatus SmilMediaFactory::PrefetchStatus(const TDesC& aUrl, TInt& aDown, TInt& aSize) 
	{
		
		return EPrefetchNone;
	}

	/**
	* Creates a renderer object for a given media URL. If the method fails, 
	* an error code is returned. Media may not be fully loaded when this 
	* method completes (in case of a stream it can't be), but at least enough 
	* must be known about it to construct a correct media specific renderer 
	* object. 
	*
	* This method is synchronous and might block for a while. The 
	* ownership of the returned renderer object is moved to whoever called 
	* this method.
	*
	* Return values:
	* EOk - renderer created succefully
	* ENoRenderer - there is no renderer object available for this media type
	* ENotFound - given URL was not found
	* EConnectionError - getting the URL failed, possibly by timeout
	*/
MSmilMediaFactory::TMediaFactoryError SmilMediaFactory::CreateRendererL(const TDesC& aUrl, MSmilMedia* aMedia, MSmilMediaRenderer*& aRender) 
	{
		
		return EOk;
	}

