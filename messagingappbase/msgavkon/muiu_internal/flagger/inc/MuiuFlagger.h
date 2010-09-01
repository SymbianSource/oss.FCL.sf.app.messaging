/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains declearation of CMuiuFlags class
*
*/




#ifndef CMUIUFLAGGER_H
#define CMUIUFLAGGER_H

//  INCLUDES
#include "MuiuFlags.h"
#include <e32cmn.h>

// CONSTANTS
// MACROS
// DATA TYPES
struct TMuiuLocalFeatureItem
    {
    // The Central Repository ID to be used to access the feature
    TUid iUid;
    // The Key id that is used to get the value
    TUint32 iKeyId;    
    // Value to be used to set the feature
    TUint32 iFlag;
    // iFlag is a flag (ETrue) or an index (EFalse)
    TBool iIsFlag;
    };

typedef RArray<TUint> TMuiuGlobalFeatureArray;
typedef RArray<TMuiuLocalFeatureItem> TMuiuLocalFeatureArray;
typedef RArray<TBool> TMuiuFlagArray;

// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  ?one_line_short_description.
*  ?other_description_lines
*
*  @lib ?library
*  @since Series60 3.0
*/
class CMuiuFlags : public CBase
    {
    public:  // Constructors and destructor
    
        /**
        *
        * @since Series60 3.0
        */
        IMPORT_C static CMuiuFlags* NewL( 
            const TMuiuGlobalFeatureArray* aGlobalFeatures = NULL,
            const TMuiuLocalFeatureArray* aLocalFeatures = NULL,
            const TMuiuFlagArray* aFlags = NULL );
            
        /**
        *
        * @since Series60 3.0
        */
        IMPORT_C static CMuiuFlags* NewLC( 
            const TMuiuGlobalFeatureArray* aGlobalFeatures = NULL,
            const TMuiuLocalFeatureArray* aLocalFeatures = NULL,
            const TMuiuFlagArray* aFlags = NULL );            
 
            
        IMPORT_C virtual ~CMuiuFlags();

    public: // New functions
    
        inline TBool GF( 
            const TUint aGlobalFlag ) const;
        inline TBool LF( 
            const TUint aLocalFeature ) const;
                    
        inline void SetFlag( 
            const TUint aFlag );
        inline void ClearFlag( 
            const TUint aFlag );
        inline TBool Flag( 
            const TUint aFlag ) const;
        inline void ChangeFlag( 
            const TUint aFlag, 
            const TBool aNewState );

    public: // Functions from base classes
        
    protected:  // New virtual functions
    protected:  // New functions
    protected:  // Functions from base classes   
        
    private:  // Constructors and destructor
         
        CMuiuFlags();
        void ConstructL( 
            const TMuiuGlobalFeatureArray* aGlobalFeatures,
            const TMuiuLocalFeatureArray* aLocalFeatures,
            const TMuiuFlagArray* aFlags );
            
    private:  // New virtual functions
    private:  // New functions
            
        // FLAG HANDLERS
        
        /**
        *
        * @since Series60 3.0
        */
        void PrepareGlobalFeaturesL(
            const TMuiuGlobalFeatureArray& aGlobalFeatures );
            
        /**
        *
        * @since Series60 3.0
        */
        void PrepareLocalFeaturesL(
            const TMuiuLocalFeatureArray& aLocalFeatures );   
            
        /**
        *
        * @since Series60 3.0
        */
        void PrepareGeneralFlags(
            const TMuiuFlagArray& aFlags );           
        
        inline void ChangeGF( 
            const TUint aGlobalFlag,
            const TBool aNewState );        
        
        inline void ChangeLF( 
            const TUint aLocalFlag,
            const TBool aNewState );                
            
        inline void HandleSetFlag( 
            TMuiuFlags& aFlags, 
            const TUint aFlag ) const;
        inline void HandleClearFlag(
            TMuiuFlags& aFlags, 
            const TUint aFlag ) const;
        inline TBool HandleFlag( 
            const TMuiuFlags& aFlags, 
            const TUint aFlag ) const; 
        inline void HandleChangeFlag( 
            TMuiuFlags& aFlags, 
            const TUint aFlag, 
            const TBool aNewState ) const;
        
        private:  // Functions from base classes 

    public:     // Data  
    protected:  // Data
    private:    // Data    
    
        // Flag storage for global features   
        TMuiuFlags  iGlobalFeatures;
        // Flag storage for local features
        TMuiuFlags  iLocalFeatures;
        // Flag storage for basic flags
        TMuiuFlags  iFlags;        
    };

#include <muiuflagger.inl>

#endif      // CMUIUFLAGGER_H
            
// End of File
