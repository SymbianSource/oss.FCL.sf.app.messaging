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
* Description:   Provides CXhtmlEntityConverter class methods.
*
*/



#include <gmxmlcomposer.h>
#include <gmxmldocument.h>
#include <gmxmlparser.h>
#include <gmxmlnode.h>
#include <gmxmlelement.h>
#include <gmxmlcharacterdata.h>
#include <gmxmlcomment.h>
#include <gmxmlconstants.h>
#include <gmxmlentityconverter.h>

#include "xhtmlentityconverter.h"

const TUint KEntityStart = '&';
const TUint KEntityEnd = ';';

_LIT(KNbspEntity,"&nbsp;"); 
_LIT(KIexclEntity,"&iexcl;"); 
_LIT(KCentEntity,"&cent;"); 
_LIT(KPoundEntity,"&pound;"); 
_LIT(KCurrenEntity,"&curren;"); 
_LIT(KYenEntity,"&yen;"); 
_LIT(KBrvbarEntity,"&brvbar;"); 
_LIT(KSectEntity,"&sect;"); 
_LIT(KUmlEntity,"&uml;"); 
_LIT(KCopyEntity,"&copy;"); 
_LIT(KordfEntity,"&ordf;"); 
_LIT(KLaquoEntity,"&laquo;"); 
_LIT(KNotEntity,"&not;"); 
_LIT(KShyEntity,"&shy;"); 
_LIT(KRegEntity,"&reg;"); 
_LIT(KMacrEntity,"&macr;"); 
_LIT(KDegEntity,"&deg;"); 
_LIT(KPlusmnEntity,"&plusmn;"); 
_LIT(KSup2Entity,"&sup2;"); 
_LIT(KSup3Entity,"&sup3;"); 
_LIT(KAcuteEntity,"&acute;"); 
_LIT(KMicroEntity,"&micro;"); 
_LIT(KParaEntity,"&para;"); 
_LIT(KMiddotEntity,"&middot;"); 
_LIT(KCedilEntity,"&cedil;"); 
_LIT(KSup1Entity,"&sup1;"); 
_LIT(KOrdmEntity,"&ordm;"); 
_LIT(KRaquoEntity,"&raquo;"); 
_LIT(KFrac14Entity,"&frac14;"); 
_LIT(KFrac12Entity,"&frac12;"); 
_LIT(KFrac34Entity,"&frac34;"); 
_LIT(KIquestEntity,"&iquest;");
 
_LIT(KAgraveEntity,"&Agrave;"); 
_LIT(KAacuteEntity,"&Aacute;"); 
_LIT(KAcircEntity,"&Acirc;"); 
_LIT(KAtildeEntity,"&Atilde;"); 
_LIT(KAumlEntity,"&Auml;"); 
_LIT(KAringEntity,"&Aring;"); 
_LIT(KAeligEntity,"&AElig;"); 
_LIT(KCcedilEntity,"&Ccedil;"); 
_LIT(KEgraveEntity,"&Egrave;"); 
_LIT(KEacuteEntity,"&Eacute;"); 
_LIT(KEcircEntity,"&Ecirc;"); 
_LIT(KEumlEntity,"&Euml;"); 
_LIT(KIgraveEntity,"&Igrave;"); 
_LIT(KIacuteEntity,"&Iacute;"); 
_LIT(KIcircEntity,"&Icirc;"); 
_LIT(KIumlEntity,"&Iuml;"); 
_LIT(KEthEntity,"&ETH;"); 
_LIT(KNtildeEntity,"&Ntilde;"); 
_LIT(KOgraveEntity,"&Ograve;"); 
_LIT(KOacuteEntity,"&Oacute;");
_LIT(KOcircEntity,"&Ocirc;"); 
_LIT(KOtildeEntity,"&Otilde;");
_LIT(KOumlEntity,"&Ouml;");  
_LIT(KTimesEntity,"&times;");  
_LIT(KOslashEntity,"&Oslash;");  
_LIT(KUgraveEntity,"&Ugrave;"); 
_LIT(KUacuteEntity,"&Uacute;");
_LIT(KUcircEntity,"&Ucirc;"); 
_LIT(KUumlEntity,"&Uuml;");  
_LIT(KYacuteEntity,"&Yacute;");  
_LIT(KThornEntity,"&THORN;");  
_LIT(KSzligEntity,"&szlig;");  
_LIT(KagraveEntity,"&agrave;");  
_LIT(KaacuteEntity,"&aacute;");  
_LIT(KacircEntity,"&acirc;");  
_LIT(KatildeEntity,"&atilde;");  
_LIT(KaumlEntity,"&auml;");  
_LIT(KaringEntity,"&aring;");  
_LIT(KaeligEntity,"&aelig;");  
_LIT(KccedilEntity,"&ccedil;");  
_LIT(KegraveEntity,"&egrave;");  
_LIT(KeacuteEntity,"&eacute;");  
_LIT(KecircEntity,"&ecirc;");  
_LIT(KeumlEntity,"&euml;"); 
_LIT(KigraveEntity,"&igrave;");  
_LIT(KiacuteEntity,"&iacute;");  
_LIT(KicircEntity,"&icirc;");  
_LIT(KiumlEntity,"&iuml;");  
_LIT(KethEntity,"&eth;");  
_LIT(KntildeEntity,"&ntilde;");  
_LIT(KograveEntity,"&ograve;"); 
_LIT(KoacuteEntity,"&oacute;");
_LIT(KocircEntity,"&ocirc;"); 
_LIT(KotildeEntity,"&otilde;");
_LIT(KoumlEntity,"&ouml;");
_LIT(KDivideEntity,"&divide;");
_LIT(KoslashEntity,"&oslash;");
_LIT(KugraveEntity,"&ugrave;");
_LIT(KuacuteEntity,"&uacute;");
_LIT(KucircEntity,"&ucirc;");
_LIT(KuumlEntity,"&uuml;");
_LIT(KyacuteEntity,"&yacute;");
_LIT(KthornEntity,"&thorn;");
_LIT(KyumlEntity,"&yuml;");

_LIT(KfnofEntity,"&fnof;");

_LIT(KAlphaEntity,"&Alpha;");
_LIT(KBetaEntity,"&Beta;");
_LIT(KGammaEntity,"&Gamma;");
_LIT(KDeltaEntity,"&Delta;");
_LIT(KEpsilonEntity,"&Epsilon;");
_LIT(KZetaEntity,"&Zeta;");
_LIT(KEtaEntity,"&Eta;");
_LIT(KThetaEntity,"&Theta;");
_LIT(KIotaEntity,"&Iota;");
_LIT(KKappaEntity,"&Kappa;");
_LIT(KLambdaEntity,"&Lambda;");
_LIT(KMuEntity,"&Mu;");
_LIT(KNuEntity,"&Nu;");
_LIT(KXiEntity,"&Xi;");
_LIT(KOmicronEntity,"&Omicron;");
_LIT(KPiEntity,"&Pi;");
_LIT(KRhoEntity,"&Rho;");
_LIT(KSigmaEntity,"&Sigma;");
_LIT(KTauEntity,"&Tau;");
_LIT(KUpsilonEntity,"&Upsilon;");
_LIT(KPhiEntity,"&Phi;");
_LIT(KChiEntity,"&Chi;");
_LIT(KPsiEntity,"&Psi;");
_LIT(KOmegaEntity,"&Omega;");

