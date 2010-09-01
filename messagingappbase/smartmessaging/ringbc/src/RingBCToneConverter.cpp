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
* Description:  
*       For checking the validity of Smart messaging ringing tone and for 
*       ripping the ringing tone title from binary data. 
*
*/



// INCLUDE FILES

#include <e32svr.h>
#include "RingBCToneConverter.h"
#include "NsmRingTone.h"

#ifdef _DEBUG_
#define DEBUG(s) RDebug::Print( _L(s) )
#define DEBUG1(s,t) RDebug::Print( _L(s),t )
#else
#define DEBUG(s)
#define DEBUG1(s,t)
#endif



// LOCAL CONSTANTS AND MACROS
const TInt KCompMaxSongLength = 240; // maximum number of notes


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// NewL()
//
// ---------------------------------------------------------
CRingBCNSMConverter* CRingBCNSMConverter::NewL()
    {
	CRingBCNSMConverter* self = new (ELeave) CRingBCNSMConverter();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();	// self
	return self;
    }

// ---------------------------------------------------------
// CRingBCNSMConverter()
//
// ---------------------------------------------------------
CRingBCNSMConverter::CRingBCNSMConverter()
    {
	iScale = EScale2;
	iStyle = EStyleNatural;
    }

// ---------------------------------------------------------
// CRingBCNSMConverter()
//
// ---------------------------------------------------------
void CRingBCNSMConverter::ConstructL()
    {
    }

// ---------------------------------------------------------
// ~CRingBCNSMConverter()
//
// ---------------------------------------------------------
CRingBCNSMConverter::~CRingBCNSMConverter()
    {
	delete iCompData.iSongData; 	
    }


// ---------------------------------------------------------
// ConvertNsmToCompDataL()
//
// ---------------------------------------------------------
TCompData* CRingBCNSMConverter::ConvertNsmToCompDataL( const TDesC8& aFileData )
    {
#ifdef _DEBUG_
    DEBUG( "***BEGIN" );
#endif

	iReceivedNsmRTData.Set( aFileData );

    iReadOnly = EFalse;
	ResetForCompData();
	delete iCompData.iSongData;
	iCompData.iSongData = NULL;
	iCompData.iSongData = new(ELeave) CArrayFixFlat<TInstruction>( KCompMaxSongLength );

#ifdef _DEBUG_
    DEBUG1( "***NSM DATA LENGTH: %d", iReceivedNsmRTData.Length() );
#endif
    if( iReceivedNsmRTData.Length() == 0 )
        {
        DEBUG( "!!!ZERO LENGTH" );
        User::Leave( KErrCorrupt );
        }

	TInt commandlength( 1 );
	while( commandlength != KrtCommandEnd && 
        iNsmPosition < iReceivedNsmRTData.Length() )
        {
		commandlength = GetBits(KrtCommandLengthBits);
#ifdef _DEBUG_
        DEBUG1( "***NEXT CMD LENGTH: %d", commandlength );
#endif
		if (commandlength > 3)
            {
            DEBUG( "!!!CMD TOO LONG" );
            User::Leave( KErrCorrupt );
            }

		for (TInt i=0; i<commandlength; i++)
            {		
#ifdef _DEBUG_
            DEBUG1( "***CMD NUMBER %d", i );
#endif
			ProcessNsmRingToneCommandPartL();
            }
        }

	return &iCompData;
    }

