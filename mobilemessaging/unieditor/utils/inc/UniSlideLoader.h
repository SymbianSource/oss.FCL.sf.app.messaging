/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Unified Message Editor slide loader.
*
*/



#ifndef C_UNISLIDELOADER_H
#define C_UNISLIDELOADER_H

// INCLUDES

#include <e32base.h>
#include <MsgMedia.hrh>
#include <msgasynccontrolobserver.h>
#include <ConeResLoader.h>

#include <MsgEditorView.h>

// CONSTANTS

// MACROS

// DATA TYPES

enum TUniControlMode
    {
    EUniControlEditorMode = 0,
    EUniControlViewerMode
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMsgBaseControl;
//Hma class CMsgEditorView;
class CUniObject;
class CUniDataModel;
class CMsgMediaControl;
class MMsgAsyncControl;
class CMsgXhtmlBodyControl;
class MUniObjectObserver;

// CLASS DECLARATION

class MUniSlideLoaderObserver
    {
    public:
    
        /**
        * Called when slide loading has finished.
        *
        * @since S60 3.1
        *
        * @param aError KErrNone if no error happened,
        *               otherwise some systemwide error code.        
        */
        virtual void SlideLoadReady( TInt aError ) = 0;
    };


// ==========================================================

/**
*  Unified Editor slide loader class declaration.
*
*  @lib UniUtils.lib
*
*  @since S60 3.2
*/
NONSHARABLE_CLASS( CUniSlideLoader ): public CActive,
                                      public MMsgAsyncControlObserver
    {
    public:  // Constructor and destructor

        /**
        * Two-phased constructor.
        *
        * @since S60 3.2
        *
        * @param aControlObserver Observer for added control.
        * @param aModel Reference to UniDataModel
        * @param aView View to which controls are added.
        * @param aControlMode TUniControlMode enumeration
        *
        * @return new object
        */
        IMPORT_C static CUniSlideLoader* NewL(
            MMsgAsyncControlObserver& aControlObserver,
            CUniDataModel& aModel,
            CMsgEditorView& aView,
            TUniControlMode aControlMode );

        /**
        * Two-phased constructor.
        *
        * @since S60 5.0
        *
        * @param aUniObjectObserver Observer for CUniObjet events. When slide is loaded 
        * observer is added to CUniObject. When ResetViewL() is called, observer is removed 
        * from CUniObject. When control is reloaded, observer remains untouched. 
        * @param aControlObserver Observer for added control.
        * @param aModel Reference to UniDataModel
        * @param aView View to which controls are added.
        * @param aControlMode TUniControlMode enumeration
        *
        * @return new object
        */
        IMPORT_C static CUniSlideLoader* NewL(
            MUniObjectObserver*         aUniObjectObserver,
            MMsgAsyncControlObserver&   aControlObserver,
            CUniDataModel&              aModel,
            CMsgEditorView&             aView,
            TUniControlMode             aControlMode );


        /**
        * Destructor.
        *
        * @since S60 3.2
        */
        virtual ~CUniSlideLoader();

    public: 

        /**
        * Loads all the slide from given slide.
        *
        * @since S60 3.2
        *
        * @param aSlideObserver Slide loader observer.
        * @param aSlideNum Loaded slide number
        */
        IMPORT_C void LoadSlideL(
            MUniSlideLoaderObserver& aSlideObserver,
            TInt aSlideNum );

        /**
        * Loads given object.
        *
        * @since S60 3.2
        *
        * @param aSlideObserver Slide loader observer.
        * @param aObject Object to be loaded.
        */
        IMPORT_C void LoadObject(
            MUniSlideLoaderObserver& aSlideObserver,
            CUniObject* aObject );
            
        /**
        * Removes all the controls from the view.
        *
        * @since S60 3.2
        */
        IMPORT_C void ResetViewL();

        /**
        * Updates correct icon for media control.
        *
        * @since S60 3.2
        *
        * @param aMediaControl Control that icon is wanted to be updated.
        * @param aObject Object related to the control
        */
        IMPORT_C void UpdateControlIconL( CMsgMediaControl& aMediaControl, CUniObject* aObject );

        /**
        * Reloads one control without need to reload whole slide.
        *
        * @since S60 5.0
        *
        * @param aMediaControl Control which needs to be reloaded.
        * @param aObject Object related to the control
        */
        IMPORT_C void ReLoadControlL( CMsgMediaControl* aControl, CUniObject* aObject );

    public:
    
        /**
        * From CActive. See e32base.h
        *
        * @since S60 3.2
        *
        */
        void DoCancel();

        /**
        * From CActive. See e32base.h
        *
        * @since S60 3.2
        */
        void RunL();
    
        /**
        * From CActive. See e32base.h
        *
        * @since S60 3.2
        */
        TInt RunError( TInt aError );

        /**
        * From MMsgAsyncControlObserver. See MsgAsyncControlObserver.h
        *
        * @since S60 3.2
        */
        void MsgAsyncControlStateChanged( CMsgBaseControl& aControl,
                                          TMsgAsyncControlState aNewState,
                                          TMsgAsyncControlState aOldState );
        
        /**
        * From MMsgAsyncControlObserver. See MsgAsyncControlObserver.h
        *
        * @since S60 3.2       
        */
        void MsgAsyncControlResourceChanged( CMsgBaseControl& aControl, TInt aType );
                                                   
    private: // Constructors

        /**
        * Hidden C++ default constructor.
        */
        CUniSlideLoader();
        
        /**
        * C++ constructor
        */
        CUniSlideLoader(    MUniObjectObserver*         aUniObjectObserver,
                            MMsgAsyncControlObserver&   aControlObserver,
                            CUniDataModel&              aDataModel,
                            CMsgEditorView&             aView,                         
                            TUniControlMode             aControlMode );

        /**
        * Symbian constructor.
        */
        void ConstructL();
        
        /**
        * Completed current active scheduler loop.
        */
        void CompleteSelf();
        
        /**
        * Performs single load slide operation step.
        */
        void LoadSlideStepL();

        /**
        * Performs single load object operation step.
        */
        void LoadObjectStepL();

        /**
        * Loads object. Common function for all objects.
        */
        void LoadObjectL( CUniObject* aObject );

        /**
        * Loads text object.
        */
        void LoadTextL( CUniObject* aObject );

        /**
        * Loads image object.
        */
        void LoadImageL( CUniObject* aObject );

        /**
        * Loads audio object.
        */
        void LoadAudioL( CUniObject* aObject );

        /**
        * Loads video object.
        */
        void LoadVideoL( CUniObject* aObject );
        
        /**
        * Loads the svg
        */
        void LoadSvgL( CUniObject* aObject );
        
        /**
        * Loads content for media control.
        */
        TBool LoadControlL( MMsgAsyncControl& aMediaControl, CUniObject* aObject );
                
        /**
        * Adds control to view.
        */
        void AddToViewL( CMsgBaseControl* aControl,
                         TInt aControlId,
                         TMsgMediaType aType,
                         CUniObject* aObject );
        
        /**
        * Determines if error code is DRM error code or not.
        */
        TBool IsDRMError( TInt aError ) const;
        
        /**
        * Determines whether UniObject DRM rights are valid.
        */
        TBool DrmRightsValid( CUniObject* aObject ) const;
        
        /**
        * Consumes DRM righs from the UniObject.
        */
        TBool ConsumeDrmRights( CUniObject* aObject ) const;
        
        /**
        * Sets correct layout for media icon for specified control.
        */
        void SetIconLayoutL( CMsgMediaControl& aMediaControl );
        
        /**
        * Performs media control resource change event handling
        */
        void DoMsgAsyncControlResourceChangedL( CMsgBaseControl& aControl, TInt aType );
        
        /**
        * Resolves base control type and returns correct type pointer.
        */
        void ResolveCorrectControlType( CMsgBaseControl& aControl,
                                        CMsgMediaControl*& aMediaControl,
                                        CMsgXhtmlBodyControl*& aXhtmlControl );
        
        /**
        * Implements part of reloading control code
        *
        * @since S60 5.0
        *
        * @param aMediaControl Control which needs to be reloaded.
        * @param aObject Object related to the control
        */
        TBool DoReLoadControlL( MMsgAsyncControl& aMediaControl, CUniObject* aObject );

        /**
        * Removes observers of the slide
        *
        * @since S60 5.0
        */
        void  RemoveObservers( );
        
        /**
        * Resolve CUniObject related to media control.
        *
        * @since S60 5.0
        * @param aControl control for which CUniObjects is needed
        * @return CUniObject related to the given control
        */
        CUniObject* ObjectByBaseControl( CMsgBaseControl* aControl );

    private: //Data

        enum TMmsLoadOperation
            {
            ELoadSlide,
            ELoadObject
            };

        MMsgAsyncControlObserver&   iControlObserver;
        CUniDataModel&              iDataModel;
        CMsgEditorView&             iView;
        TUniControlMode             iControlMode;

        MUniSlideLoaderObserver*    iSlideObserver;

        TInt                        iLoadedSlide;
        TInt                        iObjectNum;
        TMmsLoadOperation           iOperation;
        CUniObject*                 iLoadedObject;
        
        TInt                        iError;
        
        RConeResourceLoader         iResourceLoader;

        MUniObjectObserver*         iUniObjectObserver;
    };

#endif // C_UNISLIDELOADER_H