_LIT(KalphaEntity,"&alpha;");
_LIT(KbetaEntity,"&beta;");
_LIT(KgammaEntity,"&gamma;");
_LIT(KdeltaEntity,"&delta;");
_LIT(KepsilonEntity,"&epsilon;");
_LIT(KzetaEntity,"&zeta;");
_LIT(KetaEntity,"&eta;");
_LIT(KthetaEntity,"&theta;");
_LIT(KiotaEntity,"&iota;");
_LIT(KkappaEntity,"&kappa;");
_LIT(KlambdaEntity,"&lambda;");
_LIT(KmuEntity,"&mu;");
_LIT(KnuEntity,"&nu;");
_LIT(KxiEntity,"&xi;");
_LIT(KomicronEntity,"&omicron;");
_LIT(KpiEntity,"&pi;");
_LIT(KrhoEntity,"&rho;");
_LIT(KsigmafEntity,"&sigmaf;");
_LIT(KsigmaEntity,"&sigma;");
_LIT(KtauEntity,"&tau;");
_LIT(KupsilonEntity,"&upsilon;");
_LIT(KphiEntity,"&phi;");
_LIT(KchiEntity,"&chi;");
_LIT(KpsiEntity,"&psi;");
_LIT(KomegaEntity,"&omega;");
_LIT(KThetasymEntity,"&thetasym;");
_LIT(KUpsihEntity,"&upsih;");
_LIT(KPivEntity,"&piv;");
//
_LIT(KbullEntity,"&bull;");
_LIT(KhellipEntity,"&hellip;");
_LIT(KprimeEntity,"&prime;");
_LIT(KPrimeEntity,"&Prime;");
_LIT(KolineEntity,"&oline;");
_LIT(KfraslEntity,"&frasl;");
//Letterlike symbols
_LIT(KweierpEntity,"&weierp;");
_LIT(KimageEntity,"&image;");
_LIT(KrealEntity,"&real;");
_LIT(KtradeEntity,"&trade;");
_LIT(KalefsymEntity,"&alefsym;");
//Arrows
_LIT(KlarrEntity,"&larr;");
_LIT(KuarrEntity,"&uarr;");
_LIT(KrarrEntity,"&rarr;");
_LIT(KdarrEntity,"&darr;");
_LIT(KharrEntity,"&harr;");
_LIT(KcrarrEntity,"&crarr;");
_LIT(KlArrEntity,"&lArr;");
_LIT(KuArrEntity,"&uArr;");
_LIT(KrArrEntity,"&rArr;");
_LIT(KdArrEntity,"&dArr;");
_LIT(KhArrEntity,"&hArr;");
//Mathematical operators
_LIT(KForallEntity,"&forall;");
_LIT(KPartEntity,"&part;");
_LIT(KExistEntity,"&exist;");
_LIT(KEmptyEntity,"&empty;");
_LIT(KNablaEntity,"&nabla;");
_LIT(KIsinEntity,"&isin;");
_LIT(KNotinEntity,"&notin;");
_LIT(KNiEntity,"&ni;");
_LIT(KProdEntity,"&prod;");
_LIT(KSumEntity,"&sum;");
_LIT(KMinusEntity,"&minus;");
_LIT(KLowastEntity,"&lowast;");
_LIT(KRadicEntity,"&radic;");
_LIT(KPropEntity,"&prop;");
_LIT(KInfinEntity,"&infin;");
_LIT(KAngEntity,"&ang;");
_LIT(KAndEntity,"&and;");
_LIT(KOrEntity,"&or;");
_LIT(KCapEntity,"&cap;");
_LIT(KCupEntity,"&cup;");
_LIT(KIntEntity,"&int;");
_LIT(KThere4Entity,"&there4;");
_LIT(KsimEntity,"&sim;");
_LIT(KCongEntity,"&cong;");
_LIT(KAsympEntity,"&asymp;");
_LIT(KNeEntity,"&ne;");
_LIT(KEquivEntity,"&equiv;");
_LIT(KLeEntity,"&le;");
_LIT(KGeEntity,"&ge;");
_LIT(KSubEntity,"&sub;");
_LIT(KSupEntity,"&sup;");
_LIT(KNsubEntity,"&nsub;");
_LIT(KSubeEntity,"&sube;");
_LIT(KSupeEntity,"&supe;");
_LIT(KOplusEntity,"&oplus;");
_LIT(KOtimesEntity,"&otimes;");
_LIT(KperpEntity,"&perp;");
_LIT(KSdotEntity,"&sdot;");

_LIT(KLceilEntity,"&lceil;");
_LIT(KRceilEntity,"&rceil;");
_LIT(KLfloorEntity,"&lfloor;");
_LIT(KRfloorEntity,"&rfloor;");
_LIT(KlangEntity,"&lang;");
_LIT(KRangEntity,"&rang;");
_LIT(KLozEntity,"&loz;");
_LIT(KSpadesEntity,"&spades;");
_LIT(KClubsEntity,"&clubs;");
_LIT(KHeartsEntity,"&hearts;");
_LIT(KdiamsEntity,"&diams;");
_LIT(KLtEntity,"&lt;");
_LIT(KGtEntity,"&gt;");
_LIT(KAmpEntity,"&amp;");
_LIT(KAposEntity,"&apos;");
_LIT(KQuotEntity,"&quot;");
_LIT(KOEligEntity,"&OElig;");
_LIT(KoeligEntity,"&oelig;");
_LIT(KScaronEntity,"&Scaron;");
_LIT(KscaronEntity,"&scaron;");
_LIT(KYumlEntity,"&Yuml;");
_LIT(KCircEntity,"&circ;");
_LIT(KtildeEntity,"&tilde;");
_LIT(KEnspEntity,"&ensp;");
_LIT(KemspEntity,"&emsp;");
_LIT(KThinspEntity,"&thinsp;");
_LIT(KZwnjEntity,"&zwnj;");
_LIT(KZwjEntity,"&zwj;");
_LIT(KLrmEntity,"&lrm;");
_LIT(KRlmEntity,"&rlm;");
_LIT(KNdashEntity,"&ndash;");
_LIT(KMdashEntity,"&mdash;");
_LIT(KLsquoEntity,"&lsquo;");
_LIT(KRsquoEntity,"&rsquo;");
_LIT(KsbquoEntity,"&sbquo;");
_LIT(KldquoEntity,"&ldquo;");
_LIT(KRdquoEntity,"&rdquo;");
_LIT(KbdquoEntity,"&bdquo;");
_LIT(KdaggerEntity,"&dagger;");
_LIT(KDaggerEntity,"&Dagger;");
_LIT(KpermilEntity,"&permil;");
_LIT(KLsaquoEntity,"&lsaquo;");
_LIT(KRsaquoEntity,"&rsaquo;");
_LIT(KEuroEntity,"&euro;");



// ---------------------------------------------------------
// CXhtmlEntityConverter::OutputComposedTextL
// ---------------------------------------------------------
//

TInt CXhtmlEntityConverter::OutputComposedTextL( CMDXMLComposer* aComposer,
    const TDesC& aTextToConvert )
    {
	TInt error = KErrNone;

	TBuf<1> oneChar;
	TInt offset = 0;
	TInt textLen = aTextToConvert.Length();

	while( error == KErrNone && (offset < textLen) )
	    {
		oneChar = aTextToConvert.Mid( offset, 1 );
		offset++;
		//return entity code if found
		//or oneChar if not
		error = aComposer->OutputDataL( MatchEntityString(oneChar) );
	    }
	return error;
    }


// ---------------------------------------------------------
// CXhtmlEntityConverter::DTDEntityToText
// ---------------------------------------------------------
//
TInt CXhtmlEntityConverter::DTDEntityToText( TDes& aTextToConvert )
    {
	TInt entityStart (-1);
    TInt currentPos (0);
    
    TBool entityFound( EFalse );

    while( currentPos < aTextToConvert.Length() )
        {
        if( aTextToConvert[currentPos] == KEntityStart )
            {
            //Found entity start
            entityFound = ETrue;
            entityStart = currentPos;
            currentPos++;
            }
        else if( aTextToConvert[currentPos] == KEntityEnd && entityFound )
            {
            // Found entity end
            if( HandleEntity(aTextToConvert ,entityStart, currentPos - entityStart + 1) )
                {
                //Continue searching from entity start
                currentPos = entityStart;
                }
            else
                {
                //If the entity was not valid we move on.
                currentPos++;
                }

            entityStart = -1;
            entityFound = EFalse;
            }
        else
            {
            currentPos++;
            }
        }
	return KErrNone;
    }