// ---------------------------------------------------------
// ProcessNsmRingToneCommandPartL()
//
// ---------------------------------------------------------
void CRingBCNSMConverter::ProcessNsmRingToneCommandPartL()
    {
	TInt commandpart( GetBits(KrtCommandPartBits) );
#ifdef _DEBUG_
    DEBUG1( "***CMD PART: %d", commandpart );
#endif
	switch (commandpart)
	    {
	    case KrtCommandRTProgramming:
			{
			iNsmRTFlags |= KrtFlagRTProgramming;
		    break;
			}
	    case KrtCommandUnicode:
			{
			iNsmRTFlags |= KrtFlagUnicode;
		    break;
			}
	    case KrtCommandCancelCommand:
			{
		    if (GetBits(KrtCommandPartBits) == KrtCommandUnicode)
                {
			    iNsmRTFlags &= ~KrtFlagUnicode;
                }
		    break;
			}
	    case KrtCommandSound:
			{
			if ((iNsmRTFlags & KrtFlagRTProgramming) == 0)
                {
                DEBUG( "!!!CMD SOUND WITH NO RT PROGRAMMING" );
                User::Leave( KErrCorrupt );
                }
		    ProcessNsmRingToneSoundSpecL();
		    break;
			}
	    default:
			{
            // Ignore unknown commands
            DEBUG( "???CMD UNKNOWN" );
            iReadOnly = ETrue;
		    break;
			}
	    }

	GetBits(KrtFiller); // skip the filler bits
    }

// ---------------------------------------------------------
// ProcessNsmRingToneSoundSpecL()
//
// ---------------------------------------------------------
void CRingBCNSMConverter::ProcessNsmRingToneSoundSpecL()
    {
	TInt soundspec( GetBits(KrtSongTypeBits) );
#ifdef _DEBUG_
    DEBUG1( "***SONG TYPE: %d", soundspec );
#endif
	switch (soundspec)
	    {
	    case KrtSongTypeBasic:
			{
		    ProcessNsmRingToneSongTitleL();
		    ProcessNsmRingToneSongL();
		    break;
			}
	    case KrtSongTypeTemporary:
            {
		    ProcessNsmRingToneSongL();
		    break;
			}
	    default:
            {
		    iReadOnly = ETrue;
		    break;
			}
	    }

	iCompData.iSongLength = iCompDataPosition;
    }

// ---------------------------------------------------------
// ProcessNsmRingToneSongTitleL()
//
// ---------------------------------------------------------
void CRingBCNSMConverter::ProcessNsmRingToneSongTitleL()
{
	TInt titlelength( GetBits(KrtSongTitleTextLengthBits) );
    DEBUG1( "***TITLE LENGTH %d", titlelength );

    TInt charWidth( KrtDefaultCharBits );
	if( iNsmRTFlags & KrtFlagUnicode )
    	{
        charWidth = KrtUnicodeCharBits;
        }
    DEBUG1( "***TITLE CHAR WIDTH: %d", charWidth );

    TBuf<KrtSongTitleMaxLength> title;
	for( TInt i( 0 ); i < titlelength; i++ )
        {
        title.Append( GetBits(charWidth) );
        }
    DEBUG1( "***TITLE: %S", &title );

    SetTitle( title );
    }

// ---------------------------------------------------------
// ProcessNsmRingToneSong()
//
// ---------------------------------------------------------
void CRingBCNSMConverter::ProcessNsmRingToneSongL()
    {
	TInt songsequencelength( GetBits(KrtSongsequenceLengthBits) );
    DEBUG1( "***SEQUENCE LENGTH %d", songsequencelength );

	/*if( songsequencelength == 0 )
		{
		iReadOnly = ETrue;
		}*/

	for (TInt i( 0 ); i<songsequencelength; i++)
	    {
		TInt patternheaderid( GetBits(KrtInstructionIdLengthBit) );
        DEBUG1( "***PATTERN HEADER ID: %d", patternheaderid );
		if (patternheaderid != EPatternHeaderId)
            {
            DEBUG( "!!!PATTERN HEADER ID INVALID" );
			User::Leave(KErrCorrupt);
            }

		TInt patternid( GetBits(KrtPatternIdLengthBit) );
        DEBUG1( "***PATTERN ID: %d", patternid );
		TInt loopvalue( GetBits(KrtPatternLoopValueBits) );
        DEBUG1( "***PATTERN LOOP VALUE: %d", loopvalue );
		TInt patternspec( GetBits(KrtPatternSpecBits) );
        DEBUG1( "***PATTERN SPEC: %d", patternspec );
		
		if( patternspec != KrtPatternDefined )
		    {
            DEBUG( "***PATTERN NOT DEFINED" );
			iNsmRTPatterns[patternid].iStart = iCompData.iSongData->Count();
			
            for (TInt l( 0 ); l < patternspec && iCompDataPosition < KCompMaxSongLength; l++)
                {
				ProcessNsmRingTonePatternInstructionL();
                }
			iNsmRTPatterns[patternid].iEnd = iCompDataPosition - 1;
		    }
		// check if the pattern is really already defined before copying
		TInt start( iNsmRTPatterns[patternid].iStart );
		TInt end( iNsmRTPatterns[patternid].iEnd );
		if( start < end )
		    {
			if( loopvalue < KrtPatternRepeatInfinite )
			    {
				for( TInt loopNum( 0 ); loopNum < loopvalue; loopNum++ ) // handle repeat
				    {
					for (TInt k=start; (k<=end)&&(iCompDataPosition<KCompMaxSongLength); 
						k++, iCompDataPosition++)
                        {
						iCompData.iSongData->AppendL( iCompData.iSongData->At( k ) );
                        }
				    }
			    }
			else // handle infinite repeat
			    {
				while( iCompDataPosition<KCompMaxSongLength )
				    {
					for (TInt k=start; (k<=end)&&(iCompDataPosition<KCompMaxSongLength); 
						k++, iCompDataPosition++)
                        {
						iCompData.iSongData->AppendL( iCompData.iSongData->At( k ) );
                        }
				    }
			}
		}
	}
}

