//////////////////////////////////////////////////////////////////////
// RTFparser.cpp: implementation of the CRTFParser class.
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include "RTFparser.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// RTF parser tables construction
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Property descriptions
PROP rgprop [ipropMax] = {
	actnByte, propChp, offsetof(CHP, fBold), // ipropBold
	actnByte, propChp, offsetof(CHP, fItalic), // ipropItalic
	actnByte, propChp, offsetof(CHP, fUnderline), // ipropUnderline
	actnWord, propPap, offsetof(PAP, xaLeft), // ipropLeftInd
	actnWord, propPap, offsetof(PAP, xaRight), // ipropRightInd
	actnWord, propPap, offsetof(PAP, xaFirst), // ipropFirstInd
	actnWord, propSep, offsetof(SEP, cCols), // ipropCols
	actnWord, propSep, offsetof(SEP, xaPgn), // ipropPgnX
	actnWord, propSep, offsetof(SEP, yaPgn), // ipropPgnY
	actnWord, propDop, offsetof(DOP, xaPage), // ipropXaPage
	actnWord, propDop, offsetof(DOP, yaPage), // ipropYaPage
	actnWord, propDop, offsetof(DOP, xaLeft), // ipropXaLeft
	actnWord, propDop, offsetof(DOP, xaRight), // ipropXaRight
	actnWord, propDop, offsetof(DOP, yaTop), // ipropYaTop
	actnWord, propDop, offsetof(DOP, yaBottom), // ipropYaBottom
	actnWord, propDop, offsetof(DOP, pgnStart), // ipropPgnStart
	actnByte, propSep, offsetof(SEP, sbk), // ipropSbk
	actnByte, propSep, offsetof(SEP, pgnFormat), // ipropPgnFormat
	actnByte, propDop, offsetof(DOP, fFacingp), // ipropFacingp
	actnByte, propDop, offsetof(DOP, fLandscape), // ipropLandscape
	actnByte, propPap, offsetof(PAP, just), // ipropJust
	actnSpec, propPap, 0, // ipropPard
	actnSpec, propChp, 0, // ipropPlain
	actnSpec, propSep, 0, // ipropSectd
};

