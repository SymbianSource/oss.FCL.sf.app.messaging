/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*       Utility methods for UI and engine modules. Provides help for address
*       string parsing, resource files and contact database access.
*       General address format is either
*           alias<real-address>
*       or
*           <real-address>
*       as used in the Client MTM API.
*
*/

 

// INCLUDE FILES
#include <barsc.h>               // resource file
#include <bautils.h>
#include <miutpars.h>            // e-mail utilities

#include <miutconv.h>            // CharConv 
#include <flogger.h>
#include <e32svr.h>
#include <e32base.h>
#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <imcvcodc.h>
#else
#include <imcvcodc.h>
#include <cimconvertheader.h>
#endif
#include <f32file.h>                 
#include <UiklafInternalCRKeys.h>
#include <telconfigcrkeys.h>
#include <centralrepository.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <data_caging_path_literals.hrh>

#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactLink.h>
#include <CPbk2StoreConfiguration.h>   // Contact store configuration
#include <contactmatcher.h> // contact match wrapper
#include <CVPbkPhoneNumberMatchStrategy.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkFieldTypeRefsList.h>
#include <TVPbkFieldVersitProperty.h>
#include <VPbkFieldType.hrh>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactStoreUriArray.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include "mmsgenutils.h"
#include "MmsEnginePrivateCRKeys.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KLogBufferLength = 256;
#ifdef _DEBUG
_LIT( KLogDir, "mmsc" );
_LIT( KLogFile, "mmsc.txt" );
#endif
const TUint KMinAliasMaxLength = 14;
const TUint KExtraSpaceForConversion10 = 10;
const TUint KExtraSpaceForConversion30 = 30;

const TInt KErrMultipleMatchFound = KErrGeneral;
// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// Constructor 
//
EXPORT_C TMmsGenUtils::TMmsGenUtils()
    {
    }

// Destructor 
//
// THERE SHOULD NOT BE DESTRUCTOR IN T-CLASS.
// MUST BE REMOVED
EXPORT_C TMmsGenUtils::~TMmsGenUtils()
    {
    }


// ---------------------------------------------------------
// TMmsGenUtils::AddressTypeAndRealAddress
// ---------------------------------------------------------
//
EXPORT_C TInt TMmsGenUtils::AddressTypeAndRealAddress(
    const TDesC& aAddress,
    TMmsAddressType& aType,
    TDes& aRealAddress,
    TInt aMaxLength,
    const TDesC& aOpen,
    const TDesC& aClose )
    {
    aRealAddress.Zero();
    aType = EMmsAddressTypeUnknown;

    TPtrC realAddress;
    realAddress.Set( PureAddress( aAddress, aOpen, aClose ) );

    if ( realAddress.Length() > aMaxLength )
        {
        return KErrTooBig;
        }

    if ( IsValidMMSPhoneAddress( realAddress ) )
        {
        aType = EMmsAddressTypeMobile;
        }
    else if ( IsValidEmailAddress( realAddress ) )
        {
        aType = EMmsAddressTypeEmail;
        }
    else
        {
    	
        }

    // we returned earlier if address was too big.
    if ( aType != EMmsAddressTypeUnknown )
        {
        aRealAddress.Copy( realAddress );
        }

    return KErrNone; 
    }

// ---------------------------------------------------------
// TMmsGenUtils::IsValidAddress
// ---------------------------------------------------------
//
EXPORT_C TBool TMmsGenUtils::IsValidAddress( 
   const TDesC& aAddress,
   TBool aReal,
   const TDesC& aOpen,
   const TDesC& aClose )
   {
   TPtrC realAddress;

   if ( !aReal)
       {
       realAddress.Set( PureAddress( aAddress, aOpen, aClose ) );
       }
   else
       {
       realAddress.Set ( aAddress );
       }

   if ( !IsValidMMSPhoneAddress( realAddress ) )
       {
       return IsValidEmailAddress( realAddress );
       }

   return ETrue;
   }

// ---------------------------------------------------------
// TMmsGenUtils::IsValidEmailAddress
// ---------------------------------------------------------
//          
EXPORT_C TBool TMmsGenUtils::IsValidEmailAddress( 
    const TDesC& aAddress,
    TBool aReal,
    const TDesC& aOpen,
    const TDesC& aClose )
    {
    // Strip off alias part if necessary
    TPtrC realAddress;
    if ( aReal )
      {
      realAddress.Set( aAddress ); 
      }
    else
      {
      realAddress.Set( PureAddress( aAddress, aOpen, aClose ) );
      }
    TImMessageField email;
    return email.ValidInternetEmailAddress( realAddress );
    }

// ---------------------------------------------------------
// TMmsGenUtils::IsValidMMSPhoneAddress
// ---------------------------------------------------------
//
EXPORT_C TBool TMmsGenUtils::IsValidMMSPhoneAddress( 
  const TDesC& aAddress,
  TBool aReal,
  const TDesC& aOpen,
  const TDesC& aClose  )
  {
  // global_phone_number = [+] 1*(DIGIT, written-sep)
  // written_sep = ('-')

  // Strip off alias part if necessary
  TPtrC realAddress;
  if ( aReal )
      {
      realAddress.Set( aAddress ); 
      }
  else
      {
      realAddress.Set( PureAddress( aAddress, aOpen, aClose ) );
      }

  TInt length = realAddress.Length();

  if ( length == 0 )
      {
      return EFalse;
      }

  TInt pos = 0;
  TInt ich = realAddress[0];
  TChar ch = ich;
  if ( ch == '+' )
      {
      pos++;
      }

  if ( pos == length )
      {
      return EFalse;
      }

  while ( pos < length )
      {
      ich = realAddress[pos];
      ch = ich;
      ch.Fold(); 
      TInt fch = ch;
        // do not check for fch == '.'
	//Even # and * are valid characters now
        if ( !( ch.IsDigit() || fch == '-' || fch == '#' || fch == '*' ) )
          {
          return EFalse;
          }
      pos++;
      }

  return ETrue;
  }


