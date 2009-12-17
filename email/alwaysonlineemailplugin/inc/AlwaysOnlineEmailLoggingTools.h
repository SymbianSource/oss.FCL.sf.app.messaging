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
*     Class for more comprehensive logging features
*
*/


#ifndef __ALWAYSONLINEEMAILLOGGINGTOOLS_H__
#define __ALWAYSONLINEEMAILLOGGINGTOOLS_H__

#ifdef _DEBUG
#define AO_LOGGING_TOOLS_ON
#endif

#ifdef AO_LOGGING_TOOLS_ON

#include <e32debug.h>
#include <flogger.h>
 

const TInt KAOLoggingToolsBuffer = 200;
const TInt KAOLoggingToolsMaxFuncLength = 40;
const TInt KAOLoggingToolsMaxMsgLength = 70;

#define AO_LAST_MAP_VALUE -999983
#define AO_LAST_MAP_ROW { AO_LAST_MAP_VALUE , (TText*)L" val not found!" }
#define AO_MAP_ROW(a,b) { a , (TText*)L ## #b }

// define AOLOG_USE_RBUF if you want to use RBuf (heap) instead of TBuf (stack)
// There is not used CleanupStack with RBuf because of efficiency issues
// that my cause memory leaks (and leak notifications in debug mode)
#define AO_LOG_USE_RBUF

struct AOMapVal // CSI: 80 # no need to use classes
    {
    TInt  _numVal;
    TText* _strVal;
    };

/*
*  Log filter stages
*/

enum TAOLogVerboseSt 
    {
    EAOLogSt1       = 0x00000001,  // log st1
                                       
    EAoLogSt2       = 0x00000002,  // log st2
                                            
    EAoLogSt3       = 0x00000004,  // log st3
    
    EAoLogInOut     = 0x00000008,  // in out logs
    
    EAoLogStAll     = 0xffffffff   // all
    };

/*
*  Log directory and file
*/

_LIT(KAOEmailLogToolsDir,                 "AOEmail");
_LIT(KAOEmailLogToolsFile,                "AOEmailTool.txt");


/*
*  Hard coded log verbose state
*/

const TInt KAoLoggingToolsFilter = EAoLogStAll;


/*
*  Default log strings
*/

_LIT(KAOMapError, " tableptr NULL, check AOLogT::WriteAoL implementation");
_LIT(KAOLogToolInFunc, "--> " );
_LIT(KAOLogToolOutFunc, "<-- " );
_LIT(KAOLogError, "LOGGING ERROR" );


/*
*  Value mapping table to email plugin states
*/

const AOMapVal EMailPluginStatesTable[] =
    {
    AO_MAP_ROW(0,EEmailAgentInitialised),
    AO_MAP_ROW(1,EEmailAgentIdle),
    AO_MAP_ROW(2,EEmailAgentConnecting),
    AO_MAP_ROW(3,EEmailAgentDoingFilteredPopulate),
    AO_MAP_ROW(4,EEmailAgentPlainConnecting),
    AO_MAP_ROW(5,EEmailAgentConnectingToStayOnline),
    AO_MAP_ROW(6,EEmailAgentSynchronising),
    AO_MAP_ROW(7,EEmailAgentFetching),
    AO_MAP_ROW(8,EEmailAgentAutoDisconnecting),
    AO_MAP_ROW(9,EEmailAgentUserDisconnecting),
    AO_MAP_ROW(10,EEmailAgentQueued),
    AO_MAP_ROW(11,EEmailAgentTimerWaiting),
    AO_MAP_ROW(12,EEmailAgentTimerWaitingForStart),
    AO_MAP_ROW(13,EEmailAgentTimerWaitingForStop),
    AO_MAP_ROW(14,EEmailAgentReconnecting),
    AO_MAP_ROW(15,EEmailAgentConnectFailed),
    AO_MAP_ROW(16,EEmailAgentFatalError),
    AO_MAP_ROW(17,EEmailAgentConnTerminated),
    AO_LAST_MAP_ROW
    };