//////////////////////////////////////////////////////////////////////
// Keyword descriptions
SYM rgsymRtf[] = {
	// keyword dflt fPassDflt kwd idx
	"b", 1, fFalse, kwdProp, ipropBold,
	"u", 1, fFalse, kwdProp, ipropUnderline,
	"i", 1, fFalse, kwdProp, ipropItalic,
	"li", 0, fFalse, kwdProp, ipropLeftInd,
	"ri", 0, fFalse, kwdProp, ipropRightInd,
	"fi", 0, fFalse, kwdProp, ipropFirstInd,
	"cols", 1, fFalse, kwdProp, ipropCols,
	"sbknone", sbkNon, fTrue, kwdProp, ipropSbk,
	"sbkcol", sbkCol, fTrue, kwdProp, ipropSbk,
	"sbkeven", sbkEvn, fTrue, kwdProp, ipropSbk,
	"sbkodd", sbkOdd, fTrue, kwdProp, ipropSbk,
	"sbkpage", sbkPg, fTrue, kwdProp, ipropSbk,
	"pgnx", 0, fFalse, kwdProp, ipropPgnX,
	"pgny", 0, fFalse, kwdProp, ipropPgnY,
	"pgndec", pgDec, fTrue, kwdProp, ipropPgnFormat,
	"pgnucrm", pgURom, fTrue, kwdProp, ipropPgnFormat,
	"pgnlcrm", pgLRom, fTrue, kwdProp, ipropPgnFormat,
	"pgnucltr", pgULtr, fTrue, kwdProp, ipropPgnFormat,
	"pgnlcltr", pgLLtr, fTrue, kwdProp, ipropPgnFormat,
	"qc", justC, fTrue, kwdProp, ipropJust,
	"ql", justL, fTrue, kwdProp, ipropJust,
	"qr", justR, fTrue, kwdProp, ipropJust,
	"qj", justF, fTrue, kwdProp, ipropJust,
	"paperw", 12240, fFalse, kwdProp, ipropXaPage,
	"paperh", 15480, fFalse, kwdProp, ipropYaPage,
	"margl", 1800, fFalse, kwdProp, ipropXaLeft,
	"margr", 1800, fFalse, kwdProp, ipropXaRight,
	"margt", 1440, fFalse, kwdProp, ipropYaTop,
	"margb", 1440, fFalse, kwdProp, ipropYaBottom,
	"pgnstart", 1, fTrue, kwdProp, ipropPgnStart,
	"facingp", 1, fTrue, kwdProp, ipropFacingp,
	"landscape",1, fTrue, kwdProp, ipropLandscape,
	"par", 0, fFalse, kwdChar, 0x0a,
	"\0x0a", 0, fFalse, kwdChar, 0x0a,
	"\0x0d", 0, fFalse, kwdChar, 0x0a,
	"tab", 0, fFalse, kwdChar, 0x09,
	"ldblquote",0, fFalse, kwdChar, '"',
	"rdblquote",0, fFalse, kwdChar, '"',
	"bin", 0, fFalse, kwdSpec, ipfnBin,
	"*", 0, fFalse, kwdSpec, ipfnSkipDest,
	"'", 0, fFalse, kwdSpec, ipfnHex,
	"author", 0, fFalse, kwdDest, idestSkip,
	"buptim", 0, fFalse, kwdDest, idestSkip,
	"colortbl", 0, fFalse, kwdDest, idestSkip,
	"comment", 0, fFalse, kwdDest, idestSkip,
	"creatim", 0, fFalse, kwdDest, idestSkip,
	"doccomm", 0, fFalse, kwdDest, idestSkip,
	"fonttbl", 0, fFalse, kwdDest, idestSkip,
	"footer", 0, fFalse, kwdDest, idestSkip,
	"footerf", 0, fFalse, kwdDest, idestSkip,
	"footerl", 0, fFalse, kwdDest, idestSkip,
	"footerr", 0, fFalse, kwdDest, idestSkip,
	"footnote", 0, fFalse, kwdDest, idestSkip,
	"ftncn", 0, fFalse, kwdDest, idestSkip,
	"ftnsep", 0, fFalse, kwdDest, idestSkip,
	"ftnsepc", 0, fFalse, kwdDest, idestSkip,
	"header", 0, fFalse, kwdDest, idestSkip,
	"headerf", 0, fFalse, kwdDest, idestSkip,
	"headerl", 0, fFalse, kwdDest, idestSkip,
	"headerr", 0, fFalse, kwdDest, idestSkip,
	"info", 0, fFalse, kwdDest, idestSkip,
	"keywords", 0, fFalse, kwdDest, idestSkip,
	"operator", 0, fFalse, kwdDest, idestSkip,
	"pict", 0, fFalse, kwdDest, idestSkip,
	"printim", 0, fFalse, kwdDest, idestSkip,
	"private1", 0, fFalse, kwdDest, idestSkip,
	"revtim", 0, fFalse, kwdDest, idestSkip,
	"rxe", 0, fFalse, kwdDest, idestSkip,
	"stylesheet", 0, fFalse, kwdDest, idestSkip,
	"subject", 0, fFalse, kwdDest, idestSkip,
	"tc", 0, fFalse, kwdDest, idestSkip,
	"title", 0, fFalse, kwdDest, idestSkip,
	"txe", 0, fFalse, kwdDest, idestSkip,
	"xe", 0, fFalse, kwdDest, idestSkip,
	"{", 0, fFalse, kwdChar, '{',
	"}", 0, fFalse, kwdChar, '}',
	"\\", 0, fFalse, kwdChar, '\\'
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRTFParser::CRTFParser()
{
	m_pStyleList = NULL ;
	m_pObjectList = NULL ;

	m_StyleState = fFalse ;
	m_ParStateBegin = fFalse ;
	m_pStyleCurrent = NULL ;
	m_pObjectCurrent = NULL ;

	m_NumStyle = 0 ;
	m_NumObject = 0 ;

	cGroup = 0 ;
	fSkipDestIfUnk = fFalse ;
	cbBin = 0 ;
	lParam = 0 ;
	rds = (RDS)0 ;
	ris = (RIS)0 ;
	psave = NULL ;;
}

CRTFParser::~CRTFParser()
{
	_DELETE_STYLELIST() ;
	_DELETE_OBJECTLIST() ;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecRtfParse
//
// Step 1:
// Isolate RTF keywords and send them to ecParseRtfKeyword;
// Push and pop state at the start and end of RTF groups;
// Send text to ecParseChar for further processing.
//
int CRTFParser::ecRtfParse(FILE *fp)
{
	int ch;
	int ec;
	int cNibble = 2;
	int b = 0;

	m_StyleState = fFalse ;
	m_ParStateBegin = fFalse ;
	m_pStyleCurrent = NULL ;
	m_pObjectCurrent = NULL ;

	_DELETE_STYLELIST() ;
	_DELETE_OBJECTLIST() ;

	while ((ch = getc(fp)) != EOF)
	{
		if (cGroup < 0)
			return ecStackUnderflow;
		if (ris == risBin) // if we're parsing binary data, handle it directly
		{
			if ((ec = ecParseChar(ch)) != ecOK)
			return ec;
		}
		else
		{
			switch (ch)
			{
				case '{':
					if ((ec = ecPushRtfState()) != ecOK)
						return ec;
					break;
				case '}':
					if ((ec = ecPopRtfState()) != ecOK)
						return ec;
					break;
				case '\\':
					if ((ec = ecParseRtfKeyword(fp)) != ecOK)
						return ec;
					break;
				case 0x0d:
				case 0x0a: // cr and lf are noise characters...
					break;
				default:
					if (ris == risNorm)
					{
						if ((ec = ecParseChar(ch)) != ecOK)
							return ec;
					}
					else
					{ // parsing hex data
						if (ris != risHex)
							return ecAssertion;
						b = b << 4;
						if (isdigit(ch))
							b += (char) ch - '0';
						else
						{
							if (islower(ch))
							{
								if (ch < 'a' || ch > 'f')
									return ecInvalidHex;
								b += (char) ch - 'a';
							}
							else
							{
								if (ch < 'A' || ch > 'F')
									return ecInvalidHex;
								b += (char) ch - 'A';
							}
						}
						cNibble--;
						if (!cNibble)
						{
							if ((ec = ecParseChar(b)) != ecOK)
								return ec;
							cNibble = 2;
							b = 0;
							ris = risNorm;
						}
					} // end else (ris != risNorm)
					break;
			} // switch
		} // else (ris != risBin)
	} // while
	if (cGroup < 0)
		return ecStackUnderflow;
	if (cGroup > 0)
		return ecUnmatchedBrace;
	return ecOK;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecPushRtfState
//
// Save relevant info on a linked list of SAVE structures.
//
int CRTFParser::ecPushRtfState(void)
{
	SAVE *psaveNew = (SAVE*)malloc(sizeof(SAVE));
	if (!psaveNew)
		return ecStackOverflow;
	psaveNew -> pNext = psave;
	psaveNew -> chp = chp;
	psaveNew -> pap = pap;
	psaveNew -> sep = sep;
	psaveNew -> dop = dop;
	psaveNew -> rds = rds;
	psaveNew -> ris = ris;
	ris = risNorm;
	psave = psaveNew;
	cGroup++;
	return ecOK;
}
//
// %%Function: ecPopRtfState
//
// If we're ending a destination (that is, the destination is changing),
// call ecEndGroupAction.
// Always restore relevant info from the top of the SAVE list.
//
int CRTFParser::ecPopRtfState(void)
{
	SAVE *psaveOld;
	int ec;
	if (!psave)
		return ecStackUnderflow;
	if (rds != psave->rds)
	{
		if ((ec = ecEndGroupAction(rds)) != ecOK)
			return ec;
	}
	chp = psave->chp;
	pap = psave->pap;
	sep = psave->sep;
	dop = psave->dop;
	rds = psave->rds;
	ris = psave->ris;
	psaveOld = psave;
	psave = psave->pNext;
	cGroup--;
	free(psaveOld);
	return ecOK;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecParseRtfKeyword
//
// Step 2:
// get a control word (and its associated value) and
// call ecTranslateKeyword to dispatch the control.
//
int CRTFParser::ecParseRtfKeyword(FILE *fp)
{
	int ch;
	char fParam = fFalse;
	char fNeg = fFalse;
	int param = 0;
	char *pch;
	char szKeyword[30];
	char szParameter[20];
	szKeyword[0] = '\0';
	szParameter[0] = '\0';
	if ((ch = getc(fp)) == EOF)
		return ecEndOfFile;
	if (!isalpha(ch)) // a control symbol; no delimiter.
	{
		szKeyword[0] = (char) ch;
		szKeyword[1] = '\0';
		return ecTranslateKeyword(szKeyword, 0, fParam);
	}

	for (pch = szKeyword; isalpha(ch); ch = getc(fp))
		*pch++ = (char) ch;

	*pch = '\0';

	if (ch == '-')
	{
		fNeg = fTrue;
		if ((ch = getc(fp)) == EOF)
			return ecEndOfFile;
	}
	if (isdigit(ch))
	{
		fParam = fTrue; // a digit after the control means we have a parameter
		for (pch = szParameter; isdigit(ch); ch = getc(fp))
		*pch++ = (char) ch;
		*pch = '\0';
		param = atoi(szParameter);
		if (fNeg)
			param = -param;
		lParam = atol(szParameter);
		if (fNeg)
			param = -param;
	}

	if (ch != ' ')
	ungetc(ch, fp);

	char szStyle[256] ;	strcpy( szStyle, szKeyword ) ;

	/////////////////////////////////////////////
	// traitement du style

	if ( strcmp(szKeyword, "stylesheet") == 0 )
	{
		m_StyleState = fTrue ;
	}
	else
	// paramétrage des styles
	// - index
	if ( m_StyleState == fTrue && strcmp(szKeyword, "s") == 0 )
	{
		m_pStyleCurrent = ecCreateStyle() ;
		if ( m_pStyleCurrent )
		{
			strcat( szStyle, szParameter ) ;
			m_pStyleCurrent->szStyleDef = _strdup( szStyle ) ;
		}
	}

	///////////////////////////////////////////////////
	// traitement des paragraphes et des tags spéciaux

	// début de paragraphe
	if ( strcmp(szKeyword, "pard") == 0 )
	{
		m_ParStateBegin = fTrue ;
		m_pStyleCurrent = NULL ;
		if ( m_pObjectCurrent == NULL )
			m_pObjectCurrent = ecCreateObject() ;
	}
	else
	// fin de paragraphe
	if ( strcmp(szKeyword, "par") == 0 )
	{
		if ( m_pObjectCurrent->szObjectStyle == NULL && m_pStyleCurrent )
			m_pObjectCurrent->szObjectStyle = m_pStyleCurrent ;

		m_pObjectCurrent = ecCreateObject() ;
		m_ParStateBegin = fFalse ;
	}
	else
	// définition d'un style
	if ( m_ParStateBegin == fTrue && strcmp(szKeyword, "s") == 0 )
	{
		strcat( szStyle, szParameter ) ;
		m_pStyleCurrent = ecFindStyle( szStyle ) ;
	}
	else
	// 25/07/2004, suite à un bug lié à l'inclusion du tag \line dans un paragraphe
	// et qui est considéré par RichEdit comme un paragraphe à part (?!?)
	// je fais la modif suivante : 
	if ( m_ParStateBegin == fTrue && strcmp(szKeyword, "line") == 0 )
	{
		// clore l'objet courant
		if ( m_pObjectCurrent->szObjectStyle == NULL && m_pStyleCurrent )
			m_pObjectCurrent->szObjectStyle = m_pStyleCurrent ;

		// créer un nouvel objet
		m_pObjectCurrent = ecCreateObject() ;
	}

	/////////////////////////////////////////////

	return ecTranslateKeyword(szKeyword, param, fParam);
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecCreateStyle
//
//
SRTFStyle* CRTFParser::ecFindStyle(char* pStyle)
{
	SRTFStyle* ptr = m_pStyleList ;
	while( ptr )
	{
		if ( strcmp(ptr->szStyleDef, pStyle ) == 0 ) 
			return ptr ;
		ptr = ptr->pNext ;
	};
	return NULL ;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecCreateStyle
//
//
SRTFObject* CRTFParser::ecCreateObject()
{
	SRTFObject* ptr = NULL;
	if ( !m_pObjectList )	ptr = m_pObjectList = new SRTFObject() ;
	else
	{
		ptr = m_pObjectList; 
		while (ptr->pNext)
		{
			ptr = ptr->pNext;
		};
		ptr = ptr->pNext = new SRTFObject() ;
	}
	if ( !ptr )
		return NULL;
	ptr->szObjectStyle = NULL ;
	ptr->szObjectInfo = NULL ;
	ptr->pNext = NULL ;

	ptr->szCpStart1 = ptr->szCpEnd1 = ptr->szCpStart2 = ptr->szCpEnd2 = -1 ;

	m_NumObject++;
	return ptr ;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecCreateStyle
//
//
SRTFStyle* CRTFParser::ecCreateStyle()
{
	SRTFStyle* ptr = NULL;
	if ( !m_pStyleList )	ptr = m_pStyleList = new SRTFStyle() ;
	else
	{
		ptr = m_pStyleList;	
		while (ptr->pNext) 
		{
			ptr = ptr->pNext;	
		} ;
		ptr = ptr->pNext = new SRTFStyle() ;
	}
	if ( !ptr )
		return NULL;
	ptr->szStyleDef = NULL ;
	ptr->szStyleName = NULL ;
	ptr->pNext = NULL ;
	m_NumStyle++;
	return ptr ;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecParseChar
//
// Route the character to the appropriate destination stream.
//
int CRTFParser::ecParseChar(int ch)
{
	if (ris == risBin && --cbBin <= 0)
		ris = risNorm;
	switch (rds)
	{
		case rdsSkip:
			/////////////////////////////////////////////
			// traitement du style
			// -> le nom m'intéresse pour la reconnaissance
			if ( m_StyleState == fTrue && m_pStyleCurrent!=NULL )
			{
				char szBuff[2] ;
				szBuff[0] = (char)ch ;
				szBuff[1] = 0 ;
				if ( (char)ch!=';' )
				{
					if ( m_pStyleCurrent->szStyleName==NULL)
					{
						m_pStyleCurrent->szStyleName = _strdup( szBuff ) ;
					}
					else
					{
						char szName[256] ;
						strcpy( szName, m_pStyleCurrent->szStyleName ) ;
						free( m_pStyleCurrent->szStyleName ) ;
						strcat( szName, szBuff ) ;
						m_pStyleCurrent->szStyleName = _strdup( szName ) ;
					}
				}
			}
			/////////////////////////////////////////////
			return ecOK;
		case rdsNorm:
			// Output a character. Properties are valid at this point.
			return ecPrintChar(ch);
		default:
			// handle other destinations....
			return ecOK;
	}
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecPrintChar
// Send a character to the output.
//
int CRTFParser::ecPrintChar(int ch)
{
	/////////////////////////////////////////////
	// traitement du texte
	if ( m_pObjectCurrent!=NULL )
	{	
		char szBuff[2] ;
		szBuff[0] = (char)ch ;
		szBuff[1] = 0 ;
		if ( m_pObjectCurrent->szObjectInfo==NULL)
		{
			m_pObjectCurrent->szObjectInfo = _strdup( szBuff ) ;
		}
		else
		{
			char* szInfo = new char[ strlen( m_pObjectCurrent->szObjectInfo ) + strlen( szBuff ) + 1 ] ;
			strcpy( szInfo, m_pObjectCurrent->szObjectInfo ) ;
			free( m_pObjectCurrent->szObjectInfo ) ;
			strcat( szInfo, szBuff ) ;
			m_pObjectCurrent->szObjectInfo = _strdup( szInfo ) ;
			delete [] szInfo ;
		}
	}
//	putchar(ch);
	return ecOK;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecApplyPropChange
//
// Set the property identified by _iprop_ to the value _val_.
//
//
int CRTFParser::ecApplyPropChange(IPROP iprop, int val)
{
	char *pb = NULL;

	if (rds == rdsSkip) // If we're skipping text,
		return ecOK; // don't do anything.

	switch (rgprop[iprop].prop)
	{
		case propDop:
			pb = (char *)&dop;
			break;
		case propSep:
			pb = (char *)&sep;
			break;
		case propPap:
			pb = (char *)&pap;
			break;
		case propChp:
			pb = (char *)&chp;
			break;
		default:
			if (rgprop[iprop].actn != actnSpec)
			return ecBadTable;
			break;
	}

	switch (rgprop[iprop].actn)
	{
		case actnByte:
			if (pb != NULL)
			{
				pb[rgprop[iprop].offset] = (unsigned char) val;
			}
			break;
		case actnWord:
			if (pb != NULL)
			{
				(*(int *) (pb+rgprop[iprop].offset)) = val;
			}
			break;
		case actnSpec:
			return ecParseSpecialProperty(iprop, val);
			break;
		default:
			return ecBadTable;
	}
	return ecOK;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecParseSpecialProperty
//
// Set a property that requires code to evaluate.
//
int CRTFParser::ecParseSpecialProperty(IPROP iprop, int val)
{
	switch (iprop)
	{
		case ipropPard:
			memset(&pap, 0, sizeof(pap));
			return ecOK;
		case ipropPlain:
			memset(&chp, 0, sizeof(chp));
			return ecOK;
		case ipropSectd:
			memset(&sep, 0, sizeof(sep));
			return ecOK;
		default:
			return ecBadTable;
	}
	return ecBadTable;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecTranslateKeyword.
//
// Step 3.
// Search rgsymRtf for szKeyword and evaluate it appropriately.
//
// Inputs:
// szKeyword: The RTF control to evaluate.
// param: The parameter of the RTF control.
// fParam: fTrue if the control had a parameter; (that is, if param is valid)
// fFalse if it did not.
//
int CRTFParser::ecTranslateKeyword(char *szKeyword, int param, fBool fParam)
{
	int isym;
	int isymMax = sizeof(rgsymRtf) / sizeof(SYM);

	// search for szKeyword in rgsymRtf
	for (isym = 0; isym < isymMax; isym++)
	{
		if (strcmp(szKeyword, rgsymRtf[isym].szKeyword) == 0)	break;
	}
	if (isym == isymMax) // control word not found
	{
		if (fSkipDestIfUnk) // if this is a new destination
			rds = rdsSkip; // skip the destination
		// else just discard it
		fSkipDestIfUnk = fFalse;
		return ecOK;
	}
	// found it! use kwd and idx to determine what to do with it.
	fSkipDestIfUnk = fFalse;
	switch (rgsymRtf[isym].kwd)
	{
		case kwdProp:
			if (rgsymRtf[isym].fPassDflt || !fParam)
			param = rgsymRtf[isym].dflt;
			return ecApplyPropChange((IPROP)rgsymRtf[isym].idx, param);
		case kwdChar:
			return ecParseChar(rgsymRtf[isym].idx);
		case kwdDest:
			return ecChangeDest((IDEST)rgsymRtf[isym].idx);
		case kwdSpec:
			return ecParseSpecialKeyword((IPFN)rgsymRtf[isym].idx);
		default:
			return ecBadTable;
	}
	return ecBadTable;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecChangeDest
//
// Change to the destination specified by idest.
// There's usually more to do here than this...
//
int CRTFParser::ecChangeDest(IDEST idest)
{
	if (rds == rdsSkip) // if we're skipping text,
		return ecOK; // don't do anything
	switch (idest)
	{
		default:
			rds = rdsSkip; // when in doubt, skip it...
			break;
	}
	return ecOK;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecEndGroupAction
//
// The destination specified by rds is coming to a close.
// If there's any cleanup that needs to be done, do it now.
//
int CRTFParser::ecEndGroupAction(RDS rds)
{
	m_StyleState = fFalse ;
	return ecOK;
}

//////////////////////////////////////////////////////////////////////
// %%Function: ecParseSpecialKeyword
//
// Evaluate an RTF control that needs special processing.
//
int CRTFParser::ecParseSpecialKeyword(IPFN ipfn)
{
	if (rds == rdsSkip && ipfn != ipfnBin) // if we're skipping, and it's not
		return ecOK; // the \bin keyword, ignore it.
	switch (ipfn)
	{
		case ipfnBin:
			ris = risBin;
			cbBin = lParam;
			break;
		case ipfnSkipDest:
			fSkipDestIfUnk = fTrue;
			break;
		case ipfnHex:
			ris = risHex;
			break;
		default:
			return ecBadTable;
	}
	return ecOK;
}