// ---------------------------------------------------------
// TMmsGenUtils::Alias
// ---------------------------------------------------------
//
EXPORT_C TPtrC TMmsGenUtils::Alias( 
    const TDesC& aAddress,
    const TDesC& aOpen,
    const TDesC& aClose )
    {

    // syntax is :
    // <alias><separator1><pure_address><separator2> |
    // <pure_address>
    TInt firstPos = 0;
    TInt lastPos = 0;
    TInt length = aAddress.Length();
    TInt sepaLen1 = aOpen.Length();
    TInt sepaLen2 = aClose.Length();
    TInt firstSeparatorPosition = 0;

    while( firstSeparatorPosition >= 0 )
        {
        firstSeparatorPosition = aAddress.Mid( firstPos ).Find( aOpen );
        if ( firstSeparatorPosition >= 0 )
            {
            firstPos += firstSeparatorPosition + sepaLen1;
            }
        }
    if ( firstPos <= 0 )
        {
        // No alias
        return TPtrC();
        }

    // Search another separator after the first separator from
    lastPos = aAddress.Mid( firstPos ).Find( aClose );
    if ( lastPos == KErrNotFound )
        {
        return TPtrC();
        }
    firstPos -= sepaLen1; // point to first separator    
    if ( lastPos == length - firstPos - sepaLen1 - sepaLen2 )
        {
        // Alias part found
        // remove trailing and leading spaces
        lastPos = firstPos;
        firstPos = 0;
        while ( firstPos < aAddress.Length() &&
            aAddress.Mid( firstPos, 1 ).Compare( KSpace16 ) == 0 )
            {
            // remove leading spaces
            firstPos++;
            }
        while ( lastPos > 0 && aAddress.Mid( lastPos - 1, 1 ).Compare( KSpace16 ) == 0 )
            {
            lastPos--;
            }
        if ( lastPos > firstPos )
            {
            return aAddress.Mid( firstPos, lastPos - firstPos );
            }
        }
    // No alias defined - spaces alone do not count as alias.
    return TPtrC();
    }

// ---------------------------------------------------------
// TMmsGenUtils::PureAddress
// ---------------------------------------------------------
//
EXPORT_C TPtrC TMmsGenUtils::PureAddress( 
    const TDesC& aAddress,
    const TDesC& aOpen,
    const TDesC& aClose )
    {
    // syntax is :
    // <alias><separator1><pure_address><separator2> |
    // <pure_address>
    TInt firstPos = 0;
    TInt lastPos = 0;
    TInt length = aAddress.Length();
    TInt sepaLen1 = aOpen.Length();
    TInt sepaLen2 = aClose.Length();
    TInt firstSeparatorPosition = 0;
    
    while( firstSeparatorPosition >= 0 )
        {
        firstSeparatorPosition = aAddress.Mid( firstPos ).Find( aOpen );
        if ( firstSeparatorPosition >= 0 )
            {
            firstPos += firstSeparatorPosition + sepaLen1;
            }
        }
    if ( firstPos <= 0 )
        {
        // No alias
        return aAddress;
        }
        
    // Check if the second separator ends the address
    TPtrC last = aAddress.Right( sepaLen2 );
    lastPos = length - sepaLen2;
    
    if ( !last.Compare( aClose ) )
        {
        // Alias part found
        if ( lastPos > firstPos )
            {
            return aAddress.Mid( firstPos, lastPos - firstPos );
            }
        }
    // No alias defined - return the original string as pure address
    // If syntax is weird, namely 
    // alias <>
    // with nothing between the separators, we return the original string as is
    return aAddress;
    }

// ---------------------------------------------------------
// TMmsGenUtils::GenerateDetails
// ---------------------------------------------------------
//
EXPORT_C TInt TMmsGenUtils::GenerateDetails( 
    const TDesC& aAddress,
    TDes& aAlias,
    TInt aMaxLength,
    RFs& aFs ) 
    {
    
    TInt err = KErrNone;
    
    // alias search order: Local alias, remote alias, none
    
    TPtrC realAddress;
    realAddress.Set( PureAddress( aAddress, KSepaOpen, KSepaClose ) );

    TRAP( err, DoGetAliasL( aFs, realAddress, aAlias, aMaxLength ) );
    if ( err != KErrNone || aAlias.Length() == 0 )
        {
        // No alias found from Contact db, see if there is a local alias
        TPtrC myAddress = Alias( aAddress, KSepaOpen, KSepaClose );
        if ( myAddress.Length() > 0 )
            {
            // Alias was found from the address
            aAlias.Copy( myAddress.Left( aMaxLength ) );
            return KErrNone;
            }
        else
            {
            if ( err == KErrNotFound )
                {
                err = KErrNone;
                }
            // just keep the original address
            aAlias.Copy( aAddress.Left( aMaxLength ) );
            }
        }

    return err;

    }
    

// ---------------------------------------------------------
// TMmsGenUtils::GetAlias
// ---------------------------------------------------------
//
EXPORT_C TInt TMmsGenUtils::GetAlias( 
    const TDesC& aAddress,
    TDes& aAlias,
    TInt aMaxLength,
    RFs& aFs )
    {
    TInt err = KErrNone;
    TRAP( err, DoGetAliasL( aFs, aAddress, aAlias, aMaxLength ) );
    if ( err == KErrNotFound )
        {
        err = KErrNone;
        }
    return err;
    }

    
// ---------------------------------------------------------
// TMmsGenUtils::GetAliasForAllL
// This function searches aliases for all addresses in a 
// address field by opening the contact db only once for all 
// addresses. This significantly reduces processing time 
// when executing "Create Reply to all" with a lot of addresses
// ---------------------------------------------------------
// 
EXPORT_C void TMmsGenUtils::GetAliasForAllL(
    const CDesCArray& aAddress,
    CDesCArray& aAlias,
    TInt aMaxLength,
    RFs& aFs )
    {
    
    if ( aMaxLength <= 0 )
        {
        User::Leave( KErrArgument );
        }
    
    TUint stack = 0;

    //Let's find the number of digits to match
    TInt digitsToMatch = DigitsToMatch();
    TInt err = KErrNone;
    
    // Use contact wrapper to open all databases
    CContactMatcher* contactMatcher = OpenAllStoresL( aFs );
    CleanupStack::PushL( contactMatcher );
    stack++;
    
    HBufC* helpBuffer = HBufC::NewL( aMaxLength );
    CleanupStack::PushL( helpBuffer );
    stack++; 
       
    TPtr pHelpBuffer = helpBuffer->Des();   
   
    for ( TInt i = 0; i < aAddress.MdcaCount(); i++  )
        {  
        // We trap these one by one in order to be able
        // to continue to next match in case of error (not found)
        
        // make sure the alias is empty if nothing found
        pHelpBuffer.Zero();
        
    	TRAP( err, DoGetAliasL( 
            aAddress.MdcaPoint(i), 
    	    pHelpBuffer, 
    	    aMaxLength, 
    	    *contactMatcher, 
    	    digitsToMatch ) );

        // if alias is not found, we'll have an empty buffer
        // We have to insert it anyway to keep the indexes correct
        // as we have two parallel arrays
        aAlias.InsertL( i, pHelpBuffer );
        }
        
    // closing is best effort only.    
    TRAP_IGNORE( contactMatcher->CloseStoresL() );
    CleanupStack::PopAndDestroy( stack, contactMatcher );  //contactMatcher, helpBuffer   
    
    }

