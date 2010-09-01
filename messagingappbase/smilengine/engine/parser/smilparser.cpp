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
* Description: smilparser implementation
*
*/



#include "smilparser.h"

#include "smilsequence.h"
#include "smilparallel.h"
#include "smilexclusive.h"
#include "smilmedia.h"
#include "smilregion.h"
#include "smilrootregion.h"
#include "smilpresentation.h"
#include "smilanchor.h"
#include "smilarea.h"
#include "smila.h"
#include "smiltransition.h"
#include "smilprefetch.h"

#include "smilmediafactoryinterface.h"

#include "smilliterals.h"
#include "smillinkedlist.h"

// maximum number of elements allowed in the timegraph
#define MAX_TIMEGRAPH_SIZE 80

// what it says. only used for resolving the content control attribute
#define SYSTEM_SCREEN_DEPTH 16

//#define PARSER_DEBUG

// defined elsewhere
void ParseXmlL( CSmilParser* parser, const TDesC& aSource );
void ParseDomL( CSmilParser* parser, CMDXMLDocument* aSource );


// ----------------------------------------------------------------------------
// CSmilParser::CSmilParser
// ----------------------------------------------------------------------------
//
CSmilParser::CSmilParser()
    {    
    }

// ----------------------------------------------------------------------------
// CSmilParser::~CSmilParser
// ----------------------------------------------------------------------------
//
CSmilParser::~CSmilParser()
    {
    delete iSwitchStack;
    delete iCurrent; // not yet part of a presentation!
    delete iCurrentRegion;
    
    if ( iNamespaces )
        {
        CLinkedList<Namespace>::Iter nsi( *iNamespaces );
        while( nsi.HasMore() )
            {
            Namespace& ns = nsi.Next();
            delete ns.name;
            delete ns.uri;
            }
        }
        
    delete iNamespaces;
    }

// ----------------------------------------------------------------------------
// CSmilParser::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CSmilParser* CSmilParser::NewL( MSmilPlayer* aPlayer )
    {
    CSmilParser* self = new( ELeave ) CSmilParser();
    
    CleanupStack::PushL( self );
    self->ConstructL( aPlayer );
    CleanupStack::Pop( self );
     
    return self;    
    }

// ----------------------------------------------------------------------------
// CSmilParser::ConstructL
// ----------------------------------------------------------------------------
//
void CSmilParser::ConstructL( MSmilPlayer* aPlayer )
    {
    iPlayer = aPlayer;

    iCurrent = 0;
    iCurrentRegion = 0;
    iSwitchStack = new( ELeave ) CLinkedList<Switch>();
    iNamespaces = new( ELeave ) CLinkedList<Namespace>();
    }

// ----------------------------------------------------------------------------
// CSmilParser::Init
// ----------------------------------------------------------------------------
//
void CSmilParser::Init()
    {
    iState = EInitial;
    
    delete iCurrent;  //might be some leftovers from a failed parsi
    iCurrent = 0;
    
    iTimeContainer = 0;
    
    delete iCurrentRegion;
    iCurrentRegion = 0;
    
    iCurrentTransition = 0;
    
    if ( iAnchor )
        {
        iAnchor->Close();
        iAnchor = 0;
        }

    CLinkedList<Namespace>::Iter nsi( *iNamespaces );
    while ( nsi.HasMore() )
        {
        Namespace& ns = nsi.Next();
        delete ns.name;
        delete ns.uri;
        }
    iNamespaces->Empty();

    iArea = 0;

    iIgnoreDepth = 0;
    iDepth = 0;
    iSwitchStack->Empty();
    iSkipContent = ETrue;
    iUnknownElement = EFalse;
    iTag.Set( TPtrC() );
    iParamName.Set( TPtrC() );
    iParamValue.Set( TPtrC() );
    iTimegraphSize = 0;
    }


// ---------------------------------------------------------
// CSmilParser::ParseL
// 
// ---------------------------------------------------------
//
EXPORT_C CSmilPresentation* CSmilParser::ParseL( const TDesC& /*aSmil*/, 
                                                 const TDesC& /*aBaseUrl*/ )
    {
#if 0
    // can't parse descriptor with current Symbian DOM parser!!!
#ifdef PARSER_DEBUG
    RDebug::Print(_L("ParseL(): %S"), &aSmil);
#endif    
    
    Init();
    
    CSmilPresentation* tempPresentationPointer = CSmilPresentation::NewL( iPlayer );

    // Everything constructed during parsing is owned by the presentation
    // (directly or indirectly) and added there immediatly after construction.
    // Thus it is enough to take care that the presentation gets deleted if 
    // a leave occurs.

    CleanupStack::PushL( tempPresentationPointer );    

    tempPresentationPointer->SetBaseUrlL( aBaseUrl );    
    tempPresentationPointer->SetMaxDownUpScaling( iDown, iUp );    
    iPlayer->GetMediaFactory(tempPresentationPointer)->SetBaseUrlL( aBaseUrl );    
    
    iPresentation = tempPresentationPointer;    
    TBool ok = ParseXml( this, aSmil ); //parse! (see smilparser_cxml.cpp etc.)
    
    if ( ok && iState == ESmilFinished )
        {
#ifdef PARSER_DEBUG
        RDebug::Print(_L("Parsed succesfully"));
#endif
        tempPresentationPointer->Ready();    
        
#ifdef PARSER_DEBUG
        tempPresentationPointer->PrintDebug();    
        
#endif
        CleanupStack::Pop(); // tempPresentationPointer
        }
    else
        {
#ifdef PARSER_DEBUG
        RDebug::Print(_L("Parsing failed, leaving"));
#endif        
        User::Leave( KErrUnknown );
        }
    return tempPresentationPointer;    
    
#endif
    return 0;
    }