const AOMapVal TManagerServerCommandsTable[] =
    {
    AO_MAP_ROW(0,EAOManagerPluginStart),
    AO_MAP_ROW(1,EAOManagerPluginStop),
    AO_MAP_ROW(2,EAOManagerStartedRoaming), 
    AO_MAP_ROW(3,EAOManagerStoppedRoaming), 
    AO_MAP_ROW(4,EAOManagerNWOpsAllowed),
    AO_MAP_ROW(5,EAOManagerNWOpsNotAllowed), 
    AO_MAP_ROW(6,EAOManagerDiskSpaceAboveCritical), 
    AO_MAP_ROW(7,EAOManagerDiskSpaceBelowCritical),
    AO_MAP_ROW(8,EAOManagerSuicideQuery),   
    AO_MAP_ROW(199,EAOManagerAOSchdulerError), 
    AO_MAP_ROW(1000,EAOManagerEmailCommandBase),
    AO_MAP_ROW(1001,EAOManagerMailboxAgentSuspend), 
    AO_MAP_ROW(1002,EAOManagerMailboxAgentResume), 
    AO_MAP_ROW(1003,EAOManagerMailboxAgentRemove),  
    AO_MAP_ROW(1004,EAOManagerEMNReceived),         
    AO_MAP_ROW(1005,EAOManagerMailboxAgentUpdateMailWhileConnected),
    AO_LAST_MAP_ROW
    };

const AOMapVal TMsvSessionEventTable[] =
    {
    AO_MAP_ROW(0,EMsvEntriesCreated),
    AO_MAP_ROW(1,EMsvEntriesChanged),
    AO_MAP_ROW(2,EMsvEntriesDeleted),
    AO_MAP_ROW(3,EMsvEntriesMoved),
    AO_MAP_ROW(4,EMsvMtmGroupInstalled),
    AO_MAP_ROW(5,EMsvMtmGroupDeInstalled),
    AO_MAP_ROW(6,EMsvGeneralError),
    AO_MAP_ROW(7,EMsvCloseSession),
    AO_MAP_ROW(8,EMsvServerReady),
    AO_MAP_ROW(9,EMsvServerFailedToStart),
    AO_MAP_ROW(10,EMsvCorruptedIndexRebuilt),
    AO_MAP_ROW(11,EMsvServerTerminated),
    AO_MAP_ROW(12,EMsvMediaChanged),
    AO_MAP_ROW(13,EMsvMediaUnavailable),
    AO_MAP_ROW(14,EMsvMediaAvailable),
    AO_MAP_ROW(15,EMsvMediaIncorrect),
    AO_MAP_ROW(16,EMsvCorruptedIndexRebuilding),
    AO_LAST_MAP_ROW
    };

