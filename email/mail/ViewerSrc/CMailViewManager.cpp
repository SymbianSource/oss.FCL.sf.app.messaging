/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plug-in view manager
*
*/



// INCLUDE FILES
#include 	"MailLog.h"
#include    "CMailViewManager.h"
#include    "CMailViewFactory.h"
#include 	"CMailBaseControlAdapter.h"
#include    "CMailMsgBaseControlAdapter.h"
#include    "MMailMsgBaseControl.h"

#include 	<MsgBaseControl.h>
#include 	<MsgEditorView.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMailViewManager::CMailViewManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMailViewManager::CMailViewManager()
    {
    }

// -----------------------------------------------------------------------------
// CMailViewManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMailViewManager::ConstructL()
    {
    iMailViewArray = MailViewFactory::CreateViewPluginsL();
    }

// -----------------------------------------------------------------------------
// CMailViewManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMailViewManager* CMailViewManager::NewL()
    {
    CMailViewManager* self = new( ELeave ) CMailViewManager;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CMailViewManager::~CMailViewManager()
    {
    if (iMailViewArray)
        {
        iMailViewArray->ResetAndDestroy();	
        delete iMailViewArray;
        }
    delete iAdapter;
    REComSession::FinalClose();
    }


// -----------------------------------------------------------------------------
// CMailViewManager::MessageLoadL
// -----------------------------------------------------------------------------
//
void CMailViewManager::MessageLoadL( TInt aStatus, CMailMessage& aMessage )
    {
    for (TInt i=0; i<iMailViewArray->Count(); i++)
        {
        CMailMessageView* view = iMailViewArray->At(i);
        
        if(view)
        	{
        	LOG2("CMailViewManager::MessageLoadL @ aStatus: %d, i:%d", 
        		aStatus, i);
        	TRAPD(error, view->MessageLoadL(aStatus, aMessage));

       		if (error != KErrNone)
       			{
       			// remove problem plugin
       			delete view;
       			iMailViewArray->Delete(i);
       			iMailViewArray->Compress();
       			}
        	}
        }   
    }
    
// -----------------------------------------------------------------------------
// CMailViewManager::AddBodyControlL
// -----------------------------------------------------------------------------
//
void CMailViewManager::AddBodyControlL( 
	CMsgEditorView* aParent, CCoeControl& aControl )
    {    
    CMsgBaseControl* adapter = CMailBaseControlAdapter::NewL(aParent, aControl);
    CleanupStack::PushL(adapter);
        
    AddBodyControlL(aParent, *adapter);    
        
    CleanupStack::Pop(); // adapter
    }

// -----------------------------------------------------------------------------
// CMailViewManager::AddBodyControlL
// -----------------------------------------------------------------------------
//
void CMailViewManager::AddBodyControlL( 
	CMsgEditorView* aParent, MMailMsgBaseControl& aControl )
    {
    CMsgBaseControl* adapter = 
        CMailMsgBaseControlAdapter::NewL(aParent, aControl);             
    CleanupStack::PushL(adapter);
        
    AddBodyControlL(aParent, *adapter);     
    CleanupStack::Pop(); // adapter
    }
  
// -----------------------------------------------------------------------------
// CMailViewManager::ViewPlugin
// -----------------------------------------------------------------------------
//
CMailMessageView* CMailViewManager::ViewPlugin()
    {
     // Use first view plugin
 	return PluginCount() ? iMailViewArray->At(0) : NULL;
    }

// -----------------------------------------------------------------------------
// CMailViewManager::PluginCount
// -----------------------------------------------------------------------------
//    
TInt CMailViewManager::PluginCount()
	{
	return iMailViewArray->Count();
	}
	
// -----------------------------------------------------------------------------
// CMailViewManager::RemovePlugin
// -----------------------------------------------------------------------------
//    
void CMailViewManager::RemovePlugin()
	{
	// remove first plugin
	if (PluginCount())
		{
		delete iMailViewArray->At(0);
		iMailViewArray->Delete(0);
		iMailViewArray->Compress();	
		}
	
	}  
	
// -----------------------------------------------------------------------------
// CMailViewManager::AddBodyControlL
// -----------------------------------------------------------------------------
//
void CMailViewManager::AddBodyControlL( 
	CMsgEditorView* aParent, CMsgBaseControl& aControl )
    {           
    // replace defaul base body control
    CMsgBaseControl* bodycontrol = aParent->RemoveControlL(EMsgComponentIdBody);
    delete bodycontrol;
    bodycontrol = NULL;
    
    aParent->AddControlL(
        &aControl,
        EMsgComponentIdBody,
        EMsgAppendControl, 
        EMsgBody);
    }  	
//  End of File  