// ----------------------------------------------------------------------------
// CSmilParser::ParseL
// ----------------------------------------------------------------------------
//
EXPORT_C CSmilPresentation* CSmilParser::ParseL( const CMDXMLDocument* aSmilDoc, 
                                                 const TDesC& aBaseUrl )
    {
#ifdef PARSER_DEBUG
    RDebug::Print(_L("ParseL(): parsing DOM"));
#endif

    Init();
    
    CSmilPresentation* tempPresentationPointer = CSmilPresentation::NewL( iPlayer );

    // Everything constructed during parsing is owned by the presentation
    // (directly or indirectly) and added there immediatly after construction.
    // Thus it is enough to take care that the presentation gets deleted if 
    // a leave occurs.

    CleanupStack::PushL( tempPresentationPointer );    
    
    tempPresentationPointer->SetBaseUrlL( aBaseUrl );    
    tempPresentationPointer->SetMaxDownUpScaling( iDown, iUp );    
    iPlayer->GetMediaFactory(tempPresentationPointer)->SetBaseUrlL( aBaseUrl );    
    
    iPresentation = tempPresentationPointer;    
    
    ParseDomL( this, const_cast<CMDXMLDocument*>( aSmilDoc ) ); //parse! (see smilparser_dom.cpp)
    
    if ( iState == ESmilFinished )
        {
#ifdef PARSER_DEBUG
        RDebug::Print(_L("Parsed succesfully"));
#endif
        tempPresentationPointer->ReadyL();    

#ifdef PARSER_DEBUG
        tempPresentationPointer->PrintDebug();    
#endif
        CleanupStack::Pop(); // tempPresentationPointer
        }
    else
        {
#ifdef PARSER_DEBUG
        RDebug::Print(_L("Parsing failed, leaving"));
#endif    
        User::Leave( KErrUnknown );
        }

    return tempPresentationPointer;
    }
    
// ----------------------------------------------------------------------------
// CSmilParser::BeginElementL
// ----------------------------------------------------------------------------
//
void CSmilParser::BeginElementL( const TDesC& aName )
    {
#ifdef PARSER_DEBUG
    RDebug::Print(_L("BeginElement: %S"), &aName);
#endif

    iDepth++;

    if ( iIgnoreDepth > 0 )
        {
        iIgnoreDepth++;
        return;
        }

    if ( !iSwitchStack->IsEmpty() && 
         iSwitchStack->Peek().depth == iDepth - 1 && 
         iSwitchStack->Peek().done )
        {
        iIgnoreDepth = 1;
        return;
        }

    if ( aName.CompareF( KSmilTag ) == 0 )
        {        
        if ( iState != EInitial )
            {
            User::Leave( KErrGeneral );
            }
    
        iState = ESmil;
        iTag.Set( KSmilTag );

        // we need root layout even if there is no <layout> element
        CSmilRootRegion* root = CSmilRootRegion::NewL( iPresentation );
        iPresentation->SetLayout( root );        
        }
    else if ( aName.CompareF( KRegionTag ) == 0 )
        {
        if ( iState != ELayout )
            {
            User::Leave( KErrGeneral );
            }

        iTag.Set( KRegionTag );
        CSmilRegion* r = CSmilRegion::NewL( iPresentation );
        iCurrentRegion = r;
        }
    else if ( aName.CompareF( KHeadTag ) == 0 )
        {
        if (iState != ESmil)
            {
            User::Leave( KErrGeneral );
            }

        iState = EHead;
        iTag.Set( KHeadTag );
        }
    else if ( aName.CompareF( KLayoutTag ) == 0 )
        {
        if ( iState != EHead )
            {
            User::Leave( KErrGeneral );
            }

        iState = ELayout;
        iTag.Set( KLayoutTag );
        iPresentation->GetLayout()->SetDefaultLayout( EFalse );
        }
    else if ( aName.CompareF( KRootLayoutTag ) == 0 && 
              iState == ELayout )
        {
        if ( iState != ELayout )
            {
            User::Leave( KErrGeneral );
            }

        iTag.Set( KRootLayoutTag );
        iCurrentRegion = iPresentation->GetLayout();
        }    
    else if ( aName.CompareF( KBodyTag ) == 0 )
        {        
        if ( iState != ESmil && 
             iState != EHeadFinished )
            {
            User::Leave( KErrGeneral );
            }

        iTag.Set( KBodyTag );
        CSmilSequence* body = CSmilSequence::NewL( iPresentation );
        iPresentation->SetTimegraph( body );
        iCurrent = body;

        iState = EBody;
        }
    else if ( aName.CompareF( KSeqTag ) == 0 )
        {
        if ( iState != EBody )
            {
            User::Leave(KErrGeneral);
            }

        iTag.Set( KSeqTag );
        iCurrent = CSmilSequence::NewL( iPresentation );
        }    
    else if ( aName.CompareF( KParTag ) == 0 )
        {
        if ( iState != EBody )
            {
            User::Leave(KErrGeneral);
            }

        iTag.Set( KParTag );
        iCurrent = CSmilParallel::NewL( iPresentation );        
        }
    else if ( aName.CompareF( KExclTag ) == 0 )
        {
        if ( iState != EBody )
            {
            User::Leave( KErrGeneral );
            } 

        iTag.Set( KExclTag );
        iCurrent = CSmilExclusive::NewL( iPresentation );
        }
    else if ( aName.CompareF( KTransitionTag ) == 0 )
        {
        if ( iState != EHead && 
             iState != ELayoutFinished )
            {
            User::Leave( KErrGeneral );
            }

        iTag.Set( KTransitionTag );
        
        iCurrentTransition = new (ELeave) CSmilTransition();
        iPresentation->AddTransitionL( iCurrentTransition );
        }
    else if ( aName.CompareF( KImageTag ) == 0 ||
              aName.CompareF( KTextTag ) == 0 ||
              aName.CompareF( KAudioTag ) == 0 ||
              aName.CompareF( KRefTag ) == 0 ||
              aName.CompareF( KTextStreamTag ) == 0 ||
              aName.CompareF( KAnimationTag ) == 0 ||
              aName.CompareF( KVideoTag ) == 0 )
        {
        if ( iState != EBody )
            {
            User::Leave(KErrGeneral);
            }

        iTag.Set( KRefTag );
        iCurrent = CSmilMedia::NewL( iPresentation );    

        if ( iAnchor )
            {
            iCurrent->SetAnchor( iAnchor );
            }
        }
    else if ( aName.CompareF( KParamTag ) == 0 )
        {
        iTag.Set( KParamTag );
        }
    else if ( aName.CompareF( KSwitchTag ) == 0 )
        {
        iTag.Set( KSwitchTag );
        
        Switch sw;
        sw.depth = iDepth;
        sw.done = EFalse;
        iSwitchStack->PushL( sw );
        }
    else if ( aName.CompareF( KATag ) == 0 )
        {
        iTag.Set( KATag );

        if ( iAnchor )
            {
            iAnchor->Close();
            iAnchor = 0;
            }

        iAnchor = new( ELeave ) CSmilAnchor();
        }
    else if ( aName.CompareF( KAreaTag ) == 0 || 
              aName.CompareF( KAnchorTag ) == 0)
        {
        iTag.Set( KAreaTag );
                
        iArea = CSmilArea::NewL( iPresentation );    
        iArea->SetTimeContainer( iTimeContainer );
        iCurrent = iArea;
        }
    else if ( aName.CompareF( KPrefetchTag ) == 0 )
        {
        if ( iState != EBody )
            {
            User::Leave(KErrGeneral);
            }

        iTag.Set( KPrefetchTag );
        iCurrent = CSmilPrefetch::NewL( iPresentation );
        }    
    else
        {
        if ( iState != EInitial )
            {
            iUnknownElement = ETrue;
            }
        }

    return;
    }