const AOMapVal NormalErrorTable[] = 
    {
    AO_MAP_ROW(0, KErrNone),
    AO_MAP_ROW(-1, KErrNotFound),
    AO_MAP_ROW(-2, KErrGeneral),
    AO_MAP_ROW(-3, KErrCancel),
    AO_MAP_ROW(-4, KErrNoMemory),
    AO_MAP_ROW(-5, KErrNotSupported),
    AO_MAP_ROW(-6, KErrArgument),
    AO_MAP_ROW(-7, KErrTotalLossOfPrecision),
    AO_MAP_ROW(-8, KErrBadHandle),
    AO_MAP_ROW(-9, KErrOverflow),
    AO_MAP_ROW(-5120, KErrAoServerNotFound),
    AO_MAP_ROW(-10, KErrUnderflow),
    AO_MAP_ROW(-11, KErrAlreadyExists),
    AO_MAP_ROW(-12, KErrPathNotFound),
    AO_MAP_ROW(-13, KErrDied),
    AO_MAP_ROW(-14, KErrInUse),
    AO_MAP_ROW(-15, KErrServerTerminated),
    AO_MAP_ROW(-16, KErrServerBusy),
    AO_MAP_ROW(-17, KErrCompletion),
    AO_MAP_ROW(-18, KErrNotReady),
    AO_MAP_ROW(-19, KErrUnknown),
    AO_MAP_ROW(-20, KErrCorrupt),
    AO_MAP_ROW(-21, KErrAccessDenied),
    AO_MAP_ROW(-22, KErrLocked),
    AO_MAP_ROW(-23, KErrWrite),
    AO_MAP_ROW(-24, KErrDisMounted),
    AO_MAP_ROW(-26, KErrDiskFull),
    AO_MAP_ROW(-27, KErrBadDriver),
    AO_MAP_ROW(-28, KErrBadName),
    AO_MAP_ROW(-29, KErrCommsLineFail),
    AO_MAP_ROW(-30, KErrCommsFrame),
    AO_MAP_ROW(-31, KErrCommsOverrun),
    AO_MAP_ROW(-32, KErrCommsParity),
    AO_MAP_ROW(-33, KErrTimedOut),
    AO_MAP_ROW(-34, KErrCouldNotConnect),
    AO_MAP_ROW(-17210,KErrConnectionTerminated),
    AO_MAP_ROW(-17211,KErrCannotFindProtocol),
    AO_MAP_ROW(-35, KErrCouldNotDisconnect),
    AO_MAP_ROW(-36, KErrDisconnected),
    AO_MAP_ROW(-37, KErrBadLibraryEntryPoint),
    AO_MAP_ROW(-38, KErrBadDescriptor),
    AO_MAP_ROW(-39, KErrAbort),
    AO_MAP_ROW(-40, KErrTooBig),
    AO_MAP_ROW(-41, KErrDivideByZero),
    AO_MAP_ROW(-42, KErrBadPower),
    AO_MAP_ROW(-43, KErrDirFull),
    AO_MAP_ROW(-44, KErrHardwareNotAvailable),
    AO_MAP_ROW(-45, KErrSessionClosed),
    AO_MAP_ROW(-46, KErrPermissionDenied),
    AO_MAP_ROW(-47, KErrExtensionNotSupported),
    AO_MAP_ROW(-48, KErrCommsBreak),
    AO_MAP_ROW(-170, KPop3CannotConnect),
    AO_MAP_ROW(-171, KPop3InvalidUser),
    AO_MAP_ROW(-172, KPop3InvalidLogin),
    AO_MAP_ROW(-173, KPop3CannotCreateApopLogonString),
    AO_MAP_ROW(-174, KPop3ProblemWithRemotePopServer),
    AO_MAP_ROW(-175, KPop3CannotOpenServiceEntry),
    AO_MAP_ROW(-176, KPop3CannotSetRequiredFolderContext),
    AO_MAP_ROW(-177, KPop3InvalidApopLogin),
    AO_MAP_ROW(-180, KImskErrorDNSNotFound),
    AO_MAP_ROW(-183, KImskErrorActiveSettingIsDifferent),
    AO_MAP_ROW(-181, KImskErrorControlPanelLocked),
    AO_MAP_ROW(-182, KImskErrorISPOrIAPRecordNotFound),
    AO_MAP_ROW(-184, KImskSecuritySettingsFailed),
    AO_MAP_ROW(-200, KErrImapConnectFail),
    AO_MAP_ROW(-201, KErrImapServerFail),
    AO_MAP_ROW(-202, KErrImapServerParse),
    AO_MAP_ROW(-203, KErrImapServerBusy),
    AO_MAP_ROW(-204, KErrImapServerVersion),
    AO_MAP_ROW(-213 ,KErrImapInvalidServerResponse),
    AO_MAP_ROW(-205, KErrImapSendFail),
    AO_MAP_ROW(-206, KErrImapBadLogon),
    AO_MAP_ROW(-207, KErrImapSelectFail),
    AO_MAP_ROW(-208, KErrImapWrongFolder),
    AO_MAP_ROW(-209, KErrImapServerNoSecurity),
    AO_MAP_ROW(-210, KErrImapServerLoginDisabled),
    AO_MAP_ROW(-211, KErrImapTLSNegotiateFailed),
    AO_MAP_ROW(-212, KErrImapCantDeleteFolder),
    AO_MAP_ROW(-240, KSmtpNoMailFromErr),
    AO_MAP_ROW(-241, KSmtpUnknownErr),
    AO_MAP_ROW(-242, KSmtpBadMailFromAddress),
    AO_MAP_ROW(-243, KSmtpBadRcptToAddress),
    AO_MAP_ROW(-244, KSmtpLoginRefused),
    AO_MAP_ROW(-245, KSmtpNoMsgsToSendWithActiveSettings), 
    AO_MAP_ROW(-246, KErrSmtpTLSNegotiateFailed),
    AO_MAP_ROW(-249, KSmtpTopError),
    AO_MAP_ROW(-260, KErrPop3TLSNegotiateFailed),
    AO_MAP_ROW(-2013, KErrEtelPortNotLoanedToClient),
    AO_MAP_ROW(-2014, KErrEtelWrongModemType),
    AO_MAP_ROW(-2015, KErrEtelUnknownModemCapability),
    AO_MAP_ROW(-2016, KErrEtelAnswerAlreadyOutstanding),
    AO_MAP_ROW(-2017, KErrEtelModemNotDetected),
    AO_MAP_ROW(-2018, KErrEtelModemSettingsCorrupt),
    AO_MAP_ROW(-2019, KErrEtelPortAlreadyLoaned),
    AO_MAP_ROW(-2020, KErrEtelCallAlreadyActive),
    AO_MAP_ROW(-3001, KErrExitNoModem),
    AO_MAP_ROW(-3002, KErrExitModemError),
    AO_MAP_ROW(-3003, KErrExitLoginFail),
    AO_MAP_ROW(-3004, KErrExitScriptTimeOut),
    AO_MAP_ROW(-3005, KErrExitScriptError),
    AO_MAP_ROW(-3050, KErrIfAuthenticationFailure),
    AO_MAP_ROW(-3051, KErrIfAuthNotSecure),
    AO_MAP_ROW(-3052, KErrIfAccountDisabled),
    AO_MAP_ROW(-3053, KErrIfRestrictedLogonHours),
    AO_MAP_ROW(-3055, KErrIfNoDialInPermission),
    AO_MAP_ROW(-3056, KErrIfChangingPassword),
    AO_MAP_ROW(-3057, KErrIfCallbackNotAcceptable),
    AO_MAP_ROW(-3058, KErrIfDNSNotFound),
    AO_MAP_ROW(-3059, KErrIfLRDBadLine),
    AO_MAP_ROW(-3609, KErrGenConnNoGPRSNetwork),
    AO_MAP_ROW(-4135, KErrGprsServicesNotAllowed),
    AO_MAP_ROW(-4136, KErrGprsAndNonGprsServicesNotAllowed),
    AO_MAP_ROW(-4137, KErrGprsMSIdentityCannotBeDerivedByTheNetwork),
    AO_MAP_ROW(-4138, KErrGprsMSImplicitlyDetached),
    AO_MAP_ROW(-4141, KErrGsmMMRoamingNotAllowedInThisLocationArea),
    AO_MAP_ROW(-4144, KErrGprsMSCTemporarilyNotReachable),
    AO_MAP_ROW(-4153, KErrGprsLlcOrSndcpFailure),
    AO_MAP_ROW(-4154, KErrGprsInsufficientResources),
    AO_MAP_ROW(-4155, KErrGprsMissingorUnknownAPN),
    AO_MAP_ROW(-4156, KErrGprsUnknownPDPAddress),
    AO_MAP_ROW(-4157, KErrGprsUserAuthenticationFailure),
    AO_MAP_ROW(-4158, KErrGprsActivationRejectedByGGSN),
    AO_MAP_ROW(-4159, KErrGprsActivationRejected),
    AO_MAP_ROW(-4163, KErrGprsNSAPIAlreadyUsed),
    AO_MAP_ROW(-4164, KErrGprsRegularContextDeactivation),
    AO_MAP_ROW(-4165, KErrGprsQoSNotAccepted),
    AO_MAP_ROW(-4167, KErrGprsReactivationRequested),
    AO_MAP_ROW(-4168, KErrGprsFeatureNotSupported),
    AO_MAP_ROW(-5105, KErrInet6NoRoute),
    AO_MAP_ROW(-5120, KErrDndNameNotFound),
    AO_MAP_ROW(-6000, KErrPDPMaxContextsReached),
    AO_MAP_ROW(-6001, KErrPacketDataTsyInvalidAPN),
    AO_MAP_ROW(-30172, KErrWlanOpenAuthFailed),
    AO_MAP_ROW(-30173, KErrWlanSharedKeyAuthRequired),
    AO_MAP_ROW(-30174, KErrWlanSharedKeyAuthFailed),
    AO_MAP_ROW(-30175, KErrWlanWpaAuthRequired),
    AO_MAP_ROW(-30176, KErrWlanWpaAuthFailed),
    AO_MAP_ROW(-30177, KErrWlan802dot1xAuthFailed),
    AO_MAP_ROW(-30178, KErrWlanIllegalWpaPskKey),
    AO_MAP_ROW(-30179, KErrWlanWpaCounterMeasuresActive),
    AO_MAP_ROW(-30180, KErrWlanNetworkNotFound),
    AO_MAP_ROW(-30181, KErrWlanIllegalEncryptionKeys),
    AO_MAP_ROW(-30182, KErrWlanRoamingFailed),
    AO_MAP_ROW(-30184, KErrWlanPSKModeRequired),
    AO_MAP_ROW(-30185, KErrWlanEAPModeRequired),
    AO_MAP_ROW(-30186, KErrWlanEAPSIMFailed),
    AO_MAP_ROW(-30187, KErrWlanEAPTLSFailed),
    AO_MAP_ROW(-30188, KErrWlanEAPPEAPFailed),
    AO_MAP_ROW(-30189, KErrWlanEAPMSCHAPFailed),
    AO_MAP_ROW(-30190, KErrWlanEAPAKAFailed),
    AO_MAP_ROW(-30191, KErrWlanEAPTTLSFailed),
    AO_MAP_ROW(-30192, KErrWlanLEAPFailed),
    AO_MAP_ROW(-30193, KErrWlanEAPGTCFailed),
    AO_MAP_ROW(-30194, KErrWlanSIMNotInstalled),
    AO_MAP_ROW(-30195, KErrWlanNotSubscribed),
    AO_MAP_ROW(-30196, KErrWlanAccessBarred),
    AO_MAP_ROW(-30197, KErrWlanPasswordExpired),
    AO_MAP_ROW(-30198, KErrWlanNoDialinPermissions),
    AO_MAP_ROW(-30199, KErrWlanAccountDisabled),
    AO_MAP_ROW(-30200, KErrWlanRestrictedLogonHours),
    AO_MAP_ROW(-30201, KErrWlanServerCertificateExpired),
    AO_MAP_ROW(-30202, KErrWlanCertificateVerifyFailed),
    AO_MAP_ROW(-30203, KErrWlanNoUserCertificate),
    AO_MAP_ROW(-30204, KErrWlanNoCipherSuite),
    AO_MAP_ROW(-30205, KErrWlanUserRejected),
    AO_MAP_ROW(-30206, KErrWlanUserCertificateExpired),
    AO_MAP_ROW(-30207, KErrWlanConnAlreadyActive),
    AO_MAP_ROW(-50000, KErrGeneralConnection),
    AO_MAP_ROW(-50001, GenericMailConnectionFailure),
    AO_LAST_MAP_ROW
    };

