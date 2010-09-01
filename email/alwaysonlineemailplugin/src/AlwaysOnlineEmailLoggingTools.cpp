/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Class for more comprehensive logging features
*
*/


#include "AlwaysOnlineEmailLoggingTools.h"

#ifdef AO_LOGGING_TOOLS_ON

// ----------------------------------------------------------------------------
// AOLogT()
// Constructor
// ----------------------------------------------------------------------------
#ifndef AO_LOG_USE_RBUF 
AOLogT::AOLogT( const TDesC& _aolFunc ) : 
    iAoFunc( _aolFunc ), iAoLogBuf( KAOLogToolInFunc )
    {
    if (DoFilter(AoLogInOut)) 
        {
        AOLogT::WriteAoLog(iAoLogBuf);   
        iAoLogBuf.Append(iAoFunc);
        AOLogT::WriteAoLog(iAoLogBuf);
        }
    }

#else
AOLogT::AOLogT( const TDesC& _aolFunc ) : iAoFunc( _aolFunc )
    {
    if ( iAoLogBuf.Create(KAOLoggingToolsBuffer) )
        {
        AOLogT::WriteAoLog(KAOLogError);
        }
    else
        {
        if (DoFilter(EAoLogInOut)) 
            {
            iAoLogBuf.Append( KAOLogToolInFunc );
            iAoLogBuf.Append(iAoFunc);
            AOLogT::WriteAoLog(iAoLogBuf);
            }
        }
    }
#endif
    
// ----------------------------------------------------------------------------
// ~AOLogT()
// Destructor
// ----------------------------------------------------------------------------
AOLogT::~AOLogT()
    {
    if (DoFilter(EAoLogInOut)) 
        {
        // write function exit to log
        iAoLogBuf.Zero();
        iAoLogBuf.Append( KAOLogToolOutFunc );
        iAoLogBuf.Append( iAoFunc );
        iAoLogBuf.Append(' ');
        AOLogT::WriteAoLog( iAoLogBuf );
        }
#ifdef AO_LOG_USE_RBUF
    iAoLogBuf.Close();
#endif
    }
    
// ----------------------------------------------------------------------------
// WriteContext()
//
// ----------------------------------------------------------------------------
void AOLogT::WriteContext( const TDesC& _aolMsg, TAoValueCat valCat, TInt _aolVal )
    {
    iAoLogBuf.Zero();
    // append function name
    iAoLogBuf.Append( iAoFunc );
    // append space
    iAoLogBuf.Append(' ');
    iAoLogBuf.Append( _aolMsg );
    AOLogT::WriteAoLog( iAoLogBuf, valCat, _aolVal );
    }
    
// ----------------------------------------------------------------------------
// WriteContext()
//
// ----------------------------------------------------------------------------
void AOLogT::WriteContext( const TDesC& _aolMsg )
    {
    iAoLogBuf.Zero();
    // append function name
    iAoLogBuf.Append( iAoFunc );
    iAoLogBuf.Append(' ');
    iAoLogBuf.Append( _aolMsg );
    AOLogT::WriteAoLog( iAoLogBuf );
    }
    
// ----------------------------------------------------------------------------
// WriteContextNum()
//
// ----------------------------------------------------------------------------
void AOLogT::WriteContextNum( const TDesC& _aolMsg, TInt aNum )
    {
    iAoLogBuf.Zero();
    iAoLogBuf.Append( iAoFunc );
    iAoLogBuf.Append(' ');
    iAoLogBuf.Append( _aolMsg );
    iAoLogBuf.Append(' ');
    iAoLogBuf.AppendNum( aNum );
    AOLogT::WriteAoLog( iAoLogBuf );
    }
// ----------------------------------------------------------------------------
// WriteContextDes()
//
// ----------------------------------------------------------------------------
void AOLogT::WriteContextDes( const TDesC& _aolMsg, const TDesC& aDes )
    {
    iAoLogBuf.Zero();
    //check that message will not be oversize and cause panic. 4 is for
    //appented extra spaces etc.
    if ( ( iAoFunc.Length() + _aolMsg.Length() + aDes.Length() + 4 ) < // CSI: 47 # see comment above
        iAoLogBuf.MaxLength() )
        {
        iAoLogBuf.Append( iAoFunc );
        iAoLogBuf.Append(' ');
        iAoLogBuf.Append( _aolMsg );
        iAoLogBuf.Append(' ');
        iAoLogBuf.Append( aDes );
        }
    else
        {
        iAoLogBuf.Append( KAOLogError );
        }
    AOLogT::WriteAoLog( iAoLogBuf );
    }