// ---------------------------------------------------------
// ProcessNsmRingTonePatternInstruction()
//
// ---------------------------------------------------------
void CRingBCNSMConverter::ProcessNsmRingTonePatternInstructionL()
    {
	TInt instructionid( GetBits(KrtInstructionIdLengthBit) );
    DEBUG1( "***INSTRUCTION ID: %d", instructionid );
	switch (instructionid)
	    {
	    case ENoteInstructionId:
			{
            DEBUG1( "***INSTRUCTION ID: NOTE", instructionid );
		    ProcessNsmRingToneNoteInstructionL(iScale, iStyle);
		    break;
			}
	    case EScaleInstructionId:
			{
			iScale = GetBits(KrtNoteScaleBits);
            DEBUG1( "***INSTRUCTION SCALE: %d", iScale );
		    break;
			}
	    case EStyleInstructionId:
			{
			iStyle = GetBits(KrtNoteStyleBits);
            DEBUG1( "***INSTRUCTION STYLE: %d", iStyle );
            if( iStyle == EStyleReserved )
                {
                DEBUG( "???INSTRUCTION STYLE NOT SUPPORTED" );
                iReadOnly = ETrue;
                }
		    break;
			}
	    case ETempoInstructionId:
			{
			iCompData.iTempo = GetBits(KrtTempoBits);
            DEBUG1( "***INSTRUCTION TEMPO: %d", iCompData.iTempo );
            if( iCompData.iTempo > ETempo250 )
                {
                DEBUG( "???TEMPO TOO HIGH" );
                iReadOnly = ETrue;
                }
		    break;
			}
	    case EVolumeInstructionId:
            {
			DEBUG( "***INSTRUCTION VOLUME" );
		    GetBits(KrtVolumebits);
		    break;
			}
	    default:
            {
			DEBUG( "???INSTRUCTION NOT SUPPORTED" );
            iReadOnly = ETrue;
		    break;
			}
	    }
    }

// ---------------------------------------------------------
// ProcessNsmRingToneNoteInstruction()
//
// ---------------------------------------------------------
void CRingBCNSMConverter::ProcessNsmRingToneNoteInstructionL(TInt aScale, TInt aStyle)
    {
	TInt notevalue( GetBits(KrtNoteValueBits) );
    DEBUG1( "***NOTE VALUE: %d", notevalue );
    if( notevalue != ENotePause && (notevalue > ENoteB 
        || iScale == EScale4 && notevalue > ENoteGis
        || iScale == EScale1 && notevalue < ENoteG ) )
        {
        DEBUG( "???NOTE VALUE OUT OF RANGE" );
        iReadOnly = ETrue;
        }

	TInt noteduration( GetBits(KrtNoteDurationBits) );
    DEBUG1( "***NOTE DURATION: %d", noteduration );
    if( noteduration > EDurationThirtysecond )
        {
        DEBUG( "???NOTE DURATION OUT OF RANGE" );
        iReadOnly = ETrue;
        }

	TInt notedurspecifier( GetBits(KrtNoteDurSpecifierBits) );
    DEBUG1( "***NOTE DURATION SPECIFIER: %d", notedurspecifier);
    if( notedurspecifier == EDurSpecifireDoubleDotted
        || notedurspecifier > EDurSpecifierTriplet )
        {
        DEBUG( "???NOTE DURATION SPECIFIER OUT OF RANGE" );
        iReadOnly = ETrue;
        }

	TInstruction symbol;
	symbol.iValue = notevalue;
	symbol.iDuration = noteduration;
	symbol.iDurspecifier = notedurspecifier;
	symbol.iScale = aScale;
	symbol.iStyle = aStyle;

	iCompData.iSongData->AppendL( symbol );
	iCompDataPosition ++;
    }