// ----------------------------------------------------------------------------
// CSmilParser::EndElementL
// ----------------------------------------------------------------------------
//
void CSmilParser::EndElementL( const TDesC& aName )
    {
#ifdef PARSER_DEBUG
    RDebug::Print( _L( "EndElement: %S" ), &aName );
#endif

    iDepth--;

    if ( iIgnoreDepth > 0 )
        {
        iIgnoreDepth--;
        return;
        }

    if ( aName.CompareF( KSmilTag ) == 0 )
        {
        if ( iPresentation && 
             iPresentation->GetTimegraph() && 
             iPresentation->GetLayout() )
            {
            iState = ESmilFinished;
            }
        } 
    else if ( aName.CompareF( KHeadTag ) == 0 )
        {
        iState = EHeadFinished;
        } 
    else if ( aName.CompareF( KTransitionTag ) == 0 )
        {
        iCurrentTransition = 0;
        }
    else if ( aName.CompareF( KLayoutTag ) == 0 )
        {
        iState = ELayoutFinished;
        iCurrentRegion = 0;
        }
    else if ( aName.CompareF( KSwitchTag ) == 0 )
        {           
        if ( !iSwitchStack->IsEmpty() )
            {            
            iSwitchStack->Pop();
            }
        }
    else if ( aName.CompareF( KBodyTag ) == 0 )
        {
        iState = EBodyFinished;

        iCurrent = 0;
        iTimeContainer = 0;
        } 

    else if ( aName.CompareF( KATag ) == 0 )
        {
        if ( iAnchor )
            {
            if (iAnchor->AccessCount()==1 && iTimeContainer)
                {
                // empty <a> element, we'll create a place holder SmilObject
                CSmilA* a = CSmilA::NewL( iPresentation ); 
                a->SetAnchor( iAnchor );
                iTimeContainer->AddChild( a );
                a->AttachedL();
                }
            iAnchor->Close();        
            iAnchor = 0;
            }
        } 

    else if ( aName.CompareF( KParamTag ) == 0 )
        {
        iParamName.Set( TPtrC() );
        iParamValue.Set( TPtrC() );
        }

    else if ( ( aName.CompareF( KParTag ) == 0 || 
                aName.CompareF( KSeqTag ) == 0 || 
                aName.CompareF( KExclTag ) == 0 ) && 
              iTimeContainer )
        {        
        iTimeContainer = iTimeContainer->ParentObject();
        }

    else if ( ( aName.CompareF( KImageTag ) == 0 ||
                aName.CompareF( KTextTag ) == 0 || 
                aName.CompareF( KAudioTag ) == 0 ||
                aName.CompareF( KRefTag ) == 0 ||
                aName.CompareF( KTextStreamTag ) == 0 ||
                aName.CompareF( KAnimationTag ) == 0 || 
                aName.CompareF( KVideoTag ) == 0 ) && 
              iTimeContainer )
        {
        iTimeContainer = iTimeContainer->ParentObject();
        }

    return;
    }