// ---------------------------------------------------------
// TMmsGenUtils::GenerateAddress
// ---------------------------------------------------------
//
EXPORT_C HBufC* TMmsGenUtils::GenerateAddressL(
    const TDesC& aRealAddress,
    const TDesC& aAlias,
    const TDesC& aOpen,
    const TDesC& aClose )
    {
    TInt sepaLen1 = aOpen.Length();
    TInt sepaLen2 = aClose.Length();
    TInt length = aRealAddress.Length() + aAlias.Length() + sepaLen1 + sepaLen2;
    HBufC* buf = HBufC::NewL( length );
    buf->Des().Copy( aAlias );
    buf->Des().Append( aOpen );
    buf->Des().Append( aRealAddress );
    buf->Des().Append( aClose );
    return buf;
    }

// ---------------------------------------------------------
// TMmsGenUtils::GetDescriptionL
// ---------------------------------------------------------
//
EXPORT_C void TMmsGenUtils::GetDescriptionL( 
    RFs& aFs,
    const TDesC& aPath,
    TInt aFileSize,
    TPtrC8 aMimetype,
    TInt aCharSet,
    TDes& aDescription )
    {
    TInt fileSize = aFileSize;  // file size in characters
    TInt error = KErrNone;
    // set empty description if we cannot get anything
    aDescription = TPtrC();

    // No subject set, so we have to 
    // find the first text/plain attachment.

    // Update iDescription if necessary 
    
    if ( aMimetype.CompareF( KMmsTextPlain ))
        {
        // no description available
        return;
        }
    
    TInt outLength = aDescription.MaxLength();
                
    // Open the attachment file
    RFileReadStream reader;
    reader.PushL(); 
    error = reader.Open( aFs, 
        aPath, 
        EFileShareReadersOnly );
    if ( error != KErrNone )
        {
        CleanupStack::PopAndDestroy( &reader );  //reader
        // cannot open file, cannot get description
        return;
        }

    TInt firstSize = 0;
    if ( TUint( aCharSet ) == KMmsIso10646Ucs2 )
       {                    
       // Read the content
       TUint16 word = 0;
       TBool bom = EFalse;
       TBool nativeEndian = EFalse;

       // Check if first character is BOM and if so, then what kind it is.
       TRAP ( error, {word = reader.ReadUint16L();}); 
       if ( error != KErrNone )
           {
           CleanupStack::PopAndDestroy( &reader ); //reader
           return; // no description available        
           }

       // reserve extra space for conversion
       firstSize = outLength + KExtraSpaceForConversion10;
       HBufC* buf1 = HBufC::NewLC( firstSize ); 
       TPtr tp = buf1->Des();

       if ( word == KMmsByteOrderMark )
           {
           bom = ETrue;
           nativeEndian = ETrue;
           }
       else if ( word == KMmsReversedByteOrderMark )
           {
           bom = ETrue;
           } 
       else
           {
           }

       if ( bom )
           {
           fileSize -= 2;
           }

       fileSize = fileSize / 2;
                                                                                 
       // Read the rest of the characters
       if ( nativeEndian )
           {
           // No need for byte order changes
           reader.ReadL( tp, Min( firstSize, fileSize ));
           }
       else if ( bom )
           {
           // Change byte order.
           TUint8 byte1 = 0;
           TUint16 byte2 = 0;
           TUint16 word1 = 0;
           TInt numChars = Min( firstSize, fileSize );
           for ( TInt i = 0; i < numChars; i++ )
               {
               byte1 = reader.ReadUint8L();
               byte2 = reader.ReadUint8L();
               word1 = byte1;
               const TInt KMmsBitsInByte = 8;
               word1 <<= KMmsBitsInByte; 
               word1 |= byte2;
               tp.Append( word1 );
               }
           }
          
       else // no bom
           {
           // Return the first character if it was not BOM.
           // should not happen regularly
           // Read the characters
           reader.ReadL( tp, Min( firstSize, fileSize - 2 ));
           TBuf<2> auxBuf;
           auxBuf.Append(word);
           tp.Insert(0, auxBuf);
           } 

       // Replace CR and LF with SPACE. 
       ReplaceCRLFAndTrim( tp );

        // Set output parameter
       aDescription = tp.Left( Min( outLength, tp.Length()) );
       CleanupStack::PopAndDestroy( buf1 );  
       }

    else if ( aCharSet == KMmsUsAscii )
        {
        // reserve extra space for conversion
        firstSize = outLength + KExtraSpaceForConversion10;
        HBufC8* buf8 = HBufC8::NewLC( firstSize );
        TPtr8 tp8 = buf8->Des();             

        // Read the characters
        reader.ReadL( tp8, Min( firstSize, fileSize ));

        // Replace CR and LF with SPACE
        ReplaceCRLFAndTrim( tp8 );

        // Copy 8 bit data to 16 bit description
        HBufC* buf16 = NULL;
        buf16 = HBufC::NewLC( tp8.Length() );
        TPtr tp16 = buf16->Des(); 
        tp16.Copy( tp8 );

        // Set output parameter
        aDescription = tp16.Left( Min( outLength, tp16.Length()) );
        CleanupStack::PopAndDestroy( buf16 );  
        CleanupStack::PopAndDestroy( buf8 );
        }
    else if ( aCharSet == KMmsUtf8 )
        {
               
        if ( fileSize > KMmsMaxDescription )
           {
           fileSize = KMmsMaxDescription;
           }

        // reserve extra space for conversion
        firstSize = outLength + KExtraSpaceForConversion30;
        HBufC8* buf8 = HBufC8::NewLC( firstSize );
        TPtr8 tp8 = buf8->Des(); 

        // Read the characters
        TRAP( error, reader.ReadL( tp8, Min( firstSize, fileSize )));

        if ( error == KErrNone )
            {
            // Convert 8-bit UTF to Unicode
            HBufC* buf16 = HBufC::NewLC( tp8.Length() );
            TPtr tp16 = buf16->Des();
            CnvUtfConverter::ConvertToUnicodeFromUtf8( tp16, tp8 );

            // Replace CR and LF with SPACE
            ReplaceCRLFAndTrim( tp16 );
            
            // Set output parameter
            aDescription = tp16.Left( Min( outLength, tp16.Length()) );
            CleanupStack::PopAndDestroy( buf16 );  
            }
        CleanupStack::PopAndDestroy( buf8 );  
        }
    else
        {
    	
        }

    // Free memory
    CleanupStack::PopAndDestroy( &reader ); //reader
    }