/*
*  Map Value Categories
*/

enum TAoValueCat 
    {
    EAoMailPluginStates,
    EAoManagerServerCommands,
    EAoMsvSessionEvent,
    EAoNormalError,
    EAoLastValue
    };


/**
*
* This macros make easy to use logging interface
*
* OBJECT MACROS:
*
* Put this macro first in every function
*    
*   AOLOG_IN( "<function name>" )
*
* Used when mapping values needed, object have to be 
* created first with AOLOG_IN macro
*    
*   AOLOG_WRM( "<log msg>", <map value category>, <map value> ) 
*
* Only for message, adds function name in the begining of log text,
* object have to be created first with AOLOG_IN macro
*
*   AOLOG_WR( "<log msg>" )
* 
* Adds mapping value with message text and log filtering    
* 
*   AOLOG_WRMV( "<log msg>", <map value category>, <map value> , <log filter> )
* 
* 
* Message with log filtering, adds function name in the begining of log text
*   
*   AOLOG_WRV( "<log msg>", <log filter> )
*
* Message with number append in the end
*
*   AOLOG_WRN( "<log msg>", <number> )
*
* Message with number append in the end, log filtering
*
*   AOLOG_WRNV( "<log msg>", <number>, <log filter> )
*
* Writes descriptor in log
*
*   AOLOG_WRDES( "<log msg>", <TDesC>, <log filter> )
*
* Writes descriptor if filter is on
*
*   AOLOG_WRDESV( "<log msg>", <TDesC>, <log filter> )
*
* Writes 8-bit descriptor in log,
* NOTE: no spaces added in log message. Use spaces in log message
*
*   AOLOG_WRDES8( "<log msg>", <TDesC8>, <log filter> )
*
* Writes  8-bit descriptor if filter is on
* NOTE: no spacess added in log message. Use spaces in log message
*
*   AOLOG_WRDES8V( "<log msg>", <TDesC8>, <log filter> )
*
* STATIC MACROS:
*
* In these macros AOLOG_IN() have not to be called before use, 
* macros call static functions
*
*   AOLOG_STAWRM( "<log msg>", <map value category>, <map value> )
*
*   AOLOG_STAWR( "<log msg>" )
*
*   AOLOG_STAWRMV( "<log msg>", <map value category>, <map value>, <log filter> )
*
*   AOLOG_STAWRV( "<log msg>", <log filter> )
*
*   Message with formatted (%d) number
*
*   AOLOG_STAWRF( "<log msg>", <number> )
*
*   Message with formatted (%d) number and filtering
*
*   AOLOG_STAWRFV( "<log msg>", <number>, <log filter> )
*/