// ----------------------------------------------------------------------------
// StripWhitespace
// ----------------------------------------------------------------------------
//
TPtrC StripWhitespace( const TDesC& aStr )
    {
    // strip leading and trailing spaces from a string
    TInt b = 0;
    TInt e = aStr.Length() - 1;
    while ( b < aStr.Length() && aStr[b] == ' ' )
        {
        b++;
        }
    while ( e >= 0 && aStr[e] == ' ' )
        {
        e--;
        }
        
    if ( e >= b )
        {
        return aStr.Mid( b, e - b + 1 );
        }
    else
        {
        return TPtrC();
        }
    }

// ----------------------------------------------------------------------------
// CSmilParser::AttributeValueL
// ----------------------------------------------------------------------------
//
void CSmilParser::AttributeValueL( const TDesC& aName, const TDesC& aAttrValue )    
    {
#ifdef PARSER_DEBUG
    RDebug::Print( _L( "attrib: %S=%S" ), &aName, &aValue );
#endif

    if ( iIgnoreDepth > 0 )
        {
        return;
        }

    const TPtrC aValue = StripWhitespace( aAttrValue );

    if ( aName.Length() == 0 )
        {
        return;
        }

    if ( CheckSystemAttribute( aName, aValue ) )
        {
        return;
        }

    if ( aName.CompareF( KSkipContentAttr ) == 0 )
        {
        if ( aValue.CompareF( KTrueVal ) == 0 )
            {
            iSkipContent = ETrue;
            }
        else if ( aValue.CompareF( KFalseVal ) == 0 )
            {
            iSkipContent = EFalse;
            }
        }

    switch ( iState )
        {
        case EBody:
            {
            if ( ( iAnchor && iTag == KATag ) || 
                 ( iArea && iTag == KAreaTag ) )
                {
                CSmilAnchor* a = iArea ? iArea->Anchor() : iAnchor;
                
                if ( aName.CompareF( KHrefAttr ) == 0 )
                    {
                    a->SetHrefL( aValue );
                    }
                else if ( aName.CompareF( KAltAttr ) == 0 )
                    {
                    a->SetAltL( aValue );
                    }
                else if ( aName.CompareF( KTargetAttr ) == 0 )
                    {
                    a->SetTargetL( aValue );
                    }
                else if ( aName.CompareF( KTabindexAttr ) == 0 )
                    {
                    a->iTabIndex = StringToIntValue( aValue,0 );
                    }
                else if (aName.CompareF( KActuateAttr ) == 0 )
                    {
                    if ( aValue.CompareF( KOnLoadVal ) == 0 )
                        {
                        a->iActuate = CSmilAnchor::EOnLoad;
                        }
                    }
                else if ( aName.CompareF( KAccesskeyAttr ) == 0 )
                    {
                    if ( aValue.Length() > 0 )
                        {
                        a->iAccessKey = aValue[0];
                        }
                    }
                else if ( iArea )
                    {
                    if ( aName.CompareF( KShapeAttr ) == 0 )
                        {                    
                        if ( aValue.CompareF( KCircleVal ) == 0 )
                            {
                            iArea->iShape = CSmilArea::ECircle;
                            }
                        else if ( aValue.CompareF( KPolyVal ) == 0 )
                            {
                            iArea->iShape = CSmilArea::EPoly;
                            }
                        else if ( aValue.CompareF( KRectVal ) == 0 )
                            {
                            iArea->iShape = CSmilArea::ERect;
                            }
                        }
                    else if ( aName.CompareF( KCoordsAttr ) == 0 )
                        {
                        TInt b = 0;
                        TInt e = 0;
                        
                        while ( e < aValue.Length() )
                            {
                            while ( e < aValue.Length() && aValue[e] != ',' )
                                {
                                e++;
                                }
                                
                            while ( aValue[b] == ' ' )
                                {
                                b++;
                                }
                                
                            TPtrC val = aValue.Mid( b, e - b );
                            
                            iArea->iCoords->AddL( TSmilLength( val ) );
                            
                            e++;
                            b = e;        
                            }
                        }
                    }

                if ( !iArea )
                    {
                    return;
                    }
                }

            else if (iTag == KParamTag )
                {
                if ( aName.CompareF( KNameAttr ) == 0 )
                    {
                    iParamName.Set( aValue );
                    }
                else if ( aName.CompareF( KValueAttr ) == 0 )
                    {
                    iParamValue.Set( aValue );
                    }
                    
                return;
                }        

            if ( !iCurrent )
                {
                return;
                }

            if ( aName.CompareF( KDurAttr ) == 0 )
                {
                iCurrent->SetDuration( TSmilTime( aValue ) );
                }
            else if ( aName.CompareF( KBeginAttr ) == 0 )
                {
                ParseTimeListL( aValue, ETrue );
                }
            else if ( aName.CompareF( KEndAttr ) == 0 )
                {
                ParseTimeListL( aValue, EFalse );
                }
            else if ( aName.CompareF( KRepeatDurAttr ) == 0 )
                {
                iCurrent->SetRepeatDur( TSmilTime( aValue ) );
                }
            else if ( aName.CompareF( KMinAttr ) == 0 )
                {
                iCurrent->SetMin( TSmilTime( aValue ) );
                }
            else if ( aName.CompareF( KMaxAttr ) == 0 )
                {
                iCurrent->SetMax( TSmilTime( aValue ) );
                }
            else if ( aName.CompareF( KFillAttr ) == 0 )
                {
                if ( aValue.CompareF( KRemoveVal ) == 0 )
                    {
                    iCurrent->SetFill( EFillRemove );
                    }
                else if ( aValue.CompareF( KFreezeVal ) == 0 )
                    {
                    iCurrent->SetFill( EFillFreeze );
                    }
                else if (aValue.CompareF( KHoldVal ) == 0 )
                    {
                    iCurrent->SetFill( EFillHold );
                    }
                else if ( aValue.CompareF( KTransitionVal ) == 0 )
                    {
                    iCurrent->SetFill( EFillTransition );
                    }
                }
            else if ( aName.CompareF( KEndsyncAttr ) == 0 )
                {
                if ( aValue.CompareF( KFirstVal ) == 0 )
                    {
                    iCurrent->SetEndsyncL( EEndsyncFirst );
                    }
                else if (aValue.CompareF( KLastVal ) == 0 )
                    {
                    iCurrent->SetEndsyncL( EEndsyncLast );
                    }
                else if (aValue.CompareF( KAllVal ) == 0 )
                    {
                    iCurrent->SetEndsyncL( EEndsyncAll );
                    }
                else
                    {
                    iCurrent->SetEndsyncL( EEndsyncId, aValue );
                    }
                }
            else if ( aName.CompareF( KRestartAttr ) == 0 )
                {
                if ( aValue.CompareF( KNeverVal ) == 0 )
                    {
                    iCurrent->SetRestart( ERestartNever );
                    }
                else if (aValue.CompareF( KWhenNotActiveVal ) == 0 )
                    {
                    iCurrent->SetRestart( ERestartWhenNotActive );
                    }
                else 
                    {
                    iCurrent->SetRestart(ERestartAlways);
                    }
                }
            else if ( aName.CompareF( KRepeatCountAttr ) == 0 || 
                      aName.CompareF( KRepeatAttr ) == 0 )
                {
                TReal32 rc = StringToRealValue( aValue, 1 );
                
                if ( aValue.CompareF( KIndefiniteVal ) == 0 )
                    {
                    iCurrent->SetRepeatCount( KFloatIndefinite );
                    }
                else if ( rc > 0 )
                    {
                    iCurrent->SetRepeatCount( rc );
                    }
                }
            else if ( aName.CompareF( KIdAttr ) == 0 )
                {
                iCurrent->SetIdL( aValue );
                }
            else if ( iCurrent->IsMedia() )
                {
                CSmilMedia* m = static_cast<CSmilMedia*>( iCurrent );
                if ( aName.CompareF( KRegionAttr ) == 0 )
                    {
                    if ( iPresentation->GetLayout() )
                        {
                        CSmilRegion* r = iPresentation->GetLayout()->FindRegion( aValue );
                        if ( r )
                            {
                            m->SetRegion( r );
                            }
                        }
                    }
                else if ( aName.CompareF( KSrcAttr ) == 0 )
                    {
                    m->SetSrcL( aValue );
                    }
                else if ( aName.CompareF( KAltAttr ) == 0 )
                    {
                    m->SetAltL( aValue );
                    }
                else if ( aName.CompareF( KTransInAttr ) == 0 )
                    {
                    m->iTransitionIn = iPresentation->FindTransition( aValue );            
                    }
                else if ( aName.CompareF( KTransOutAttr ) == 0 )
                    {
                    m->iTransitionOut = iPresentation->FindTransition( aValue );            
                    }
                else if ( aName.CompareF( KClipBeginAttr ) == 0 || 
                          aName.CompareF( KClipBegin2Attr ) == 0 )
                    {
                    TSmilTime tmp( aValue );
                    if ( tmp.IsFinite() )
                        {
                        m->iClipBegin = tmp;
                        }
                        
                    // SMTP values
                    }
                else if ( aName.CompareF( KClipEndAttr ) == 0 || 
                          aName.CompareF( KClipEnd2Attr ) == 0 )
                    {
                    TSmilTime tmp( aValue );
                    if ( tmp.IsFinite() )
                        {
                        m->iClipEnd = tmp;
                        }
                    //### SMTP values
                    }
                else if ( aName.CompareF( KTypeAttr ) == 0 )
                    {
                    m->SetTypeL(aValue);
                    }
                }

            break;
            }
        case ELayout:
            {
            if ( !iCurrentRegion )
                {
                return;
                }

            if ( aName.CompareF( KWidthAttr ) == 0 )
                {
                iCurrentRegion->iWidth = TSmilLength( aValue );
                }
            else if ( aName.CompareF( KHeightAttr ) == 0 )
                {
                iCurrentRegion->iHeight = TSmilLength( aValue );
                }
            else if ( aName.CompareF( KTopAttr ) == 0 )
                {
                iCurrentRegion->iTop = TSmilLength( aValue );
                }
            else if ( aName.CompareF( KBottomAttr ) == 0 )
                {
                iCurrentRegion->iBottom = TSmilLength( aValue );
                }
            else if ( aName.CompareF( KLeftAttr ) == 0 )
                {
                iCurrentRegion->iLeft = TSmilLength( aValue );
                }
            else if ( aName.CompareF( KRightAttr ) == 0 )
                {
                iCurrentRegion->iRight = TSmilLength( aValue );
                }
            else if ( aName.CompareF( KIdAttr ) == 0 )
                {
                iCurrentRegion->SetId( aValue );
                }
            else if ( aName.CompareF( KRegionNameAttr ) == 0 )
                {
                iCurrentRegion->SetName( aValue );
                }
            else if ( aName.CompareF( KZIndexAttr ) == 0 )
                {
                iCurrentRegion->iZIndex = StringToIntValue( aValue, 0 );
                }
            else if ( aName.CompareF( KShowBackgroundAttr ) == 0 )
                {
                if ( aValue.CompareF( KAlwaysVal ) == 0 )
                    {
                    iCurrentRegion->iBackgroundMode = MSmilRegion::EAlways;
                    }
                else if ( aValue.CompareF( KWhenActiveVal ) == 0 )
                    {
                    iCurrentRegion->iBackgroundMode = MSmilRegion::EWhenActive;
                    }
                }
            else if ( aName.CompareF( KBackgroundColorAttr ) == 0 || 
                      aName.CompareF( KBackgroundColor2Attr ) == 0 )
                {
                TBool transparent;
                iCurrentRegion->iBackground = ParseColor( aValue, transparent );
                iCurrentRegion->iTransparent = transparent;
                }        
            else if ( aName.CompareF( KFitAttr ) == 0 )
                {
                if ( aValue.CompareF( KMeetVal ) == 0 )
                    {
                    iCurrentRegion->iFit = MSmilRegion::EMeet;
                    }
                else if ( aValue.CompareF( KSliceVal ) == 0 )
                    {
                    iCurrentRegion->iFit = MSmilRegion::ESlice;
                    }
                else if ( aValue.CompareF( KFillVal ) == 0 )
                    {
                    iCurrentRegion->iFit = MSmilRegion::EFill;
                    }
                else if ( aValue.CompareF( KHiddenVal ) == 0 )
                    {
                    iCurrentRegion->iFit = MSmilRegion::EHidden;
                    }
                else if ( aValue.CompareF( KScrollVal ) == 0 )
                    {
                    iCurrentRegion->iFit = MSmilRegion::EScroll;
                    }
                }
            break;
            }
        case EHead:
        case ELayoutFinished:
            {
            if ( !iCurrentTransition )
                {
                return;
                }

            if ( aName.CompareF( KIdAttr ) == 0 )
                {
                iCurrentTransition->SetId( aValue );
                }
            else if ( aName.CompareF( KTypeAttr ) == 0 )
                {
                iCurrentTransition->SetType( aValue );
                }
            else if ( aName.CompareF( KSubtypeAttr ) == 0 )
                {
                iCurrentTransition->SetSubtype( aValue );
                }
            else if ( aName.CompareF( KDurAttr ) == 0 )
                {
                iCurrentTransition->iDur = TSmilTime( aValue );
                }
            else if ( aName.CompareF( KFadeColorAttr ) == 0 )
                {
                TBool b;
                iCurrentTransition->iFadeColor = ParseColor( aValue, b );
                }
            else if ( aName.CompareF( KDirectionAttr ) == 0 )
                {
                iCurrentTransition->iReverse = ( aValue.CompareF( KReverseVal ) == 0 );
                }
            else if ( aName.CompareF( KStartProgressAttr ) == 0 )
                {
                iCurrentTransition->iStartProgress = (TInt8)( 100 * StringToRealValue( aValue, 0 ) );
                }
            else if ( aName.CompareF( KEndProgressAttr ) == 0 )
                {
                iCurrentTransition->iEndProgress = (TInt8)( 100 * StringToRealValue( aValue, 1 ) );
                }
            break;
            }
        case ESmil:
            {
            if ( aName.Left(6).CompareF( KXmlnsAttr2 ) == 0 && 
                 aName.Length() > 6 && 
                 aValue.Length() > 0 )
                {
                Namespace ns;
                ns.name = aName.Mid(6).AllocL();
                ns.uri = aValue.AllocL();
                iNamespaces->AddL(ns);
                }
            break;
            }
        default: 
            {
            break;
            }
        }
    
    return;
    }