// ---------------------------------------------------------
// TMmsGenUtils::ReplaceCRLFAndTrim
// ---------------------------------------------------------
//
EXPORT_C void TMmsGenUtils::ReplaceCRLFAndTrim( TDes16& aDes )
    {
    TInt position = -1;

    // Find all <CR> and <LF> characters and replace them with spaces

    for ( position = 0; position < aDes.Length(); position++ )
        {
        if ( aDes.Mid( position, 1 ) < KSpace16 ||
            aDes.Mid( position, 1 ) == KMmsUnicodeLineSeparator ||
            aDes.Mid( position, 1 ) == KMmsUnicodeParagraphSeparator ||
            aDes.Mid( position, 1 ) == KMmsIdeographicSpace ||
            ((TChar)aDes[position]).IsControl() )
            {
            aDes.Replace( position, 1, KSpace16 );
            }
        }
            
    // Delete leading and trailing space characters from the descriptor’s
    // data and replace each contiguous set of space characters within 
    // the data by one space character. 
    aDes.TrimAll();
    }


// ---------------------------------------------------------
// TMmsGenUtils::ReplaceCRLFAndTrim
// ---------------------------------------------------------
//
EXPORT_C void TMmsGenUtils::ReplaceCRLFAndTrim( TDes8& aDes )
    {
    // This function should be used for US-ASCII only
    TInt position = -1;

    for ( position = 0; position < aDes.Length(); position++ )
        {
        if ( aDes.Mid( position, 1 ) < KSpace8 )
             {
             aDes.Replace( position, 1, KSpace8 );
             }
        }

    // Delete leading and trailing space characters from the descriptor’s
    // data and replace each contiguous set of space characters within 
    // the data by one space character. 
    aDes.TrimAll();
    }

// ---------------------------------------------------------
// TMmsGenUtils::Log
// ---------------------------------------------------------
//
EXPORT_C void TMmsGenUtils::Log( TRefByValue<const TDesC> aFmt,...)
    {
#ifdef _DEBUG
    VA_LIST list;
    VA_START( list, aFmt );

    // Print to log file
    TBuf<KLogBufferLength> buf;
    buf.FormatList( aFmt, list );

    // Write to log file
    RFileLogger::Write( KLogDir, KLogFile, EFileLoggingModeAppend, buf );
#endif
    }