#define AOLOG_IN(a)   _LIT(_aoLtempIn, a); AOLogT _aolTempObj(_aoLtempIn);
#define AOLOG_WRM(a, b, c) { _LIT(_aoLtempWRM, a ); _aolTempObj.WriteContext(_aoLtempWRM, b, c ); }
#define AOLOG_WR(a) { _LIT(_aoLtempWR, a); _aolTempObj.WriteContext(_aoLtempWR); }

#define AOLOG_WRMV(a, b, c ,d) { if ( AOLogT::DoFilter(d) ) { \
    _LIT(_aoLtempWRMV, a ); _aolTempObj.WriteContext(_aoLtempWRMV, b, c ); } }
#define AOLOG_WRV(a, b)  { if ( AOLogT::DoFilter(b) )  { \
     _LIT(_aoLtempWRV, a); _aolTempObj.WriteContext(_aoLtempWRV); } }

#define AOLOG_WRDES(a, b)  { _LIT(_aoLtempWRDES, a); \
    _aolTempObj.WriteContextDes(_aoLtempWRDES, b); }

#define AOLOG_WRDESV(a, b, c)  { if ( AOLogT::DoFilter(c) )  { \
     _LIT(_aoLtempWRDESV, a); _aolTempObj.WriteContextDes(_aoLtempWRDESV, b); } }
     