// ----------------------------------------------------------------------------
// CSmilParser::AttributesEndL
// ----------------------------------------------------------------------------
//
void CSmilParser::AttributesEndL( TInt /*aCount*/ )
    {
#ifdef PARSER_DEBUG
    RDebug::Print(_L("AttributesEnd"));
#endif

    if ( !iSwitchStack->IsEmpty() && 
         iSwitchStack->Peek().depth == iDepth - 1 && 
         iIgnoreDepth == 0 )
        {
        iSwitchStack->Peek().done = ETrue;
        }

    //unknown element, start ignoring if skip-content="true" (default)
    if ( iUnknownElement )
        {
        if ( iSkipContent && !iIgnoreDepth )
            {
            iIgnoreDepth = 1;
            }
        iUnknownElement = EFalse;
        }
    // timing element
    else if ( iState == EBody && iCurrent )
        {
        if ( iIgnoreDepth == 0 && iTimegraphSize < MAX_TIMEGRAPH_SIZE )
            {
            if ( iTimeContainer )
                {
                iTimeContainer->AddChild( iCurrent );
                }
                
            if ( iCurrent->IsTimeContainer() )
                {
                iTimeContainer = static_cast<CSmilTimeContainer*>( iCurrent );
                }
                
            iCurrent->AttachedL();
            iCurrent = 0;
            iTimegraphSize++;
            }    
        else
            {
            delete iCurrent;
            iCurrent = 0;
            }
        }
    // other
    else 
        {
        if ( iTag == KParamTag && iTimeContainer && iTimeContainer->IsMedia() )
            {
            if ( iParamName.Length() > 0 && iParamValue.Length() > 0 )
                {                
                static_cast<CSmilMedia*>(iTimeContainer)->AddParamL( iParamName, iParamValue );
                }
            }
        else if ( iCurrentRegion && iCurrentRegion != iPresentation->GetLayout() )
            {
            if ( iIgnoreDepth == 0 )
                {    
                iPresentation->GetLayout()->AddChild( iCurrentRegion );                
        
                iCurrentRegion = 0;
                }
            else
                {
                delete iCurrentRegion;
                iCurrentRegion = 0;
                }
            }
        else
            {
            iCurrentRegion = 0;
            }
        }

    iArea = 0;    

    iTag.Set( TPtrC() );
    }