// ---------------------------------------------------------
// CXhtmlEntityConverter::HandleEntity
//
// Handles replacing of single character entity with real unicode character.
// ---------------------------------------------------------
//
TBool CXhtmlEntityConverter::HandleEntity( TDes& aData, TInt aStart, TInt aLength )
    {
    TPtrC entity = aData.Mid( aStart, aLength );

    //Buffer that holds the unicode that is used to replaces the entity
    TBuf16<1> entityCode;

    if( MatchEntityCode( entity, entityCode ) )
        {
        aData.Replace( aStart, aLength, entityCode );
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CXhtmlEntityConverter::MatchEntityCode
//
// Matches the entity with the unicode character value.
// ---------------------------------------------------------
//
TBool CXhtmlEntityConverter::MatchEntityCode( const TDesC& aEntity, TDes& aEntityCode )
    {
    TBool result( EFalse );
    aEntityCode.SetLength( 1 );

    if(aEntity == KNbspEntity) { aEntityCode[0] = 0xa0;  result = ETrue; }
    else if(aEntity == KIexclEntity) { aEntityCode[0] = 0xa1; result = ETrue; }
    else if(aEntity == KCentEntity) {  aEntityCode[0] = 0xa2; result = ETrue; }
    else if(aEntity == KPoundEntity) { aEntityCode[0] = 0xa3; result = ETrue; }
    else if(aEntity == KCurrenEntity) { aEntityCode[0] = 0xa4; result = ETrue; }
    else if(aEntity == KYenEntity) { aEntityCode[0] = 0xa5; result = ETrue; }
    else if(aEntity == KBrvbarEntity) { aEntityCode[0] = 0xa6; result = ETrue; }
    else if(aEntity == KSectEntity) { aEntityCode[0] = 0xa7; result = ETrue; }
    else if(aEntity == KUmlEntity) { aEntityCode[0] = 0xa8; result = ETrue; }
    else if(aEntity == KCopyEntity) { aEntityCode[0] = 0xa9; result = ETrue; }
    else if(aEntity == KordfEntity) { aEntityCode[0] = 0xaa; result = ETrue; }
    else if(aEntity == KLaquoEntity) { aEntityCode[0] = 0xab; result = ETrue; }
    else if(aEntity == KNotEntity) { aEntityCode[0] = 0xac; result = ETrue; }
    else if(aEntity == KShyEntity) { aEntityCode[0] = 0xad; result = ETrue; }
	else if(aEntity == KAgraveEntity) { aEntityCode[0] = 0xc0; result = ETrue; }
    else if(aEntity == KRegEntity) { aEntityCode[0] = 0xae; result = ETrue; }
    else if(aEntity == KMacrEntity) { aEntityCode[0] = 0xaf; result = ETrue; }
    else if(aEntity == KDegEntity) { aEntityCode[0] = 0xb0; result = ETrue; }
    else if(aEntity == KPlusmnEntity) { aEntityCode[0] = 0xb1; result = ETrue; }
    else if(aEntity == KSup2Entity) { aEntityCode[0] = 0xb2; result = ETrue; }
    else if(aEntity == KSup3Entity) { aEntityCode[0] = 0xb3; result = ETrue; }
    else if(aEntity == KAcuteEntity) { aEntityCode[0] = 0xb4; result = ETrue; }
    else if(aEntity == KMicroEntity) { aEntityCode[0] = 0xb5; result = ETrue; }
    else if(aEntity == KParaEntity) { aEntityCode[0] = 0xb6; result = ETrue; }
    else if(aEntity == KMiddotEntity) { aEntityCode[0] = 0xb7; result = ETrue; }
    else if(aEntity == KCedilEntity) { aEntityCode[0] = 0xb8; result = ETrue; }
    else if(aEntity == KSup1Entity) { aEntityCode[0] = 0xb9; result = ETrue; }
    else if(aEntity == KOrdmEntity) { aEntityCode[0] = 0xba; result = ETrue; }
    else if(aEntity == KRaquoEntity) { aEntityCode[0] = 0xbb; result = ETrue; }
    else if(aEntity == KFrac14Entity) { aEntityCode[0] = 0xbc; result = ETrue; }
    else if(aEntity == KFrac12Entity) { aEntityCode[0] = 0xbd; result = ETrue; }
    else if(aEntity == KFrac34Entity) { aEntityCode[0] = 0xbe; result = ETrue; }
    else if(aEntity == KIquestEntity) { aEntityCode[0] = 0xbf; result = ETrue; }
	else if(aEntity == KCircEntity) { aEntityCode[0] = 0x2c6; result = ETrue; }
	else if(aEntity == KtildeEntity) { aEntityCode[0] = 0x2dc; result = ETrue; }
	else if(aEntity == KEnspEntity) { aEntityCode[0] = 0x2002; result = ETrue; }
	else if(aEntity == KemspEntity) { aEntityCode[0] = 0x2003; result = ETrue; }
	else if(aEntity == KThinspEntity) { aEntityCode[0] = 0x2009; result = ETrue; }
	else if(aEntity == KZwnjEntity) { aEntityCode[0] = 0x200c; result = ETrue; }
	else if(aEntity == KZwjEntity) { aEntityCode[0] = 0x200d; result = ETrue; }
	else if(aEntity == KLrmEntity) { aEntityCode[0] = 0x200e; result = ETrue; }
	else if(aEntity == KRlmEntity) { aEntityCode[0] = 0x200f; result = ETrue; }
	else if(aEntity == KNdashEntity) { aEntityCode[0] = 0x2013; result = ETrue; }
	else if(aEntity == KMdashEntity) { aEntityCode[0] = 0x2014; result = ETrue; }
	else if(aEntity == KLsquoEntity) { aEntityCode[0] = 0x2018; result = ETrue; }
	else if(aEntity == KRsquoEntity) { aEntityCode[0] = 0x2019; result = ETrue; }
	else if(aEntity == KsbquoEntity) { aEntityCode[0] = 0x201a; result = ETrue; }
	else if(aEntity == KldquoEntity) { aEntityCode[0] = 0x201c; result = ETrue; }
	else if(aEntity == KRdquoEntity) { aEntityCode[0] = 0x201d; result = ETrue; }
	else if(aEntity == KbdquoEntity) { aEntityCode[0] = 0x201e; result = ETrue; }
	else if(aEntity == KdaggerEntity) { aEntityCode[0] = 0x2020; result = ETrue; }
	else if(aEntity == KDaggerEntity) { aEntityCode[0] = 0x2021; result = ETrue; }
	else if(aEntity == KpermilEntity) { aEntityCode[0] = 0x2030; result = ETrue; }
	else if(aEntity == KLsaquoEntity) { aEntityCode[0] = 0x2039; result = ETrue; }
	else if(aEntity == KRsaquoEntity) { aEntityCode[0] = 0x203a; result = ETrue; }
	else if(aEntity == KEuroEntity) { aEntityCode[0] = 0x20ac; result = ETrue; }

    if( result )
        {
        return result;
        }
    // Capital latin characters
    if(aEntity == KAgraveEntity) { aEntityCode[0] = 0xc0; result = ETrue; }
    else if(aEntity == KAacuteEntity) { aEntityCode[0] = 0xc1; result = ETrue; }
    else if(aEntity == KAcircEntity) { aEntityCode[0] = 0xc2; result = ETrue; }
    else if(aEntity == KAtildeEntity) { aEntityCode[0] = 0xc3; result = ETrue; }
    else if(aEntity == KAumlEntity) { aEntityCode[0] = 0xc4; result = ETrue; }
    else if(aEntity == KAringEntity) { aEntityCode[0] = 0xc5; result = ETrue; }
    else if(aEntity == KAeligEntity) { aEntityCode[0] = 0xc6; result = ETrue; }
    else if(aEntity == KCcedilEntity) { aEntityCode[0] = 0xc7; result = ETrue; }
    else if(aEntity == KEgraveEntity) { aEntityCode[0] = 0xc8; result = ETrue; }
    else if(aEntity == KEacuteEntity) { aEntityCode[0] = 0xc9; result = ETrue; }
    else if(aEntity == KEcircEntity) { aEntityCode[0] = 0xca; result = ETrue; }
    else if(aEntity == KEumlEntity) { aEntityCode[0] = 0xcb; result = ETrue; }
    else if(aEntity == KIgraveEntity) { aEntityCode[0] = 0xcc; result = ETrue; }
    else if(aEntity == KIacuteEntity) { aEntityCode[0] = 0xcd; result = ETrue; }
    else if(aEntity == KIcircEntity) { aEntityCode[0] = 0xce; result = ETrue; }
    else if(aEntity == KIumlEntity) { aEntityCode[0] = 0xcf; result = ETrue; }
    else if(aEntity == KEthEntity) { aEntityCode[0] = 0xd0; result = ETrue; }
    else if(aEntity == KNtildeEntity) { aEntityCode[0] = 0xd1; result = ETrue; }
    else if(aEntity == KOgraveEntity) { aEntityCode[0] = 0xd2; result = ETrue; }
    else if(aEntity == KOacuteEntity) { aEntityCode[0] = 0xd3; result = ETrue; }
    else if(aEntity == KOcircEntity) { aEntityCode[0] = 0xd4; result = ETrue; }
    else if(aEntity == KOtildeEntity) { aEntityCode[0] = 0xd5; result = ETrue; }
    else if(aEntity == KOumlEntity) { aEntityCode[0] = 0xd6; result = ETrue; }
    else if(aEntity == KTimesEntity) { aEntityCode[0] = 0xd7; result = ETrue; }
    else if(aEntity == KOslashEntity) { aEntityCode[0] = 0xd8; result = ETrue; }
    else if(aEntity == KUgraveEntity) { aEntityCode[0] = 0xd9; result = ETrue; }
    else if(aEntity == KUacuteEntity) { aEntityCode[0] = 0xda; result = ETrue; }
    else if(aEntity == KUcircEntity) { aEntityCode[0] = 0xdb; result = ETrue; }
    else if(aEntity == KUumlEntity) { aEntityCode[0] = 0xdc; result = ETrue; }
    else if(aEntity == KYacuteEntity) { aEntityCode[0] = 0xdd; result = ETrue; }
    else if(aEntity == KThornEntity) { aEntityCode[0] = 0xde; result = ETrue; }
	else if(aEntity == KOEligEntity) { aEntityCode[0] = 0x0152; result = ETrue; }
	else if(aEntity == KoeligEntity) { aEntityCode[0] = 0x0153; result = ETrue; }
	else if(aEntity == KScaronEntity) { aEntityCode[0] = 0x160; result = ETrue; }
	else if(aEntity == KscaronEntity) { aEntityCode[0] = 0x161; result = ETrue; }
	else if(aEntity == KYumlEntity) { aEntityCode[0] = 0x178; result = ETrue; }

	if( result )
        {
        return result;
        }
    // Small latin characters 
    if(aEntity == KSzligEntity) { aEntityCode[0] = 0xdf; result = ETrue; }
    else if(aEntity == KagraveEntity) { aEntityCode[0] = 0xe0; result = ETrue; }
    else if(aEntity == KaacuteEntity) { aEntityCode[0] = 0xe1; result = ETrue; }
    else if(aEntity == KacircEntity) { aEntityCode[0] = 0xe2; result = ETrue; }
    else if(aEntity == KatildeEntity) { aEntityCode[0] = 0xe3; result = ETrue; }
    else if(aEntity == KaumlEntity) { aEntityCode[0] = 0xe4; result = ETrue; }
    else if(aEntity == KaringEntity) { aEntityCode[0] = 0xe5; result = ETrue; }
    else if(aEntity == KaeligEntity) { aEntityCode[0] = 0xe6; result = ETrue; }
    else if(aEntity == KccedilEntity) { aEntityCode[0] = 0xe7; result = ETrue; }
    else if(aEntity == KegraveEntity) { aEntityCode[0] = 0xe8; result = ETrue; }
    else if(aEntity == KeacuteEntity) { aEntityCode[0] = 0xe9; result = ETrue; }
    else if(aEntity == KecircEntity) { aEntityCode[0] = 0xea; result = ETrue; }
    else if(aEntity == KeumlEntity) { aEntityCode[0] = 0xeb; result = ETrue; }
    else if(aEntity == KigraveEntity) { aEntityCode[0] = 0xec; result = ETrue; }
    else if(aEntity == KiacuteEntity) { aEntityCode[0] = 0xed; result = ETrue; }
    else if(aEntity == KicircEntity) { aEntityCode[0] = 0xee; result = ETrue; }
    else if(aEntity == KiumlEntity) { aEntityCode[0] = 0xef; result = ETrue; }
    else if(aEntity == KethEntity) { aEntityCode[0] = 0xf0; result = ETrue; }
    else if(aEntity == KntildeEntity) { aEntityCode[0] = 0xf1; result = ETrue; }
    else if(aEntity == KograveEntity) { aEntityCode[0] = 0xf2; result = ETrue; }
    else if(aEntity == KoacuteEntity) { aEntityCode[0] = 0xf3; result = ETrue; }
    else if(aEntity == KocircEntity) { aEntityCode[0] = 0xf4; result = ETrue; }
    else if(aEntity == KotildeEntity) { aEntityCode[0] = 0xf5; result = ETrue; }
    else if(aEntity == KoumlEntity) { aEntityCode[0] = 0xf6; result = ETrue; }
    else if(aEntity == KDivideEntity) { aEntityCode[0] = 0xf7; result = ETrue; }
    else if(aEntity == KoslashEntity) { aEntityCode[0] = 0xf8; result = ETrue; }
    else if(aEntity == KugraveEntity) { aEntityCode[0] = 0xf9; result = ETrue; }
    else if(aEntity == KuacuteEntity) { aEntityCode[0] = 0xfa; result = ETrue; }
    else if(aEntity == KucircEntity) { aEntityCode[0] = 0xfb; result = ETrue; }
    else if(aEntity == KuumlEntity) { aEntityCode[0] = 0xfc; result = ETrue; }
    else if(aEntity == KyacuteEntity) { aEntityCode[0] = 0xfd; result = ETrue; }
    else if(aEntity == KthornEntity) { aEntityCode[0] = 0xfe; result = ETrue; }
    else if(aEntity == KyumlEntity) { aEntityCode[0] = 0xff; result = ETrue; }

    if( result )
        {
        return result;
        }
    // Capital greek characters
    if(aEntity == KAlphaEntity) { aEntityCode[0] = 0x391; result = ETrue; }
    else if(aEntity == KBetaEntity) { aEntityCode[0] = 0x392; result = ETrue; }
    else if(aEntity == KGammaEntity) { aEntityCode[0] = 0x393; result = ETrue; }
    else if(aEntity == KDeltaEntity) { aEntityCode[0] = 0x394; result = ETrue; }
    else if(aEntity == KEpsilonEntity) { aEntityCode[0] = 0x395; result = ETrue; }
    else if(aEntity == KZetaEntity) { aEntityCode[0] = 0x396; result = ETrue; }
    else if(aEntity == KEtaEntity) { aEntityCode[0] = 0x397; result = ETrue; }
    else if(aEntity == KThetaEntity) { aEntityCode[0] = 0x398; result = ETrue; }
    else if(aEntity == KIotaEntity) { aEntityCode[0] = 0x399; result = ETrue; }
    else if(aEntity == KKappaEntity) { aEntityCode[0] = 0x39a; result = ETrue; }
    else if(aEntity == KLambdaEntity) { aEntityCode[0] = 0x39b; result = ETrue; }
    else if(aEntity == KMuEntity) { aEntityCode[0] = 0x39c; result = ETrue; }
    else if(aEntity == KNuEntity) { aEntityCode[0] = 0x39d; result = ETrue; }
    else if(aEntity == KXiEntity) { aEntityCode[0] = 0x39e; result = ETrue; }
    else if(aEntity == KOmicronEntity) { aEntityCode[0] = 0x39f; result = ETrue; }
    else if(aEntity == KPiEntity) { aEntityCode[0] = 0x3a0; result = ETrue; }
    else if(aEntity == KRhoEntity) { aEntityCode[0] = 0x3a1; result = ETrue; }
    else if(aEntity == KSigmaEntity) { aEntityCode[0] = 0x3a3; result = ETrue; }
    else if(aEntity == KTauEntity) { aEntityCode[0] = 0x3a4; result = ETrue; }
    else if(aEntity == KUpsilonEntity) { aEntityCode[0] = 0x3a5; result = ETrue; }
    else if(aEntity == KPhiEntity) { aEntityCode[0] = 0x3a6; result = ETrue; }
    else if(aEntity == KChiEntity) { aEntityCode[0] = 0x3a7; result = ETrue; }
    else if(aEntity == KPsiEntity) { aEntityCode[0] = 0x3a8; result = ETrue; }
    else if(aEntity == KOmegaEntity) { aEntityCode[0] = 0x3a9; result = ETrue; }
    // Small greek characters
    else if(aEntity == KalphaEntity) { aEntityCode[0] = 0x3b1; result = ETrue; }
    else if(aEntity == KbetaEntity) { aEntityCode[0] = 0x3b2; result = ETrue; }
    else if(aEntity == KgammaEntity) { aEntityCode[0] = 0x3b3; result = ETrue; }
    else if(aEntity == KdeltaEntity) { aEntityCode[0] = 0x3b4; result = ETrue; }
    else if(aEntity == KepsilonEntity) { aEntityCode[0] = 0x3b5; result = ETrue; }
    else if(aEntity == KzetaEntity) { aEntityCode[0] = 0x3b6; result = ETrue; }
    else if(aEntity == KetaEntity) { aEntityCode[0] = 0x3b7; result = ETrue; }
    else if(aEntity == KthetaEntity) { aEntityCode[0] = 0x3b8; result = ETrue; }
    else if(aEntity == KiotaEntity) { aEntityCode[0] = 0x3b9; result = ETrue; }
    else if(aEntity == KkappaEntity) { aEntityCode[0] = 0x3ba; result = ETrue; }
    else if(aEntity == KlambdaEntity) { aEntityCode[0] = 0x3bb; result = ETrue; }
    else if(aEntity == KmuEntity) { aEntityCode[0] = 0x3bc; result = ETrue; }
    else if(aEntity == KnuEntity) { aEntityCode[0] = 0x3bd; result = ETrue; }
    else if(aEntity == KxiEntity) { aEntityCode[0] = 0x3be; result = ETrue; }
    else if(aEntity == KomicronEntity) { aEntityCode[0] = 0x3bf; result = ETrue; }
    else if(aEntity == KpiEntity) { aEntityCode[0] = 0x3c0; result = ETrue; }
    else if(aEntity == KrhoEntity) { aEntityCode[0] = 0x3c1; result = ETrue; }
    else if(aEntity == KsigmafEntity) { aEntityCode[0] = 0x3c2; result = ETrue; }
    else if(aEntity == KsigmaEntity) { aEntityCode[0] = 0x3c3; result = ETrue; }
    else if(aEntity == KtauEntity) { aEntityCode[0] = 0x3c4; result = ETrue; }
    else if(aEntity == KupsilonEntity) { aEntityCode[0] = 0x3c5; result = ETrue; }
    else if(aEntity == KphiEntity) { aEntityCode[0] = 0x3c6; result = ETrue; }
    else if(aEntity == KchiEntity) { aEntityCode[0] = 0x3c7; result = ETrue; }
    else if(aEntity == KpsiEntity) { aEntityCode[0] = 0x3c8; result = ETrue; }
    else if(aEntity == KomegaEntity) { aEntityCode[0] = 0x3c9; result = ETrue; }
    else if(aEntity == KThetasymEntity) { aEntityCode[0] = 0x3d1; result = ETrue; }
    else if(aEntity == KUpsihEntity) { aEntityCode[0] = 0x3d2; result = ETrue; }
    else if(aEntity == KPivEntity) { aEntityCode[0] = 0x3d6; result = ETrue; }
	else if(aEntity == KfnofEntity) { aEntityCode[0] = 0x192; result = ETrue; }
	else if(aEntity == KbullEntity) { aEntityCode[0] = 0x2219; result = ETrue; }
	else if(aEntity == KhellipEntity) { aEntityCode[0] = 0x2026; result = ETrue; }
	else if(aEntity == KprimeEntity) { aEntityCode[0] = 0x2032; result = ETrue; }
	else if(aEntity == KPrimeEntity) { aEntityCode[0] = 0x2033; result = ETrue; }
	else if(aEntity == KolineEntity) { aEntityCode[0] = 0x203e; result = ETrue; }
	else if(aEntity == KfraslEntity) { aEntityCode[0] = 0x2044; result = ETrue; }

	if( result )
        {
        return result;
        }
	if(aEntity == KweierpEntity) { aEntityCode[0] = 0x2118; result = ETrue; }
	else if(aEntity == KimageEntity) { aEntityCode[0] = 0x2111; result = ETrue; }
	else if(aEntity == KrealEntity) { aEntityCode[0] = 0x211c; result = ETrue; }
	else if(aEntity == KtradeEntity) { aEntityCode[0] = 0x2122; result = ETrue; }
	else if(aEntity == KalefsymEntity) { aEntityCode[0] = 0x2135; result = ETrue; }
	else if(aEntity == KlarrEntity) { aEntityCode[0] = 0x2190; result = ETrue; }
	else if(aEntity == KuarrEntity) { aEntityCode[0] = 0x2191; result = ETrue; }
	else if(aEntity == KrarrEntity) { aEntityCode[0] = 0x2192; result = ETrue; }
	else if(aEntity == KdarrEntity) { aEntityCode[0] = 0x2193; result = ETrue; }
	else if(aEntity == KharrEntity) { aEntityCode[0] = 0x2194; result = ETrue; }
	else if(aEntity == KcrarrEntity) { aEntityCode[0] = 0x21b5; result = ETrue; }
	else if(aEntity == KlArrEntity) { aEntityCode[0] = 0x21d0; result = ETrue; }
	else if(aEntity == KuArrEntity) { aEntityCode[0] = 0x21d1; result = ETrue; }
	else if(aEntity == KrArrEntity) { aEntityCode[0] = 0x21d2; result = ETrue; }
	else if(aEntity == KdArrEntity) { aEntityCode[0] = 0x21d3; result = ETrue; }
	else if(aEntity == KhArrEntity) { aEntityCode[0] = 0x21d4; result = ETrue; }
	else if(aEntity == KForallEntity) { aEntityCode[0] = 0x2200; result = ETrue; }
	else if(aEntity == KPartEntity) { aEntityCode[0] = 0x2202; result = ETrue; }
	else if(aEntity == KExistEntity) { aEntityCode[0] = 0x2203; result = ETrue; }
	else if(aEntity == KEmptyEntity) { aEntityCode[0] = 0x2205; result = ETrue; }
	else if(aEntity == KNablaEntity) { aEntityCode[0] = 0x2207; result = ETrue; } 
	else if(aEntity == KIsinEntity) { aEntityCode[0] = 0x2208; result = ETrue; }
	else if(aEntity == KNotinEntity) { aEntityCode[0] = 0x2209; result = ETrue; }
	else if(aEntity == KNiEntity) { aEntityCode[0] = 0x220b; result = ETrue; }
	else if(aEntity == KProdEntity) { aEntityCode[0] = 0x220f; result = ETrue; }
	else if(aEntity == KSumEntity) { aEntityCode[0] = 0x2211; result = ETrue; }
	else if(aEntity == KMinusEntity) { aEntityCode[0] = 0x2212; result = ETrue; }
	else if(aEntity == KLowastEntity) { aEntityCode[0] = 0x2217; result = ETrue; }
	else if(aEntity == KRadicEntity) { aEntityCode[0] = 0x221a; result = ETrue; }
	else if(aEntity == KPropEntity) { aEntityCode[0] = 0x221d; result = ETrue; }
	else if(aEntity == KInfinEntity) { aEntityCode[0] = 0x221e; result = ETrue; }
	else if(aEntity == KAngEntity) { aEntityCode[0] = 0x2220; result = ETrue; }
	else if(aEntity == KAndEntity) { aEntityCode[0] = 0x2227; result = ETrue; }
	else if(aEntity == KOrEntity) { aEntityCode[0] = 0x2228; result = ETrue; }
	else if(aEntity == KCapEntity) { aEntityCode[0] = 0x2229; result = ETrue; }
	else if(aEntity == KCupEntity) { aEntityCode[0] = 0x222a; result = ETrue; }
	else if(aEntity == KIntEntity) { aEntityCode[0] = 0x222b; result = ETrue; }
	else if(aEntity == KThere4Entity) { aEntityCode[0] = 0x2234; result = ETrue; }
	else if(aEntity == KsimEntity) { aEntityCode[0] = 0x223c; result = ETrue; }
	else if(aEntity == KCongEntity) { aEntityCode[0] = 0x2245; result = ETrue; }
	else if(aEntity == KAsympEntity) { aEntityCode[0] = 0x2248; result = ETrue; }
	else if(aEntity == KNeEntity) { aEntityCode[0] = 0x2260; result = ETrue; }
	else if(aEntity == KEquivEntity) { aEntityCode[0] = 0x2261; result = ETrue; }
	else if(aEntity == KLeEntity) { aEntityCode[0] = 0x2264; result = ETrue; }
	else if(aEntity == KGeEntity) { aEntityCode[0] = 0x2265; result = ETrue; }
	else if(aEntity == KSubEntity) { aEntityCode[0] = 0x2282; result = ETrue; }
	else if(aEntity == KSupEntity) { aEntityCode[0] = 0x2283; result = ETrue; }
	else if(aEntity == KNsubEntity) { aEntityCode[0] = 0x2284; result = ETrue; }
	else if(aEntity == KSubeEntity) { aEntityCode[0] = 0x2286; result = ETrue; }
	else if(aEntity == KSupeEntity) { aEntityCode[0] = 0x2287; result = ETrue; }
	else if(aEntity == KOplusEntity) { aEntityCode[0] = 0x2295; result = ETrue; }
	else if(aEntity == KOtimesEntity) { aEntityCode[0] = 0x2297; result = ETrue; }
	else if(aEntity == KperpEntity) { aEntityCode[0] = 0x22a5; result = ETrue; }
	else if(aEntity == KSdotEntity) { aEntityCode[0] = 0x22c5; result = ETrue; }
	else if(aEntity == KLceilEntity) { aEntityCode[0] = 0x2308; result = ETrue; }
	else if(aEntity == KRceilEntity) { aEntityCode[0] = 0x2309; result = ETrue; }
	else if(aEntity == KLfloorEntity) { aEntityCode[0] = 0x230a; result = ETrue; }
	else if(aEntity == KRfloorEntity) { aEntityCode[0] = 0x230b; result = ETrue; }
	else if(aEntity == KlangEntity) { aEntityCode[0] = 0x2329; result = ETrue; }
	else if(aEntity == KRangEntity) { aEntityCode[0] = 0x232a; result = ETrue; }
	else if(aEntity == KLozEntity) { aEntityCode[0] = 0x25ca; result = ETrue; }
	else if(aEntity == KSpadesEntity) { aEntityCode[0] = 0x2660; result = ETrue; }
	else if(aEntity == KClubsEntity) { aEntityCode[0] = 0x2663; result = ETrue; }
	else if(aEntity == KHeartsEntity) { aEntityCode[0] = 0x2665; result = ETrue; }
	else if(aEntity == KdiamsEntity) { aEntityCode[0] = 0x2666; result = ETrue; }
	

    return result;

    }

// ---------------------------------------------------------
// CXhtmlEntityConverter::MatchEntityString
// ---------------------------------------------------------
//

TPtrC CXhtmlEntityConverter::MatchEntityString( const TDesC& aEntityCode ) const
    {
	TPtrC x;

	if(aEntityCode.Length() < 1)
    {
        x.Set(KNbspEntity);
        return x;       
    }

    if(aEntityCode[0] == 0xa0) { x.Set(KNbspEntity); return x; }
	if(aEntityCode[0] == 0xa1) { x.Set(KIexclEntity); return x; }
    if(aEntityCode[0] == 0xa2) { x.Set(KCentEntity); return x; }
    if(aEntityCode[0] == 0xa3) { x.Set(KPoundEntity); return x; }
    if(aEntityCode[0] == 0xa4) { x.Set(KCurrenEntity ); return x; }
    if(aEntityCode[0] == 0xa5) { x.Set(KYenEntity ); return x; }
    if(aEntityCode[0] == 0xa6) { x.Set(KBrvbarEntity );return x; }
    if(aEntityCode[0] == 0xa7) { x.Set(KSectEntity );return x; }
    if(aEntityCode[0] == 0xa8) { x.Set(KUmlEntity ); return x; }
    if(aEntityCode[0] == 0xa9) { x.Set(KCopyEntity ); return x; }
    if(aEntityCode[0] == 0xaa) { x.Set(KordfEntity ); return x; }
    if(aEntityCode[0] == 0xab) { x.Set(KLaquoEntity ); return x; }
    if(aEntityCode[0] == 0xac ) { x.Set(KNotEntity); return x; }
    if(aEntityCode[0] == 0xad ) { x.Set(KShyEntity); return x; }
    if(aEntityCode[0] == 0xae ) { x.Set(KRegEntity); return x; }
    if(aEntityCode[0] == 0xaf ) { x.Set(KMacrEntity); return x; }
    if(aEntityCode[0] == 0xb0 ) { x.Set(KDegEntity); return x; }
    if(aEntityCode[0] == 0xb1 ) { x.Set(KPlusmnEntity); return x; }
    if(aEntityCode[0] == 0xb2 ) { x.Set(KSup2Entity); return x; }
    if(aEntityCode[0] == 0xb3 ) { x.Set(KSup3Entity); return x; }
    if(aEntityCode[0] == 0xb4 ) { x.Set(KAcuteEntity); return x; }
    if(aEntityCode[0] == 0xb5 ) { x.Set(KMicroEntity); return x; }
    if(aEntityCode[0] == 0xb6 ) { x.Set(KParaEntity); return x; }
    if(aEntityCode[0] == 0xb7 ) { x.Set(KMiddotEntity); return x; }
    if(aEntityCode[0] == 0xb8 ) { x.Set(KCedilEntity); return x; }
    if(aEntityCode[0] == 0xb9 ) { x.Set(KSup1Entity); return x; }
    if(aEntityCode[0] == 0xba ) { x.Set(KOrdmEntity); return x; }
    if(aEntityCode[0] == 0xbb ) { x.Set(KRaquoEntity); return x; }
    if(aEntityCode[0] == 0xbc ) { x.Set(KFrac14Entity); return x;  }
    if(aEntityCode[0] == 0xbd ) { x.Set(KFrac12Entity); return x; }
    if(aEntityCode[0] == 0xbe ) { x.Set(KFrac34Entity); return x; }
    if(aEntityCode[0] == 0xbf ) { x.Set(KIquestEntity); return x; }
     // Capital latin characters
    if(aEntityCode[0] == 0xc0) { x.Set(KAgraveEntity ); return x; }
    if(aEntityCode[0] == 0xc1) { x.Set(KAacuteEntity ); return x; }
    if(aEntityCode[0] == 0xc2) { x.Set(KAcircEntity ); return x; }
    if(aEntityCode[0] == 0xc3) { x.Set(KAtildeEntity ); return x; }
    if(aEntityCode[0] == 0xc4) { x.Set(KAumlEntity ); return x; }
    if(aEntityCode[0] == 0xc5) { x.Set(KAringEntity ); return x; }
    if(aEntityCode[0] == 0xc6) { x.Set(KAeligEntity ); return x; }
    if(aEntityCode[0] == 0xc7) { x.Set(KCcedilEntity ); return x; }
    if(aEntityCode[0] == 0xc8) { x.Set(KEgraveEntity ); return x; }
    if(aEntityCode[0] == 0xc9) { x.Set(KEacuteEntity); return x; }
    if(aEntityCode[0] == 0xca) { x.Set(KEcircEntity ); return x; }
    if(aEntityCode[0] == 0xcb) { x.Set(KEumlEntity ); return x; }
    if(aEntityCode[0] == 0xcc) { x.Set(KIgraveEntity ); return x; }
    if(aEntityCode[0] == 0xcd) { x.Set(KIacuteEntity ); return x; }
    if(aEntityCode[0] == 0xce) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0xcf) { x.Set(KIumlEntity); return x; }
    if(aEntityCode[0] == 0xd0) { x.Set(KEthEntity ); return x;}
    if(aEntityCode[0] == 0xd1) { x.Set(KNtildeEntity ); return x; }
    if(aEntityCode[0] == 0xd2) { x.Set(KOgraveEntity); return x; }
    if(aEntityCode[0] == 0xd3) { x.Set(KOacuteEntity ); return x; }
    if(aEntityCode[0] == 0xd4) { x.Set(KOcircEntity ); return x; }
    if(aEntityCode[0] == 0xd5) { x.Set(KOtildeEntity ); return x; }
    if(aEntityCode[0] == 0xd6) { x.Set(KOumlEntity ); return x; }
    if(aEntityCode[0] == 0xd7) { x.Set(KTimesEntity ); return x; }
    if(aEntityCode[0] == 0xd8) { x.Set(KOslashEntity ); return x; }
    if(aEntityCode[0] == 0xd9) { x.Set(KUgraveEntity ); return x; }
    if(aEntityCode[0] == 0xda) { x.Set(KUacuteEntity ); return x; }
    if(aEntityCode[0] == 0xdb) { x.Set(KUcircEntity ); return x; }
    if(aEntityCode[0] == 0xdc) { x.Set(KUumlEntity ); return x; }
    if(aEntityCode[0] == 0xdd) { x.Set(KYacuteEntity ); return x; }
    if(aEntityCode[0] == 0xde) { x.Set(KThornEntity ); return x; }
    // Small latin characters 
    if(aEntityCode[0] == 0xdf) { x.Set(KSzligEntity ); return x; }
    if(aEntityCode[0] == 0xe0) { x.Set(KagraveEntity ); return x; }
    if(aEntityCode[0] == 0xe1) { x.Set(KaacuteEntity ); return x; }
    if(aEntityCode[0] == 0xe2) { x.Set(KacircEntity ); return x; }
    if(aEntityCode[0] == 0xe3) { x.Set(KatildeEntity ); return x; }
    if(aEntityCode[0] == 0xe4) { x.Set(KaumlEntity ); return x; }
    if(aEntityCode[0] == 0xe5) { x.Set(KaringEntity); return x; }
    if(aEntityCode[0] == 0xe6) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0xe7) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0xe8) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0xe9) { x.Set(KeacuteEntity ); return x; }
    if(aEntityCode[0] == 0xea) { x.Set(KecircEntity ); return x; }
    if(aEntityCode[0] == 0xeb) { x.Set(KecircEntity ); return x; }
    if(aEntityCode[0] == 0xec) { x.Set(KigraveEntity ); return x; }
    if(aEntityCode[0] == 0xed) { x.Set(KiacuteEntity ); return x; }
    if(aEntityCode[0] == 0xee) { x.Set(KicircEntity ); return x; }
    if(aEntityCode[0] == 0xef) { x.Set(KiumlEntity ); return x; }
    if(aEntityCode[0] == 0xf0) { x.Set(KethEntity ); return x; }
    if(aEntityCode[0] == 0xf1) { x.Set(KntildeEntity ); return x; }
    if(aEntityCode[0] == 0xf2) { x.Set(KograveEntity ); return x; }
    if(aEntityCode[0] == 0xf3) { x.Set(KoacuteEntity ); return x; }
    if(aEntityCode[0] == 0xf4) { x.Set(KocircEntity ); return x; }
    if(aEntityCode[0] == 0xf5) { x.Set(KotildeEntity ); return x; }
    if(aEntityCode[0] == 0xf6) { x.Set(KoumlEntity ); return x; }
    if(aEntityCode[0] == 0xf7) { x.Set(KDivideEntity ); return x; }
    if(aEntityCode[0] == 0xf8) { x.Set(KoslashEntity ); return x; }
    if(aEntityCode[0] == 0xf9) { x.Set(KugraveEntity ); return x; }
    if(aEntityCode[0] == 0xfa) { x.Set(KuacuteEntity ); return x; }
    if(aEntityCode[0] == 0xfb) { x.Set(KucircEntity ); return x; }
    if(aEntityCode[0] == 0xfc) { x.Set(KuumlEntity ); return x; }
    if(aEntityCode[0] == 0xfd) { x.Set(KyacuteEntity ); return x; }
    if(aEntityCode[0] == 0xfe) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0xff) { x.Set(KIcircEntity ); return x; }
    // Capital greek characters
    if(aEntityCode[0] == 0x391) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0x392) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0x393) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0x394) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0x395) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0x396) { x.Set(KIcircEntity ); return x; }
    if(aEntityCode[0] == 0x397) { x.Set(KEtaEntity ); return x; }
    if(aEntityCode[0] == 0x398) { x.Set(KThetaEntity ); return x; }
    if(aEntityCode[0] == 0x399) { x.Set(KIotaEntity ); return x; }
    if(aEntityCode[0] == 0x39a) { x.Set(KKappaEntity ); return x; }
    if(aEntityCode[0] == 0x39b) { x.Set(KLambdaEntity ); return x; }
    if(aEntityCode[0] == 0x39c) { x.Set(KMuEntity ); return x; }
    if(aEntityCode[0] == 0x39d) { x.Set(KNuEntity ); return x; }
    if(aEntityCode[0] == 0x39e) { x.Set(KXiEntity ); return x; }
    if(aEntityCode[0] == 0x39f) { x.Set(KOmicronEntity ); return x; }
    if(aEntityCode[0] == 0x3a0) { x.Set(KPiEntity ); return x; }
    if(aEntityCode[0] == 0x3a1) { x.Set(KRhoEntity ); return x; }
    if(aEntityCode[0] == 0x3a3) { x.Set(KSigmaEntity ); return x; }
    if(aEntityCode[0] == 0x3a4) { x.Set(KTauEntity ); return x; }
    if(aEntityCode[0] == 0x3a5) { x.Set(KUpsilonEntity ); return x; }
    if(aEntityCode[0] == 0x3a6) { x.Set(KPhiEntity ); return x; }
    if(aEntityCode[0] == 0x3a7) { x.Set(KChiEntity ); return x; }
    if(aEntityCode[0] == 0x3a8) { x.Set(KPsiEntity ); return x; }
    if(aEntityCode[0] == 0x3a9) { x.Set(KOmegaEntity ); return x; }
    // Small greek characters
    if(aEntityCode[0] == 0x3b1) { x.Set(KalphaEntity ); return x; }
    if(aEntityCode[0] == 0x3b2) { x.Set(KbetaEntity ); return x; }
    if(aEntityCode[0] == 0x3b3) { x.Set(KgammaEntity ); return x; }
    if(aEntityCode[0] == 0x3b4) { x.Set(KdeltaEntity ); return x; }
    if(aEntityCode[0] == 0x3b5) { x.Set(KepsilonEntity ); return x; }
    if(aEntityCode[0] == 0x3b6) { x.Set(KzetaEntity ); return x; }
    if(aEntityCode[0] == 0x3b7) { x.Set(KetaEntity ); return x; }
    if(aEntityCode[0] == 0x3b8) { x.Set(KthetaEntity ); return x; }
    if(aEntityCode[0] == 0x3b9) { x.Set(KiotaEntity ); return x; }
    if(aEntityCode[0] == 0x3ba) { x.Set(KkappaEntity ); return x; }
    if(aEntityCode[0] == 0x3bb) { x.Set(KlambdaEntity ); return x; }
    if(aEntityCode[0] == 0x3bc) { x.Set(KmuEntity ); return x; }
    if(aEntityCode[0] == 0x3bd) { x.Set(KnuEntity ); return x; }
    if(aEntityCode[0] == 0x3be) { x.Set(KxiEntity ); return x; }
    if(aEntityCode[0] == 0x3bf) { x.Set(KomicronEntity ); return x; }
    if(aEntityCode[0] == 0x3c0) { x.Set(KpiEntity ); return x; }
    if(aEntityCode[0] == 0x3c1) { x.Set(KrhoEntity ); return x; }
    if(aEntityCode[0] == 0x3c2) { x.Set(KsigmafEntity ); return x; }
    if(aEntityCode[0] == 0x3c3) { x.Set(KsigmaEntity ); return x; }
    if(aEntityCode[0] == 0x3c4) { x.Set(KtauEntity ); return x; }
    if(aEntityCode[0] == 0x3c5) { x.Set(KupsilonEntity ); return x; }
    if(aEntityCode[0] == 0x3c6) { x.Set(KphiEntity ); return x; }
    if(aEntityCode[0] == 0x3c7) { x.Set(KchiEntity ); return x; }
    if(aEntityCode[0] == 0x3c8) { x.Set(KpsiEntity ); return x; }
    if(aEntityCode[0] == 0x3c9) { x.Set(KomegaEntity ); return x; }
    if(aEntityCode[0] == 0x3d1) { x.Set(KThetasymEntity ); return x; }
    if(aEntityCode[0] == 0x3d2) { x.Set(KUpsihEntity ); return x; }
    if(aEntityCode[0] == 0x3d6) { x.Set(KPivEntity ); return x; }