#define AOLOG_WRDES8(a, b) { _LIT(_aoLtempWRDES8, a); \
    _aolTempObj.WriteContextDes8(_aoLtempWRDES8, b); }

#define AOLOG_WRDES8V(a, b, c) { if ( AOLogT::DoFilter(c) )  { \
     _LIT(_aoLtempWRDES8V, a); _aolTempObj.WriteContextDes8(_aoLtempWRDES8V, b); } }

#define AOLOG_WRN(a, b) { _LIT(_aoLtempWRN, a); _aolTempObj.WriteContextNum(_aoLtempWRN, b); }

#define AOLOG_WRNV(a, b, c) { if ( AOLogT::DoFilter(c) )  { \
     _LIT(_aoLtempWRNV, a); _aolTempObj.WriteContextNum(_aoLtempWRNV, b); } }

#define AOLOG_STAWR(a) { {_LIT(temp, a); AOLogT::WriteAoLog(temp);} }
#define AOLOG_STAWRF(a, b) { {_LIT(temp, a); AOLogT::WriteAoLogFormat(temp, b);} }

#ifndef AO_LOG_USE_RBUF

  #define AOLOG_STAWRM(a, b, c)  {_LIT(temp, a ); TBuf<KAOLoggingToolsBuffer> buffe (temp); \
      AOLogT::WriteAoLog(buffe, b, c );}


  #define AOLOG_STAWRMV(a, b, c ,d) { if ( AOLogT::DoFilter(d) ) { \
      _LIT(temp, a ); TBuf<KAOLoggingToolsBuffer> buffe (temp); \
      AOLogT::WriteAoLog(buffe, b, c ); } }