// ----------------------------------------------------------------------------
// CSmilParser::CheckSystemAttribute
// ----------------------------------------------------------------------------
//
TBool CSmilParser::CheckSystemAttribute(const TDesC& aName, const TDesC& aValue)
    {
    if ( !( aName.Left( 6 ) == KSystemAttr ) )
        {
        return EFalse;
        }

    if ( iIgnoreDepth > 0 )
        {
        return ETrue;
        }

    if ( aName == KSystemRequiredAttr )
        {
        TInt b = 0;
        TInt e = 0;
        while ( e < aValue.Length() )
            {
            while ( e < aValue.Length() && aValue[e] != '+' )
                {
                e++;
                }
                
            TPtrC item = aValue.Mid( b, e - b );
            TBool found = EFalse;
            CLinkedList<Namespace>::Iter nsi(*iNamespaces);
            
            while( nsi.HasMore() )
                {
                Namespace ns = nsi.Next();
                if ( *(ns.name) == item )
                    {
                    found = EFalse;
                    for ( TInt n = 0; KSystemReqValue[n]; n++ )
                        {
                        if ( *(ns.uri) == TPtrC( KSystemReqValue[n] ) )
                            {
                            found = ETrue;
                            break;
                            }
                        }        
                        
                    if ( !found )
                        {
                        iIgnoreDepth = 1;
                        }
                    break;
                    }
                }
                
            if ( !found ) 
                {
                break;
                }
                
            e++;
            b = e;
            }
        }
    else if ( aName == KSystemScreenSizeAttr )
        {
        TInt n = 0;
        for ( ; n < aValue.Length(); n++ )
            {
            if ( aValue[n] == 'x' || aValue[n] == 'X' )
                {
                break;
                }
            }
        
        if ( n > 0 && n < aValue.Length() - 1 )
            {
            TInt w, h;
            TLex lex( aValue.Left( n ) );
            lex.Val( h );            
            lex = TLex( aValue.Mid( n + 1, aValue.Length() - n - 1 ) );
            lex.Val( w );

            TRect s = iPlayer->GetDimensions( iPresentation );
            if ( w > s.Width() || h > s.Height() )
                {
                iIgnoreDepth = 1;
                }
            }
        }
    else if ( aName == KSystemScreenDepthAttr )
        {
        TInt d;
        TLex lex( aValue );
        TBool r =lex.Val( d );
        
        // Hardcoded bitdepth. This should be a callback.
        // Querying system would be bad idea though, as it would
        // introduce new lib depedencies.
        if (r && d > SYSTEM_SCREEN_DEPTH )
            {
            iIgnoreDepth = 1;
            }
        }
    else if ( aName == KSystemCPUAttr )
        {
        if ( aValue != KCPUVal )
            {
            iIgnoreDepth = 1;
            }
        }

    else if ( aName == KSystemOperatingSystemAttr )
        {
        if ( aValue != KOSVal )
            {
            iIgnoreDepth = 1;
            }
        }
    else if ( aName == KSystemComponentAttr &&
              aValue.Left( 12 ) == KContentTypeVal )
        {
        MSmilMediaFactory* f = iPlayer->GetMediaFactory( iPresentation );
        if ( aValue.Length() >= 13 )
            {
            // strip off the scheme part from "ContentType:image/gif"
            // call with "image/gif" only
            if ( f->QueryContentType( aValue.Mid( 12 ) ) == EFalse )
                {
                iIgnoreDepth = 1;
                }
            }
        }
/*    else if (aName==KSystemContentTypeAttr)
        {
        MSmilMediaFactory* f = iPlayer->GetMediaFactory(iPresentation);
        if (f->QueryContentType(aValue)==EFalse)
            iIgnoreDepth = 1;
        }*/
    else
        {
        if ( iPlayer->EvaluateContentControlAttribute( aName, aValue ) == EFalse )
            {
            iIgnoreDepth = 1;
            }
        }

    return ETrue;
    }
    