//
	if(aEntityCode[0] == 0x192) { x.Set(KfnofEntity ); return x; }
    if(aEntityCode[0] == 0x2219) { x.Set(KbullEntity ); return x; }
    if(aEntityCode[0] == 0x2026) { x.Set(KhellipEntity ); return x; }
    if(aEntityCode[0] == 0x2032) { x.Set(KprimeEntity ); return x; }
    if(aEntityCode[0] == 0x2033) { x.Set(KPrimeEntity ); return x; }
    if(aEntityCode[0] == 0x203e) { x.Set(KolineEntity ); return x; }
    if(aEntityCode[0] == 0x2044) { x.Set(KfraslEntity ); return x; }
    if(aEntityCode[0] == 0x2118) { x.Set(KweierpEntity ); return x; }
    if(aEntityCode[0] == 0x2111) { x.Set(KimageEntity ); return x; }
    if(aEntityCode[0] == 0x211C) { x.Set(KrealEntity ); return x; }
    if(aEntityCode[0] == 0x2122) { x.Set(KtradeEntity ); return x; }
    if(aEntityCode[0] == 0x2135) { x.Set(KalefsymEntity ); return x; }
    if(aEntityCode[0] == 0x2190) { x.Set(KlarrEntity ); return x; }
    if(aEntityCode[0] == 0x2191) { x.Set(KuarrEntity ); return x; }
    if(aEntityCode[0] == 0x2192) { x.Set(KrarrEntity ); return x; }
    if(aEntityCode[0] == 0x2193) { x.Set(KdarrEntity ); return x; }
    if(aEntityCode[0] == 0x2194) { x.Set(KharrEntity ); return x; }
    if(aEntityCode[0] == 0x21b5) { x.Set(KlArrEntity ); return x; }
    if(aEntityCode[0] == 0x21d1) { x.Set(KuArrEntity ); return x; }
    if(aEntityCode[0] == 0x21d2) { x.Set(KrArrEntity ); return x; }
    if(aEntityCode[0] == 0x21d3) { x.Set(KdArrEntity ); return x; }
    if(aEntityCode[0] == 0x21d4) { x.Set(KhArrEntity ); return x; }
	
	if(aEntityCode[0] == 0x2200) { x.Set(KForallEntity ); return x; }
    if(aEntityCode[0] == 0x2202) { x.Set(KPartEntity ); return x; }
    if(aEntityCode[0] == 0x2203) { x.Set(KExistEntity ); return x; }
    if(aEntityCode[0] == 0x2205) { x.Set(KEmptyEntity ); return x; }
    if(aEntityCode[0] == 0x2207) { x.Set(KNablaEntity ); return x; }
	if(aEntityCode[0] == 0x2208) { x.Set(KIsinEntity ); return x; }
    if(aEntityCode[0] == 0x2209) { x.Set(KNotinEntity ); return x; }
    if(aEntityCode[0] == 0x220b) { x.Set(KNiEntity ); return x; }
    if(aEntityCode[0] == 0x220f) { x.Set(KrarrEntity ); return x; }
    if(aEntityCode[0] == 0x2211) { x.Set(KSumEntity ); return x; }
    if(aEntityCode[0] == 0x2212) { x.Set(KMinusEntity ); return x;}
    if(aEntityCode[0] == 0x2217) { x.Set(KLowastEntity ); return x; }
    if(aEntityCode[0] == 0x221a) { x.Set(KRadicEntity ); return x; }
    if(aEntityCode[0] == 0x221d) { x.Set(KPropEntity ); return x; }
    if(aEntityCode[0] == 0x221e) { x.Set(KInfinEntity ); return x; }
    if(aEntityCode[0] == 0x2220) { x.Set(KAngEntity ); return x; }
    if(aEntityCode[0] == 0x2227) { x.Set(KAndEntity ); return x; }
	if(aEntityCode[0] == 0x2228) { x.Set(KOrEntity ); return x; }
    if(aEntityCode[0] == 0x2229) { x.Set(KCapEntity ); return x; }
    if(aEntityCode[0] == 0x222a) { x.Set(KCupEntity ); return x; }
    if(aEntityCode[0] == 0x222b) { x.Set(KIntEntity ); return x; }
    if(aEntityCode[0] == 0x2234) { x.Set(KThere4Entity ); return x; }
    if(aEntityCode[0] == 0x223c) { x.Set(KsimEntity ); return x; }
    if(aEntityCode[0] == 0x2245) { x.Set(KCongEntity ); return x; }
    if(aEntityCode[0] == 0x2248) { x.Set(KAsympEntity ); return x; }
    if(aEntityCode[0] == 0x2260) { x.Set(KNeEntity ); return x; }
    if(aEntityCode[0] == 0x2261) { x.Set(KEquivEntity ); return x; }
    if(aEntityCode[0] == 0x2264) { x.Set(KLeEntity ); return x; }
    if(aEntityCode[0] == 0x2265) { x.Set(KGeEntity ); return x; }
	if(aEntityCode[0] == 0x2282) { x.Set(KSubEntity ); return x; }
    if(aEntityCode[0] == 0x2283) { x.Set(KSupEntity ); return x; }
    if(aEntityCode[0] == 0x2284) { x.Set(KNsubEntity ); return x; }
    if(aEntityCode[0] == 0x2286) { x.Set(KSubeEntity ); return x; }
    if(aEntityCode[0] == 0x2287) { x.Set(KSupeEntity ); return x; }
    if(aEntityCode[0] == 0x2295) { x.Set(KOplusEntity ); return x; }
    if(aEntityCode[0] == 0x2297) { x.Set(KOtimesEntity ); return x; }
    if(aEntityCode[0] == 0x22a5) { x.Set(KperpEntity ); return x; }
    if(aEntityCode[0] == 0x22c5) { x.Set(KSdotEntity ); return x; }
    if(aEntityCode[0] == 0x2308) { x.Set(KLceilEntity ); return x; }

    if(aEntityCode[0] == 0x2309) { x.Set(KRceilEntity ); return x; }
    if(aEntityCode[0] == 0x230a) { x.Set(KLfloorEntity ); return x; }
	if(aEntityCode[0] == 0x230b) { x.Set(KRfloorEntity ); return x; }
    if(aEntityCode[0] == 0x2329) { x.Set(KlangEntity ); return x; }
    if(aEntityCode[0] == 0x232a) { x.Set(KRangEntity ); return x; }
    
	if(aEntityCode[0] == 0x25ca) { x.Set(KLozEntity ); return x; }
    if(aEntityCode[0] == 0x2660) { x.Set(KSpadesEntity ); return x; }
    if(aEntityCode[0] == 0x2663) { x.Set(KClubsEntity ); return x; }
    if(aEntityCode[0] == 0x2665) { x.Set(KHeartsEntity ); return x; }
    if(aEntityCode[0] == 0x2666) { x.Set(KdiamsEntity ); return x; }

    if(aEntityCode[0] == 0x3c) { x.Set(KLtEntity ); return x; }
    if(aEntityCode[0] == 0x3e) { x.Set(KGtEntity ); return x; }
    if(aEntityCode[0] == 0x26) { x.Set(KAmpEntity ); return x; }
    if(aEntityCode[0] == 0x27) { x.Set(KAposEntity ); return x; }
	if(aEntityCode[0] == 0x22) { x.Set(KQuotEntity ); return x; }
    if(aEntityCode[0] == 0x152) { x.Set(KOEligEntity ); return x; }
    if(aEntityCode[0] == 0x153) { x.Set(KoeligEntity ); return x; }
    if(aEntityCode[0] == 0x160) { x.Set(KScaronEntity ); return x; }
    if(aEntityCode[0] == 0x161) { x.Set(KscaronEntity ); return x; }
    if(aEntityCode[0] == 0x178) { x.Set(KYumlEntity ); return x; }

    if(aEntityCode[0] == 0x2c6) { x.Set(KCircEntity ); return x; }
    if(aEntityCode[0] == 0x2dc) { x.Set(KtildeEntity ); return x; }
    if(aEntityCode[0] == 0x2002) { x.Set(KEnspEntity ); return x; }
    if(aEntityCode[0] == 0x2003) { x.Set(KemspEntity ); return x; }
    if(aEntityCode[0] == 0x2009) { x.Set(KThinspEntity ); return x; }
    if(aEntityCode[0] == 0x200c) { x.Set(KZwnjEntity ); return x; }
	if(aEntityCode[0] == 0x200d) { x.Set(KZwjEntity ); return x; }
    if(aEntityCode[0] == 0x200e) { x.Set(KLrmEntity ); return x;  }
    if(aEntityCode[0] == 0x200f) { x.Set(KRlmEntity ); return x; } 
    if(aEntityCode[0] == 0x2013) { x.Set(KNdashEntity ); return x; }
    if(aEntityCode[0] == 0x2014) { x.Set(KMdashEntity ); return x; }
    if(aEntityCode[0] == 0x2018) { x.Set(KLsquoEntity ); return x; }
    if(aEntityCode[0] == 0x2019) { x.Set(KRsquoEntity ); return x; }
    if(aEntityCode[0] == 0x201a) { x.Set(KsbquoEntity ); return x; }
    if(aEntityCode[0] == 0x201c) { x.Set(KldquoEntity ); return x; }
    if(aEntityCode[0] == 0x201d) { x.Set(KRdquoEntity ); return x; }
    if(aEntityCode[0] == 0x201e) { x.Set(KbdquoEntity ); return x; }
    if(aEntityCode[0] == 0x2020) { x.Set(KdaggerEntity ); return x; }
	if(aEntityCode[0] == 0x2021) { x.Set(KDaggerEntity ); return x; }
    if(aEntityCode[0] == 0x2030) { x.Set(KpermilEntity ); return x; }
    if(aEntityCode[0] == 0x2039) { x.Set(KLsaquoEntity ); return x; }
    if(aEntityCode[0] == 0x203a) { x.Set(KRsaquoEntity ); return x; }
    if(aEntityCode[0] == 0x20ac) { x.Set(KEuroEntity ); return x; }
    
    return aEntityCode;
    }