#else

  #define AOLOG_STAWRM(a, b, c)  {_LIT(temp, a ); RBuf rBuffe; \
      if ( !rBuffe.Create(KAOLoggingToolsBuffer) ) \
      { rBuffe.Append( temp ); AOLogT::WriteAoLog(rBuffe, b, c ); } rBuffe.Close(); }


  #define AOLOG_STAWRMV(a, b, c ,d) { if ( AOLogT::DoFilter(d) ) { \
      _LIT(temp, a ); RBuf rBuffe; if ( !rBuffe.Create(KAOLoggingToolsBuffer) )  \
        { rBuffe.Append( temp ); \
        AOLogT::WriteAoLog(rBuffe, b, c ); } rBuffe.Close(); } }

#endif

#define AOLOG_STAWRV(a, b) { if ( AOLogT::DoFilter(b) )  { \
     _LIT(_aoLtempWRV, a); AOLogT::WriteAoLog(_aoLtempWRV); } }
    
#define AOLOG_STAWRFV(a, b, c) { if ( AOLogT::DoFilter(c) )  { \
     _LIT(_aoLtempWRV, a); AOLogT::WriteAoLogFormat(_aoLtempWRV, b); } }


/**
*
*  Regular C++ class for logging
*  
*  @lib ALVAYSONLINEEMAILPLUGIN.DLL
*  @since S60 3.2
*/
class AOLogT
    {
    public:
        
        /**
        * Constructor.
        */
        
        AOLogT( const TDesC& _aolFunc );
        
        /**
        * Destructor.
        */
        
        ~AOLogT();
    
        /**
        * Writes log message and map integer value to values logical name
        * 
        * @since S60 v3.2
        * @param _aolMsg Log message to be written in file
        * @param valCat integer value category
        * @param _aolVal integer value
        */
        void WriteContext( const TDesC& _aolMsg, TAoValueCat valCat, TInt _aolVal );
        
        /**
        * Writes log message 
        * 
        * @since S60 v3.2
        * @param _aolMsg Log message to be written in file
        */
        void WriteContext( const TDesC& _aolMsg );
        
        /**
        * Writes log message and number
        * 
        * @since S60 v3.2
        * @param _aolMsg Log message to be written in file
        * @param aNum Number to be added in the end
        */
        void WriteContextNum( const TDesC& _aolMsg, TInt aNum );
        
        /**
        * Writes message and additional descriptor in log
        * 
        * @since S60 v3.2
        * @param _aolMsg Log message to be written in file
        * @param aDes descriptor to be added in the end
        */
        void WriteContextDes( const TDesC& _aolMsg, const TDesC& aDes );
 
        /**
        * Writes message and additional 8-bit descriptor in log
        * 
        * @since S60 v3.2
        * @param _aolMsg Log message to be written in file
        * @param aDes8 8-bit descriptor to be added in the end
        */       
        void WriteContextDes8( const TDesC& _aolMsg, const TDesC8& aDes8 );
        
        /**
        * Writes log message
        * 
        * @since S60 v3.2
        * @param _aolMsg Log message to be written in file
        */
        static void WriteAoLog( const TDesC& _aolMsg );
        
        /**
        * Writes formatted log message and number
        * 
        * @since S60 v3.2
        * @param _aolMsg Log message to be written in file, message have to contain %s
        * @param aNum Number to be added in the end
        */
        static void WriteAoLogFormat( const TDesC& _aolMsgFor, TInt aNum );
        
        /**
        * Writes log message and map integer value to values logical name
        * 
        * @since S60 v3.2
        * @param _aolMsg Log message to be written in file
        * @param valCat integer value category
        * @param _aolVal integer value
        */
        static void WriteAoLog( TDes& _aolMsg, TAoValueCat valCat, TInt _aolVal );
        
        /**
        * Adds integers logical name string to descriptor
        * 
        * @since S60 v3.2
        * @param _aMapVal descriptor where logical name is added
        * @param _aVal integer value
        * @param _mapTable reference to integer - string pair table
        */
        static void AddMapValue(TDes& _aMapVal, TInt _aVal, const AOMapVal _mapTable[] );
        
        /**
        * Check is aFilter is contained to KAoLoggingToolsFilter flags
        * 
        * @since S60 v3.2
        * @param aFilter filter flag parameter
        * @return True if filter flag is set, 
        *         false if constant flags not contains aFilter
        */
        static TBool DoFilter(TInt aFilter);
        
    private:
        
        // name of function
        const TDesC&                    iAoFunc;
        
        // one log line
#ifndef AO_LOG_USE_RBUF
        TBuf<KAOLoggingToolsBuffer>  iAoLogBuf;
#else
        RBuf iAoLogBuf;
#endif
    };
#else

#define AOLOG_IN(a)
#define AOLOG_WRM(a, b, c)
#define AOLOG_WR(a)
#define AOLOG_WRMV(a, b, c ,d)
#define AOLOG_WRV(a, b)
#define AOLOG_WRDES(a, b)
#define AOLOG_WRDESV(a, b, c)
#define AOLOG_WRDES8(a, b)
#define AOLOG_WRDES8V(a, b, c)
#define AOLOG_WRN(a, b)
#define AOLOG_WRNV(a, b, c)
#define AOLOG_STAWR(a)
#define AOLOG_STAWRF(a, b)
#define AOLOG_STAWRM(a, b, c)
#define AOLOG_STAWRMV(a, b, c ,d)
#define AOLOG_STAWRV(a, b)
#define AOLOG_STAWRFV(a, b, c)


#endif //AO_LOGGING_TOOLS_ON

#endif // __ALWAYSONLINEEMAILLOGGINGTOOLS_H__