// ----------------------------------------------------------------------------
// CSmilParser::ParseTimeListL
// ----------------------------------------------------------------------------
//
void CSmilParser::ParseTimeListL( const TDesC& aString, TBool aBegin )
    {
    TInt itemindex = 0;

    HBufC* list = HBufC::NewL( aString.Length() );
    CleanupStack::PushL( list );
    TPtrC listitem;
    TPtr buf( list->Des() );
    buf.Copy( aString );
    TBool last = EFalse;
    while( buf.Length() > 0 && !last )
        {
        TSmilCondition condition;

        buf.TrimLeft();

        itemindex = buf.Locate(';');
        if( itemindex > 0 )
            listitem.Set( buf.Left( itemindex ) );
        else
            {
            listitem.Set( buf.Right( buf.Length() - itemindex ) );
            last = ETrue;
            }

#ifdef PARSER_DEBUG
        RDebug::Print(_L("timelist: %S"),&listitem);
#endif
        
        condition.iOffset =  TSmilTime( listitem ) ;
            
        if (condition.iOffset.IsUnresolved())
            {
            condition.iOffset = 0;
            // ### FIXME!
            TInt colindex = listitem.Locate( '.' );

            TInt offindex = listitem.Locate( ' ' );
            if (offindex==KErrNotFound)
                offindex = listitem.Locate( '+' );
            if (offindex==KErrNotFound)
                offindex = listitem.Locate( '-' );

            if (colindex!=KErrNotFound)
                {
                condition.SetEventSourceL(listitem.Left(colindex));
                if (offindex==KErrNotFound)
                    condition.SetEventClassL(listitem.Mid(colindex+1));
                else
                    condition.SetEventClassL(listitem.Mid(colindex+1, offindex-colindex-1));

                }
            else
                {    
                if (offindex==KErrNotFound)
                    condition.SetEventClassL(listitem);
                else
                    condition.SetEventClassL(listitem.Left(offindex));                
                }

            if (offindex!=KErrNotFound)
                {
                condition.iOffset = TSmilTime(listitem.Mid(offindex));
                if (!condition.iOffset.IsFinite())
                    condition.iOffset = 0;
                }
            
            
            }

            iCurrent->AddConditionL( condition, aBegin );

        buf.Delete( 0, itemindex + 1 );
        }
    CleanupStack::PopAndDestroy();  //list
    }