// ----------------------------------------------------------------------------
// WriteContextDes8()
//
// ----------------------------------------------------------------------------
void AOLogT::WriteContextDes8( const TDesC& _aolMsg, const TDesC8& aDes8 )
    {
    //check that there is enough room for log message 4 is for extra spaces etc
    if ( ( iAoFunc.Length() + aDes8.Length() + _aolMsg.Length() + 4 ) // CSI: 47 # see comment above
        < iAoLogBuf.MaxLength() )
        {
        //copy 8 bit descriptor
        iAoLogBuf.Copy( aDes8 );
        //insert function name to first of message
        iAoLogBuf.Insert( 0, iAoFunc );
        //then insert message
        iAoLogBuf.Insert( iAoFunc.Length()-1, _aolMsg );
        }
    else
        {
        iAoLogBuf.Zero();
        iAoLogBuf.Append( KAOLogError );
        }
    AOLogT::WriteAoLog( iAoLogBuf );
    }

// ----------------------------------------------------------------------------
// WriteAoL()
// static
// ----------------------------------------------------------------------------
void AOLogT::WriteAoLog( const TDesC& _aolMsg )
    {
    RFileLogger::Write(KAOEmailLogToolsDir, KAOEmailLogToolsFile, 
        EFileLoggingModeAppend, _aolMsg);
    }

// ----------------------------------------------------------------------------
// WriteAoLogFormat()
// static
// ----------------------------------------------------------------------------    
void AOLogT::WriteAoLogFormat( const TDesC& _aolMsgFor, TInt aNum )
    {
    RFileLogger::WriteFormat(KAOEmailLogToolsDir, KAOEmailLogToolsFile, 
        EFileLoggingModeAppend, _aolMsgFor, aNum);
    }
    
// ----------------------------------------------------------------------------
// WriteAoL()
// static
// ----------------------------------------------------------------------------
void AOLogT::WriteAoLog( TDes& _aolMsg, TAoValueCat _valCat, TInt _aolVal )
    {
    //pointer to map value table
    const AOMapVal* tableptr = NULL;

    // set correct table pointer represented in _valCat
    switch ( _valCat )
        {
        case EAoMailPluginStates:
            tableptr = EMailPluginStatesTable;
            break;
        case EAoManagerServerCommands:
            tableptr = TManagerServerCommandsTable;
            break;
        case EAoMsvSessionEvent:
            tableptr = TMsvSessionEventTable;
            break;
        case EAoNormalError:
            tableptr = NormalErrorTable;
            break;
        case EAoLastValue:
        default:
            break;
        }
    
    //space to message
    _aolMsg.Append(' ');
    
    if ( tableptr )
        {
        // add integer name to log message
        AOLogT::AddMapValue( _aolMsg, _aolVal, tableptr );
        }
    else 
        {
        // if correct table was not found add error message to log
        _aolMsg.Append( KAOMapError );
        }
    
    // write log 
    AOLogT::WriteAoLog( _aolMsg );
    }
    
// ----------------------------------------------------------------------------
// AddMapValue()
// static
// ----------------------------------------------------------------------------
void AOLogT::AddMapValue( TDes& _aMapVal, 
    TInt _aVal, const AOMapVal _mapTable[] )
    {
    // taple index
    TInt ind = 0;
    
    // lop while correct integer is found or last table end is achieved
    while ( _mapTable[ind]._numVal != _aVal &&          // CSI: 2 # can't check length
        _mapTable[ind]._numVal != AO_LAST_MAP_VALUE )   // CSI: 2 # can't check length
        {
        ++ind;
        }
    
    // adds string to log message, table contains TText null terminated strings
    TPtrC mapPtr( _mapTable[ind]._strVal );     // CSI: 2 # can't check length
    _aMapVal.Append( mapPtr );
    }
    
// ----------------------------------------------------------------------------
// DoFilter()
// static
// ----------------------------------------------------------------------------  
TBool AOLogT::DoFilter( TInt aFilter )
    {
    return ( aFilter & KAoLoggingToolsFilter );
    }

#endif //AO_LOGGING_TOOLS_ON

