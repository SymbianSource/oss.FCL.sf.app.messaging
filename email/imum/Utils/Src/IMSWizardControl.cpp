/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*       Control class for mailbox wizard help screen
*
*/


// INCLUDE FILES

#include <eikbctrl.h>
#include <eikrted.h>
#include <AknsUtils.h>
#include <AknsBasicBackgroundControlContext.h>
#include <AknDef.h>
#include <aknappui.h>
#include <aknlayoutscalable_apps.cdl.h>
#include <AknUtils.h>
#include <AknsControlContext.h>
#include <AknsDrawUtils.h>
#include <StringLoader.h>

#include <ImumUtils.rsg>

#include "IMSWizardControl.h"


// MEMBER FUNCTIONS

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//

CIMSWizardControl::CIMSWizardControl()
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CIMSWizardControl::~CIMSWizardControl()
{
    delete iEditor;
    iEditor = NULL;
}



// ---------------------------------------------------------------------------
// CIMSWizardControl::ConstructFromResourceL()
// From CoeControl
// ---------------------------------------------------------------------------
//
void CIMSWizardControl::ConstructFromResourceL( TResourceReader& aReader )
{
    iEditor = new ( ELeave ) CEikRichTextEditor;
    iEditor->SetContainerWindowL( *this );

    iEditor->ConstructFromResourceL( aReader );
    
    // get help screen text
    
    HBufC* helpText = StringLoader::LoadLC( R_IMUM_WIZARD_HELP_SCREEN );
    
    iEditor->SetTextL( helpText );
    CleanupStack::PopAndDestroy( helpText ); 

}

// ---------------------------------------------------------------------------
// CIMSWizardControl::SizeChanged()
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CIMSWizardControl::SizeChanged()
{
    TRect clientrect = Rect();
    
    // create akn layout rectancular
    TAknLayoutRect r;
    // first create whole messaging main pane
    r.LayoutRect( clientrect, AknLayoutScalable_Apps::main_messa_pane( ) );
    // listscroll pane for text rows
    r.LayoutRect( r.Rect(), AknLayoutScalable_Apps::listscroll_mce_pane( ) );
    // one text line pane
    r.LayoutRect( r.Rect(), AknLayoutScalable_Apps::mce_text_pane( 0 ) );

    // count of text rows
    TInt count = 
        AknLayoutScalable_Apps::list_single_mce_message_pane_ParamLimits(
            ).LastRow();
    // array contains all text rows in help screen
    RArray<TAknTextComponentLayout> array;
    
    
    for( int i = 0; i < count; i++ )
        {
        // pane area for one text row
        TAknWindowComponentLayout windowComponentWiz =  
            AknLayoutScalable_Apps::list_single_mce_message_pane(i);
        // smaller pane for text itself
        TAknTextComponentLayout textComponentWiz =  
            AknLayoutScalable_Apps::list_single_mce_message_pane_t1();
    
        // composition of two latter componets
        TAknTextComponentLayout textCompCompositionWiz = 
            TAknWindowComponentLayout::ComposeText(
                windowComponentWiz, textComponentWiz );
 
        TRAPD(error, array.AppendL( textCompCompositionWiz ) );
        // if error in append function, then just exit the loop      
        if ( error != KErrNone )
            {
            break;
            }
        }
    
    TAknMultiLineTextLayout multilineLayout 
        = TAknTextComponentLayout::Multiline( array );         

    AknLayoutUtils::LayoutEdwin( 
        iEditor, 
        r.Rect(), 
        multilineLayout,
        EAknsCIQsnTextColorsCG6 );
        
    array.Close();
}

// ---------------------------------------------------------------------------
// CIMSWizardControl::Draw()
// From CCoeControl.
// ---------------------------------------------------------------------------
//
void CIMSWizardControl::Draw( const TRect& aRect ) const
{
    // draw function for background
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = iBgContext;
    CWindowGc& gc = SystemGc();
    // if we couldn't get background from layouts, then just set some 
    // defaults
    if ( !AknsDrawUtils::Background( skin, cc, this, gc, aRect ) )
        {
        gc.SetPenStyle( CGraphicsContext::ENullPen );
        gc.SetBrushColor( AKN_LAF_COLOR( 0 ) );
        gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc.DrawRect( Rect() );
        } 
}

// ---------------------------------------------------------------------------
// CIMSWizardControl::CountComponentControls()
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CIMSWizardControl::CountComponentControls() const
{
    if ( iEditor )
        {
        return 1;
        }
    return 0;   
}

// ---------------------------------------------------------------------------
// CIMSWizardControl::CountComponentControls()
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CIMSWizardControl::ComponentControl( TInt aIndex ) const
{
    if ( aIndex == 0 )
        {
        return iEditor;   
        }
    return NULL;
}

// ---------------------------------------------------------------------------
// CIMSWizardControl::HandleResourceChange()
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CIMSWizardControl::HandleResourceChange( TInt aType )
{
CCoeControl::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange ||
        aType == KEikDynamicLayoutVariantSwitch )
        {
        SizeChanged();
        }  
}

void CIMSWizardControl::SetBackgroundContext( MAknsControlContext* aContext )
{
    iBgContext = aContext;
    TRAP_IGNORE( iEditor->SetSkinBackgroundControlContextL( iBgContext ) );
}