// ---------------------------------------------------------
// CMediaObject::StringToRealValue
// Convert a descriptor to TReal32
// ---------------------------------------------------------
//
TReal32 CSmilParser::StringToRealValue( const TDesC& aString, TReal32 aDefault )
    {
    TReal32 value = aDefault;
    if( aString.Length() != 0 )
        {
        TLex lex( aString );
        if (lex.Val( value ) != KErrNone)
            {
            value = aDefault;
            }
        }
    return value;
    }

// ---------------------------------------------------------
// CMediaObject::SetBeginString
// Convert a descriptor to TInt
// ---------------------------------------------------------
//
TInt CSmilParser::StringToIntValue( const TDesC& aString, TInt aDefault )
    {
    TInt value = aDefault;
    if( aString.Length() != 0 )
        {
        TLex lex( aString );
        if (lex.Val( value ) != KErrNone)
            {
            value = aDefault;
            }
        }
    return value;
    }


// ----------------------------------------------------------------------------
// CSmilParser::ParseTimeListL
// Parser for css2 color values. Should be somewhere else
// ----------------------------------------------------------------------------
//
EXPORT_C TRgb CSmilParser::ParseColor( const TDesC& aStr, TBool& aTransparent )
    {
    aTransparent = ETrue;        
    TRgb res;
    if (aStr.Length()>256)
        return res;
    TBuf<256> aString = aStr;
    aString.LowerCase();
    if (aString.Length()>1 )
        {
        _LIT(KRgbO,"rgb(");
        TUint32 val;
        if (aString[0]=='#')
            {
            TLex lexer (aString.Mid(1));
            if (aString.Length()==4)
                {
                // #a3b notation (==#aa33bb)
                if (lexer.Val( val, EHex, TUint(0x00000fff) )==KErrNone )
                    {
#ifdef PARSER_DEBUG
                    RDebug::Print(_L("color=%x"), val);
#endif
                    res = TRgb (((val&0xf00)>>8)|((val&0xf00)>>4), 
                        ((val&0x0f0)>>4)|(val&0x0f0), 
                        (val&0xf)|((val&0xf)<<4));
                    aTransparent = EFalse;
                    }
                }
            else
                {
                // #aa33bb notation
                if (lexer.Val( val, EHex, TUint(0x00ffffff) )==KErrNone )
                    {
#ifdef PARSER_DEBUG
                    RDebug::Print(_L("color=%x"), val);
#endif

                    res = TRgb ((val&0x00ff0000)>>16, (val&0x0000ff00)>>8, (val&0x000000ff));
                    aTransparent = EFalse;
                    }
                }            
            }
        else if (aString.Left(4)==KRgbO)
            {
            // rgb(123,33,167) notation and rgb(40%,10%,70%) notation            
            TInt rgb[3];
            TInt count=0;
            TInt b = 4;
            TInt e = 4;
            TBool percent = EFalse;
            while (e<aString.Length())
                {
                while (e<aString.Length() && aString[e]==' ')
                    e++;
                b=e;
                while (e<aString.Length() && aString[e]!=',' && aString[e]!=')')
                    e++;
                TInt te = e-1;
                while (te>b && aString[te]==' ')
                    te--;
                if (te>=b)
                    {
                    if (aString[te]=='%')
                        {
                        percent = ETrue;
                        te--;
                        }
                    TPtrC num = aString.Mid(b,te-b+1);
#ifdef PARSER_DEBUG
                    RDebug::Print(_L("rgb num=%S"),&num);
#endif
                    rgb[count] = StringToIntValue(num,0);
                    TInt max = 255;
                    if (percent) max=100;
                    if (rgb[count]<0) rgb[count]=0;
                    if (rgb[count]>max) rgb[count]=max;
    
                    count++;
                    if (count==3)
                        break;
                    }
                e++;        
                }
            if (count==3)
                {
                if (percent)
                    res = TRgb(rgb[0]*255/100, rgb[1]*255/100, rgb[2]*255/100);
                else
                    res = TRgb(rgb[0], rgb[1], rgb[2]);
                aTransparent = EFalse;
                }
            }
        else if (aString==KTransparentVal)
            {
            aTransparent = ETrue;
            }
        else
            {
            for (int n=0; KColorNames[n]; n++)
                {
                if (aString == TPtrC(KColorNames[n]))
                    {                    
                    res = KColorValues[n];
#ifdef PARSER_DEBUG
                    RDebug::Print(_L("color=%x (%S)"), res.Value(), &aString);
#endif

                    aTransparent = EFalse;
                    break;
                    }

                }            
            }
        // ### FIXME add system colors!
        
        }

    return res;

    }

// ----------------------------------------------------------------------------
// CSmilParser::SetMaxDownUpScaling
// ----------------------------------------------------------------------------
//    
EXPORT_C void CSmilParser::SetMaxDownUpScaling( TReal32 aDown, TReal32 aUp )
    {
    iDown = aDown;
    iUp = aUp;
    }
