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
* Description:     CTestVideoConversion declaration*
*/


#ifndef TESTVIDEOCONVERSION_H
#define TESTVIDEOCONVERSION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATION
class MVideoConversionCallback;
class CVideoConversionPlugin;


// CLASS DECLARATION

/**
 *  CTestVideoConversion
 */
class CTestVideoConversion : public CBase,
                             public MVideoConversionCallback
    {
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    virtual ~CTestVideoConversion();

    /**
     * Two-phased constructor.
     */
    static CTestVideoConversion* NewL( CStifLogger* aLog );

    /**
     * Two-phased constructor.
     */
    static CTestVideoConversion* NewLC( CStifLogger* aLog );
    
protected:
    /**
     * MVideoConversionCallback calls this when conversion is done
     */
    void VideoConversionDone( TInt aStatus );

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CTestVideoConversion( CStifLogger* aLog );

    /**
     * default constructor for performing 2nd stage construction
     */
    void ConstructL();
    
private: //Data
    //Pointer to any one of videoconversionplugin implemetations
    CVideoConversionPlugin* iConverter;
    //Logger for logging provided by STIF
    CStifLogger*            iLog;

    };

#endif // TESTVIDEOCONVERSION_H