// ---------------------------------------------------------
// TMmsGenUtils::DoGetAliasL
// ---------------------------------------------------------
//
void TMmsGenUtils::DoGetAliasL(
    RFs& aFs,
    const TDesC& aAddress, 
    TDes& aAlias, 
    TInt aMaxLength )
    {
    
    //Let's find the number of digits to match
    TInt digitsToMatch = DigitsToMatch();
 
    // We have only one address and one alias to put into the array
    CDesCArray* aliasArray = new ( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( aliasArray ); 

    CDesCArray* realAddressArray = new ( ELeave )CDesCArrayFlat( 1 );
    CleanupStack::PushL( realAddressArray ); 

    realAddressArray->InsertL( 0, aAddress );
    
    // GetAliasForAllL opens contact matcher
    GetAliasForAllL( *realAddressArray, *aliasArray, aMaxLength, aFs );
    
    TInt size = aliasArray->MdcaCount();
    
    if ( size > 0 )
        {
        // only one item in our array
        aAlias.Copy( aliasArray->MdcaPoint( 0 ).Left( Min( aMaxLength, aAlias.MaxLength() ) ) );
        }
    
    CleanupStack::PopAndDestroy( realAddressArray );
    CleanupStack::PopAndDestroy( aliasArray );
    
    return;
    
    }


// ---------------------------------------------------------
// TMmsGenUtils::DoGetAliasL
// ---------------------------------------------------------
//
void TMmsGenUtils::DoGetAliasL(
    const TDesC& aAddress, 
    TDes& aAlias, 
    TInt aMaxLength,
    CContactMatcher& aContactMatcher,
    TInt aDigitsToMatch  )
    {
    // It appears that with the new phonebook system with multiple phonebooks,
    // TContactItemId type id cannot be extracted.
    // The result contains MVPbkContactLink type objects.
    
    // The given descriptor has to be at least 14 in length
    // (otherwise this method would leave later)
    if( aAlias.MaxLength() < KMinAliasMaxLength )
        {
        User::Leave( KErrBadDescriptor );
        }
    
    // These should be inline with each other, but if necessary,
    // size down aMaxLength.
    // aMaxLength, however, can be smaller than the buffer
    if ( aMaxLength > aAlias.MaxLength() )
        {
        aMaxLength = aAlias.MaxLength();
        }

    if ( aMaxLength > 0 )
        {
        // set length of alias to 0
        // this can be used to determine if alias was found
        // contact id not needed or used
        aAlias.Zero();
        }

    // Convert to real address just in case. The address should be pure already,
    // but we are just being paranoid...
    
    TPtrC realAddress;
    realAddress.Set( PureAddress( aAddress, KSepaOpen, KSepaClose ) );
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewL();
    CleanupStack::PushL( linkArray );
    
    // Check if the address is phone number or EMail address
    if ( IsValidMMSPhoneAddress( realAddress, ETrue ) )
        {
        // Lookup the telephone number in the contact database
        // For numbers shorter than 7 digits, only exact matches are returned
        
        aContactMatcher.MatchPhoneNumberL( realAddress, aDigitsToMatch, 
	        CVPbkPhoneNumberMatchStrategy::EVPbkMatchFlagsNone, *linkArray );

        // If more than one matches have been found,
        // the first one will be used.
        }
    else if ( IsValidEmailAddress( realAddress ) )
        {
        // Try to match with EMail address
                
        TVPbkFieldVersitProperty prop;
        CVPbkFieldTypeRefsList* fieldTypes = CVPbkFieldTypeRefsList::NewL();
        CleanupStack::PushL( fieldTypes );
        
        const MVPbkFieldTypeList& fieldTypeList = aContactMatcher.FieldTypes();
        const MVPbkFieldType* foundType = NULL;

        prop.SetName( EVPbkVersitNameEMAIL );
/*        
        // Remove code because matching matches properies, too.
        // We don't care about properties.
        
        // The phonebook should provide a function that allows mathcing name only
        foundType = fieldTypeList.FindMatch( prop, 0 );     
         
        if ( foundType )
            {
            fieldTypes->AppendL( *foundType );   
            }
*/
           
        // The field type matching does not work because it tries to match
        // parameters we are not interested in.
        // We try to generate a list that has all entries with versit type EMAIL
        
        TInt i;
        for ( i = 0; i < fieldTypeList.FieldTypeCount(); i++ )
            {
            foundType = &(fieldTypeList.FieldTypeAt( i ));
            if ( foundType->VersitProperties().Count() > 0
                && foundType->VersitProperties()[0].Name() == prop.Name() )
                {
                fieldTypes->AppendL( *foundType );
                }
            }
        
     
        // Here we stop at first match - email addresses should be unique
		aContactMatcher.MatchDataL( realAddress, *fieldTypes, *linkArray );
		CleanupStack::PopAndDestroy( fieldTypes );    
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    TInt nameIndex = 0; //correct index if only one match is found
    if( linkArray->Count() == 0 )
        {
        Log( _L( "No match found" ) );
        User::Leave( KErrNotFound );        
        }
    else if( linkArray->Count() > 1 )
        {
        //Multiple matches found. Get the current store single match index if any.
        nameIndex = GetCurrentStoreIndexL( *linkArray );
        if( nameIndex == KErrMultipleMatchFound )
            {
            /* No unique match in current store, Hence show the name only if all the matches have 
             * identical names
             */
            if( ShowContactNameL( linkArray, nameIndex, aContactMatcher) == EFalse)
                {
                Log( _L( "No (Perfect) match found" ) );
                User::Leave( KErrMultipleMatchFound );
                }
            }
        }        
    
    // if not interested in alias, skip this
    if ( aMaxLength > 0 )
        {
        HBufC* alias = GetContactNameL( linkArray->At(nameIndex), aContactMatcher );
        if ( alias && alias->Des().Length() > 0 )
            {
            aAlias.Copy( alias->Des().Left( aMaxLength ) );
            }
        else
            {
            aAlias.Copy( TPtrC() );
            }
        delete alias;
        alias = NULL;
        // end of part skipped if not interested in alias
        }

    linkArray->ResetAndDestroy();
    CleanupStack::PopAndDestroy( linkArray );
    }

// ---------------------------------------------------------
// TMmsGenUtils::ConvertEscapesFromUri
// ---------------------------------------------------------
//
/*TInt TMmsGenUtils::ConvertEscapesFromUri(
    const TDesC8& aInput,
    TDes8& aOutput )
    {
    TInt retval = KErrNone;

    // Checkings
    if( aOutput.MaxLength() < aInput.Length() )
        {
        retval = KErrArgument;
        return retval;
        }

    // Loop through aInput and find the number of '%' chars
    for( TUint8 i = 0; i < aInput.Length(); i++ )
        {
        if( aInput[i] == 0x25 ) // '%' found
            {
            // Store the chars representing the hexvalue
            TUint8 highbyte = aInput[i+1];
            TUint8 lowbyte  = aInput[i+2];

            // Check the bytes
            TUint8 result = 0;
            
            // Map the highbyte to correct upperbits of result
            // (In order to save code lines and keep code readable,
            //  the following does not follow the coding convention.)
            if(highbyte == 0x30) result = 0x0;
            if(highbyte == 0x31) result = 0x1;
            if(highbyte == 0x32) result = 0x2;
            if(highbyte == 0x33) result = 0x3;
            if(highbyte == 0x34) result = 0x4;
            if(highbyte == 0x35) result = 0x5;
            if(highbyte == 0x36) result = 0x6;
            if(highbyte == 0x37) result = 0x7;
            if(highbyte == 0x38) result = 0x8;
            if(highbyte == 0x39) result = 0x9;
            if(highbyte == 0x41 || highbyte == 0x61) result = 0xA;
            if(highbyte == 0x42 || highbyte == 0x62) result = 0xB;
            if(highbyte == 0x43 || highbyte == 0x63) result = 0xC;
            if(highbyte == 0x44 || highbyte == 0x64) result = 0xD;
            if(highbyte == 0x45 || highbyte == 0x65) result = 0xE;
            if(highbyte == 0x46 || highbyte == 0x66) result = 0xF;

            if( ( result == 0 ) && ( highbyte != 0x30 ) )
                {
                retval = KErrArgument;
                }
            result <<= 4;

            // Map the lowbyte to correct lowerbits of result
            // (In order to save code lines and keep code readable,
            //  the following does not follow the coding convention.)
            if(lowbyte == 0x30) result += 0x0;
            if(lowbyte == 0x31) result += 0x1;
            if(lowbyte == 0x32) result += 0x2;
            if(lowbyte == 0x33) result += 0x3;
            if(lowbyte == 0x34) result += 0x4;
            if(lowbyte == 0x35) result += 0x5;
            if(lowbyte == 0x36) result += 0x6;
            if(lowbyte == 0x37) result += 0x7;
            if(lowbyte == 0x38) result += 0x8;
            if(lowbyte == 0x39) result += 0x9;
            if(lowbyte == 0x41 || lowbyte == 0x61) result += 0xA;
            if(lowbyte == 0x42 || lowbyte == 0x62) result += 0xB;
            if(lowbyte == 0x43 || lowbyte == 0x63) result += 0xC;
            if(lowbyte == 0x44 || lowbyte == 0x64) result += 0xD;
            if(lowbyte == 0x45 || lowbyte == 0x65) result += 0xE;
            if(lowbyte == 0x46 || lowbyte == 0x66) result += 0xF;
            
            if( ( ( result & 0xF ) == 0 ) && ( lowbyte != 0x30 ) ) 
                {
                retval = KErrArgument;
                }

            // Abort if error has occurred
            if( retval != KErrNone )
                {
                return retval;
                }

            // Insert the value to output parameter
            aOutput.Append( result );
            i += 2; // Jumping over the two chars already handled
            }
        else
            {
            aOutput.Append( aInput[i] );
            }
        } // for
    return retval;
    }*/

// ---------------------------------------------------------
// TMmsGenUtils::DecodeMessageHeader
// ---------------------------------------------------------
//
EXPORT_C void TMmsGenUtils::DecodeAndConvertMessageHeaderL(
            const TDesC8& aInput,
            TDes16& aOutput,
            RFs& aFs
            )
    {
    // Create CCnvCharacterSetConverter
    CCnvCharacterSetConverter* characterSetConverter
        = CCnvCharacterSetConverter::NewL();
    CleanupStack::PushL( characterSetConverter );

    // Create CImConvertCharconv 
    // (this is a wrapper for the actual char converter)
    CImConvertCharconv* converter 
        = CImConvertCharconv::NewL( *characterSetConverter, aFs );
    CleanupStack::PushL( converter );

    // Create CImConvertHeader that actually does the task
    CImConvertHeader* headerConverter = CImConvertHeader::NewL( *converter );
    CleanupStack::PushL( headerConverter );

    // Perform the decoding and charset conversion
    headerConverter->DecodeHeaderFieldL( aInput, aOutput );

    // Clean up and return
    CleanupStack::PopAndDestroy( headerConverter );
    CleanupStack::PopAndDestroy( converter );
    CleanupStack::PopAndDestroy( characterSetConverter );
    }
    

// ---------------------------------------------------------
//
// Return the free space in a drive identified by the aDrive parameter
// and the media type of the drive.
//
// ---------------------------------------------------------
static TInt64 FreeSpaceL(RFs* aFs, TInt aDrive, TMediaType& aMediaType)
{
    RFs fs;
    TInt err = KErrNone;

    if ( !aFs )
        User::LeaveIfError(fs.Connect());  // Create temp session
    else
        fs = *aFs;

    TVolumeInfo vinfo;
    err = fs.Volume(vinfo, aDrive);

    TDriveInfo driveInfo;
    TInt errorCode = fs.Drive( driveInfo, aDrive );
    if ( errorCode == KErrNone )
        {
        aMediaType = driveInfo.iType;
        }
    else
        {
        aMediaType = EMediaUnknown;
        }

    if ( !aFs )
        fs.Close(); // Close temp. session

    if (err != KErrNone)
        {
        User::LeaveIfError(err);
        }

    return TInt64(vinfo.iFree);
}

// ---------------------------------------------------------
// TMmsGenUtils::DiskSpaceBelowCriticalLevelL
// ---------------------------------------------------------
//
EXPORT_C TBool TMmsGenUtils::DiskSpaceBelowCriticalLevelL(
    RFs* aFs, TInt aBytesToWrite, TInt aDrive)
    {
    TInt64 free;
    TInt64 criticalLevel;
    CRepository* repository = NULL;
    TMediaType mediaType = EMediaNotPresent;
    free = FreeSpaceL(aFs, aDrive, mediaType);

    TInt64 newFree = free - (TInt64)aBytesToWrite;

    // Querying Critical Level from CenRep
    TInt error = KErrNone;
    TInt level = 0;
    TRAP( error, repository = CRepository::NewL( KCRUidUiklaf ) );  
    if ( error == KErrNone)
    	{
    	error = repository->Get( KUikOODDiskCriticalThreshold, level );
        delete repository;
        if( error != KErrNone )
            {
            // Default value 0 means "anything goes"
            level = 0;
    	    }
    	}
    	
#ifdef _DEBUG
    if ( error != KErrNone )
        {
        _LIT( KMmsCriticalSpaceError, "- Get critical disk space threshold returned error %d" );
        Log( KMmsCriticalSpaceError, error );
        }
    else
        {
        _LIT( KMmsCriticalSpaceLog, "- Critical level: %d, free space: %d" );
        Log( KMmsCriticalSpaceLog, level, newFree );
        }
#endif    

    criticalLevel = level;
    return newFree <= criticalLevel;
    }

// ---------------------------------------------------------
// TMmsGenUtils::NetworkOperationsAllowed()
//
// ---------------------------------------------------------
//
EXPORT_C TBool TMmsGenUtils::NetworkOperationsAllowed()
    {
    TBool networkAllowed = ETrue; // optimist
    // If there is no such key, we will continue normally.
    // This means that in a system where online/offline mode switching
    // is not supported, we behave as we were always online
    
    CRepository* repository = NULL;
    TInt error = KErrNone;
    TInt value = ECoreAppUIsNetworkConnectionAllowed;
    TRAP( error, repository = CRepository::NewL( KCRUidCoreApplicationUIs ) );
    if( error == KErrNone )
        {
        repository->Get( KCoreAppUIsNetworkConnectionAllowed, value );
        delete repository;
        repository = NULL;
        if ( value == ECoreAppUIsNetworkConnectionNotAllowed )
            {
            networkAllowed = EFalse;
            }
        }

    return networkAllowed;
    }
    
// ---------------------------------------------------------
// TMmsGenUtils::GetLoggingSettings
// ---------------------------------------------------------
//
EXPORT_C void TMmsGenUtils::GetLoggingSettings( TBool& aDecodeLoggingOn, TBool& aDumpOn )
    {
    // Consult CenRep for decodelogging and binarydump settings
    CRepository* repository = NULL;
    // default values are false if not found in repository
    aDecodeLoggingOn = EFalse;
    aDumpOn = EFalse;
    
    TInt retval = KErrNone;
    TRAP_IGNORE( 
        {
        repository = CRepository::NewL( KUidMmsServerMtm );
        CleanupStack::PushL( repository );
        TInt temp = 0;
        retval = repository->Get( KMmsEngineDecodeLog, temp );
        if( retval == KErrNone )
            {
            aDecodeLoggingOn = ( temp != 0 );
            }
        retval = repository->Get( KMmsEngineBinaryDump, temp );
        if( retval == KErrNone )
            {
            aDumpOn = ( temp != 0 );
            }
        CleanupStack::PopAndDestroy( repository );
        repository = NULL;
        }
        );
#ifndef __WINS__
    // turn decode logging on in armv5 version
    // Release versions never log anyway
    aDecodeLoggingOn = ETrue;
#endif            
    }
    
// ---------------------------------------------------------
// TMmsGenUtils::AddAttributeL
//
// ---------------------------------------------------------
//
EXPORT_C void TMmsGenUtils::AddAttributeL(
            const TDesC& aName,
            const TDesC& aValue,
            CDesCArray& aAttributeList )
    {
    TInt position = FindAttributePosition( aName, aAttributeList );
    TInt error = KErrNone;
    
    if ( position == KErrNotFound )
        {
        // not found, append to end
        aAttributeList.AppendL( aName );
        TRAP ( error, aAttributeList.AppendL( aValue ) );
        if ( error != KErrNone )
            {
            // could not add value, delete name, too.
            // It is the last item in the list
            aAttributeList.Delete( aAttributeList.MdcaCount() - 1 );
            }
        }
    else
        {
        // delete old value and insert new one
        aAttributeList.Delete( position + 1 );
        TRAP ( error, aAttributeList.InsertL( position + 1,  aValue ) );
        if ( error != KErrNone )
            {
            // could not add value, delete name, too.
            aAttributeList.Delete( position );
            }
        }
    User::LeaveIfError( error );        
    
    }
    
// ---------------------------------------------------------
// TMmsGenUtils::GetAttributeL
//
// ---------------------------------------------------------
//
EXPORT_C TPtrC TMmsGenUtils::GetAttributeL(
            const TDesC& aName,
            const CDesCArray& aAttributeList )
    {
    TInt position = FindAttributePosition( aName, aAttributeList );
    
    if ( position == KErrNotFound )
        {
        User::Leave( KErrNotFound );
        }
    return ( aAttributeList.MdcaPoint( position + 1 ) );
    }

// ---------------------------------------------------------
// TMmsGenUtils::FindAttribute
//
// ---------------------------------------------------------
//
EXPORT_C TBool TMmsGenUtils::FindAttribute(
            const TDesC& aName,
            const CDesCArray& aAttributeList )
    {
    if ( FindAttributePosition( aName, aAttributeList ) == KErrNotFound )
        {
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------
// TMmsGenUtils::DeleteAttribute
//
// ---------------------------------------------------------
//
EXPORT_C void TMmsGenUtils::DeleteAttribute(
            const TDesC& aName,
            CDesCArray& aAttributeList )
    {
    TInt position = FindAttributePosition( aName, aAttributeList );
    
    if ( position == KErrNotFound )
        {
        return; // not found, nothing to delete
        }
    // delete both name and value
    aAttributeList.Delete( position, 2 );
    }

// ---------------------------------------------------------
// TMmsGenUtils::FindAttributePosition
//
// ---------------------------------------------------------
//
TInt TMmsGenUtils::FindAttributePosition(
           const TDesC& aName,
            const CDesCArray& aAttributeList )
    {
    TInt position = KErrNotFound;
    
    TInt i;
    
    for ( i = 0; i < aAttributeList.MdcaCount() - 1; i+=2 )
        {
        //It is not possible to index out of bound (codescanner warning)
        if ( aAttributeList[i].Compare( aName ) == 0 )
            {
            position = i;
            }
        }
    return position;
    }
    
    
// ---------------------------------------------------------
// TMmsGenUtils::DigitsToMatch
//
// ---------------------------------------------------------
TInt TMmsGenUtils::DigitsToMatch()
    {
    // Find the number of digits to be used when matching phone numbers
    TInt digitsToMatch( KMmsNumberOfDigitsToMatch );

    CRepository* repository = NULL;
    TRAPD( err, repository = CRepository::NewL( KCRUidTelConfiguration ));
    if ( err == KErrNone )
        {
        err = repository->Get( KTelMatchDigits , digitsToMatch );
        delete repository;
        if( err != KErrNone )
            {
            digitsToMatch=KMmsNumberOfDigitsToMatch;
            }
    	}
    return digitsToMatch;
    }
    
// ---------------------------------------------------------
// TMmsGenUtils::OpenAllStoresL
//
// ---------------------------------------------------------
CContactMatcher* TMmsGenUtils::OpenAllStoresL( RFs& aFs )
    {
    // Use contact wrapper to open all databases
    CContactMatcher* contactMatcher = CContactMatcher::NewL( &aFs );
    CleanupStack::PushL( contactMatcher );
    
    contactMatcher->OpenDefaultMatchStoresL();    
    
    CleanupStack::Pop( contactMatcher );
    return contactMatcher;
    }

// -----------------------------------------------------------------------------
// TMmsGenUtils::GetContactNameL
// -----------------------------------------------------------------------------
//
HBufC* TMmsGenUtils::GetContactNameL(
        const MVPbkContactLink& aContactLink,
        CContactMatcher &aContactMatcher)
    {
    Log(_L( "- TMmsGenUtils::GetContactNameL  -> start" ) );
    MVPbkStoreContact* tempContact;
    aContactMatcher.GetStoreContactL(aContactLink, &tempContact);
    tempContact->PushL();
    
    MVPbkStoreContactFieldCollection& coll = tempContact->Fields();
    HBufC* nameBuff = aContactMatcher.GetNameL( coll );
    
    CleanupStack::PopAndDestroy(tempContact); // tempContact
    
    Log( _L( "- TMmsGenUtils::GetContactNameL <- end" ) );
    return nameBuff;
    }

// -----------------------------------------------------------------------------
// TMmsGenUtils::GetContactNameInLowerCaseL
// -----------------------------------------------------------------------------
//
HBufC* TMmsGenUtils::GetContactNameInLowerCaseL(
        const MVPbkContactLink& aContactLink,
        CContactMatcher &aContactMatcher)
    {
    //get the name 
    HBufC* nameBuff =  GetContactNameL( aContactLink, aContactMatcher );
    CleanupStack::PushL( nameBuff );
       
    //Convert to lower case , since this name buffer is used to compare names.    
    HBufC* nameInLowerCase = HBufC::NewL( nameBuff->Length() + 2 );
    nameInLowerCase->Des().CopyLC( *nameBuff );
    
    CleanupStack::PopAndDestroy( nameBuff ); // nameBuff
    return nameInLowerCase;
    }

// -----------------------------------------------------------------------------
// TMmsGenUtils::ShowContactNameL
// -----------------------------------------------------------------------------
//
TBool TMmsGenUtils::ShowContactNameL(
        CVPbkContactLinkArray* aLinkArray,
        TInt &aNameIndex,
        CContactMatcher &aContactMatcher)
    {
    Log( _L("- TMmsGenUtils::ShowContactName -> start") );
    Log( _L("Contact Match statistics to follow..." ) );
    Log( _L("Match count: %d"), aLinkArray->Count() );
    /* TODO:: compare the names upto standard
     * 1. if all the names are same - display the name 
     *    eg: "abcdef xyz" && "abcdef xyz"
     * 2. find min name legth among all,(if ONLY Part-match is needed )
     *    if this length is > standard length and matches upto standard length - display the larger name.
     *    eg: abcdef xyz123,  abcdef xyz12, abcdef xyz and std length is 10,
     *        since match upto 10 chars is fine, display abcdef xyz123
     * 3. in any other case do not show name
     *    eg: abcdef xyz , abcde xyz
     *        abcdef xyz , abcdef xy
     *        abcdef xyz , abcde
     */
    TInt i, minLength = 999, maxLength = 0, length = 0, maxLengthIndex = 0, stdLength = 14;
    TBool retVal = ETrue ;
    
    for( i = 0 ; i < aLinkArray->Count(); i++ )
        {
        HBufC* alias = GetContactNameL( aLinkArray->At(i), aContactMatcher );
        Log( _L(":-> %s" ), alias->Des().PtrZ());
        length = alias->Des().Length();
        if(minLength > length)
            {
            minLength = length;
            }
        if(maxLength < length)
            {
            maxLength = length;
            maxLengthIndex = i;
            }
        delete alias;
        alias = NULL;
        }
    
    Log( _L( "Contact Lengths: Std Length  : %d\n MinLength   : %d\n MaxLength   : %d\n MaxLen index: %d" ),
            stdLength,
            minLength,
            maxLength,
            maxLengthIndex);
    
    if(minLength != maxLength)
        {
        //complete length match not possible
        retVal = EFalse;
        
        /* NOTE:
         * Uncomment below code if partial length(upto stdLength) match is sufficient, 
         * ensure stdLength is correct
         */
        /*
        if(minLength < stdLength)
            {
            retVal = EFalse;
            }
        */
        }
    
    if( retVal )
        {
        TInt ret;
        HBufC* longestName = GetContactNameInLowerCaseL( aLinkArray->At(maxLengthIndex), aContactMatcher );
        Log( _L( "Longest name:-> %s" ), longestName->Des().PtrZ());
        for ( i = 0; i < aLinkArray->Count() && retVal; i++ )
            {
            HBufC* nameI = GetContactNameInLowerCaseL( aLinkArray->At(i), aContactMatcher );
            Log( _L( "compared with -> %s" ), nameI->Des().PtrZ());
            ret = longestName->Find(nameI->Des());
            if(ret == KErrNotFound || ret != 0)
                {
                Log( _L( "Part/Full Match error/offset: %d" ), ret);
                retVal = EFalse;
                }
            delete nameI;
            nameI = NULL;
           }
        delete longestName;
        longestName = NULL;
        }

    aNameIndex = maxLengthIndex;

    Log( _L( "Final Match result : %d\n Final Match index  : %d" ), retVal, maxLengthIndex);
    Log( _L( "- TMmsGenUtils::ShowContactName <- end" ) );
    
    return retVal;
    }
    
// -----------------------------------------------------------------------------
// TMmsGenUtils::GetCurrentStoreIndexL
// -----------------------------------------------------------------------------
//
TInt TMmsGenUtils::GetCurrentStoreIndexL( CVPbkContactLinkArray& aLinkArray )
    {
    TInt curStoreIndex( KErrMultipleMatchFound );
    TInt curStoreMatchCount = 0;
    RArray<TInt> otherStoreMatchIndices;
    CleanupClosePushL( otherStoreMatchIndices );
    
    //Get the current configured contact store array(s)
    CPbk2StoreConfiguration* storeConfiguration = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( storeConfiguration );
    CVPbkContactStoreUriArray* currStoreArray = storeConfiguration->CurrentConfigurationL();
    CleanupStack::PopAndDestroy(storeConfiguration);

    if ( currStoreArray )
        {
        /* Contact's store is compared against user selected stores.
         * If contact is from such store, found index is incremented
         * else, other store contact indices are populated into array for further use
         */
        for ( TInt i = 0; i < aLinkArray.Count(); i++ )
            {
            TVPbkContactStoreUriPtr uri = aLinkArray.At(i).ContactStore().StoreProperties().Uri();
            if ( currStoreArray->IsIncluded( uri ) )
                {
                // Set index to found contact and increment the count.
                curStoreIndex = i;
                curStoreMatchCount++;
                }
            else
                {
                otherStoreMatchIndices.AppendL(i);
                }
            }
        
        delete currStoreArray;    
        if ( curStoreMatchCount > 1)
            {
            /* Multiple matches found from current user selected store(s) 
             * Delete match from other stores in aLinkArray. New aLinkArray should only contain 
             * current store contact matches, so that next level pruning can be done(e.g, names can be 
             * compared and displayed if they are identical).
             */
            for(TInt i = otherStoreMatchIndices.Count() - 1; i >= 0; i--)
                {
                aLinkArray.Delete( otherStoreMatchIndices[i] );
                }
            curStoreIndex = KErrMultipleMatchFound;
            }
        }
    CleanupStack::PopAndDestroy( &otherStoreMatchIndices );
    return curStoreIndex;
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