// ---------------------------------------------------------
// GetBits()
//
// ---------------------------------------------------------
TInt CRingBCNSMConverter::GetBits(TInt aNumBits)
    {
	TUint32 buf( 0 );   
	if( aNumBits == 0 ) // handle byte alignment
	    {
		if (iNsmPositionBit != 0)
		    {
			iNsmPositionBit = 0;	// skip filler bits
			iNsmPosition ++;			
		    }
		return buf;
	    }

	for (TInt n=0; n<4; n++)
	    {
		buf <<= 8;
		if ( iNsmPosition+n < iReceivedNsmRTData.Length() )
            {
			buf |= iReceivedNsmRTData[iNsmPosition+n];
            }
	    }

	TUint32 filter = 0;
	buf = (buf >> (32 - iNsmPositionBit - aNumBits)) & ~(~filter << aNumBits);

	iNsmPositionBit += aNumBits;
	while (iNsmPositionBit > 7)
	    {
		iNsmPositionBit -= 8;
		iNsmPosition ++;
	    }
	return buf;
    }

// ---------------------------------------------------------
// ResetForCompData()
//
// ---------------------------------------------------------
void CRingBCNSMConverter::ResetForCompData()
    {
	iNsmRTPatterns[EPatternIdA].iStart = 0;
	iNsmRTPatterns[EPatternIdA].iEnd = 0;
	iNsmRTPatterns[EPatternIdB].iStart = 0;
	iNsmRTPatterns[EPatternIdB].iEnd = 0;
	iNsmRTPatterns[EPatternIdC].iStart = 0;
	iNsmRTPatterns[EPatternIdC].iEnd = 0;
	iNsmRTPatterns[EPatternIdD].iStart = 0;
	iNsmRTPatterns[EPatternIdD].iEnd = 0;
	iNsmRTFlags = 0;
    iNsmPosition = 0;
	iNsmPositionBit = 0;
	iCompDataPosition = 0;

	iCompData.iTempo = KrtBpmDefault;
	iCompData.iSongLength = 0;
    }

// ---------------------------------------------------------
// SetTitle()
//
// ---------------------------------------------------------
void CRingBCNSMConverter::SetTitle( const TDesC& aFileName )
    {
    iCompData.iSongTitle = aFileName.Left( KrtSongTitleTextLength );
    }


// ---------------------------------------------------------
// TitleLC()
//
// ---------------------------------------------------------
HBufC* CRingBCNSMConverter::TitleLC(TPtr8& aFileData)
{
	iReceivedCompData = ConvertNsmToCompDataL(aFileData);

	HBufC* titlePtr = iReceivedCompData->iSongTitle.AllocLC();
	return titlePtr;
}


// ---------------------------------------------------------
// IsRingToneMimeTypeL()
//
// ---------------------------------------------------------
TBool  CRingBCNSMConverter::IsRingToneMimeTypeL(TPtr8& aFileData)
    {
	TRAPD( returnError, 
        iReceivedCompData = ConvertNsmToCompDataL(aFileData) );
    if( returnError != KErrCorrupt 
        && returnError != KErrNotSupported )
        {
        User::LeaveIfError( returnError );
        }

    return returnError != KErrCorrupt;
    }
// end of file
