// SceneAnalysis.h: interface for the CSceneAnalysis class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_SCENEANALYSIS_H__16DDB6B1_F3E7_4B06_A0A3_C8BBE4C75B14__INCLUDED_)
//#define AFX_SCENEANALYSIS_H__16DDB6B1_F3E7_4B06_A0A3_C8BBE4C75B14__INCLUDED_

//#if _MSC_VER > 1000
//#pragma once
//#endif // _MSC_VER > 1000

//#include <afxcoll.h>
//#include <afxwin.h>
//#include "EBoardScriptParserDll.h"

#define SA_MAX_FILESIZE				100000
#define	SA_MAX_BUFFER				1024

#define	SC_DICOENTRY_STYLES			"STYLES"
#define	SC_DICOENTRY_SCENES			"SCENES"
#define	SC_DICOENTRY_SITUATIONS		"SITUATIONS"
#define	SC_DICOENTRY_PERIODS		"PERIODES"

#define SC_STYLEENTRY_TITLE			"$TITRE"
#define SC_STYLEENTRY_ACTION		"$ACTION"
#define SC_STYLEENTRY_CHARACTER		"$PERSONNAGE"
#define SC_STYLEENTRY_PARENTHESIS	"$PARENTHESE"
#define SC_STYLEENTRY_DIALOG		"$DIALOGUE"
#define SC_STYLEENTRY_TRANSITION	"$TRANSITION"
#define SC_STYLEENTRY_ANIMATION		"$ANIMATION"

#define SC_SCENEENTRY_NUM			"$NUMBER"

#define SC_SITUATIONENTRY_EXT		"$EXT"
#define SC_SITUATIONENTRY_INT		"$INT"
#define SC_SITUATIONENTRY_IE		"$IE"
#define SC_SITUATIONENTRY_EI		"$EI"

#define SC_PERIODENTRY_DAY			"$JOUR"
#define SC_PERIODENTRY_NIGHT		"$NUIT"
#define SC_PERIODENTRY_AFTERNOON	"$APRES-MIDI"
#define SC_PERIODENTRY_MORNING		"$MATIN"
#define SC_PERIODENTRY_TWILIGHT		"$CREPUSCULE"
#define SC_PERIODENTRY_EVENING		"$SOIR"
#define SC_PERIODENTRY_LATER		"$PLUS-TARD"
#define SC_PERIODENTRY_MOMENT_LATER	"$MOMENT-PLUS-TARD"

#define CH_STANDARD					0x00000000
#define CH_BOLD						0x00000001
#define CH_ITALIC					0x00000010
#define CH_UNDERLINE				0x00000100

typedef enum { PR_LEFT, PR_CENTER, PR_RIGHT, PR_JUSTIFY } SAPARTYPE ;

class CScriptShape
{
public:
	CScriptShape() 
	{
		mFont = "Times New Roman" ;
		mCaps = false ;
		mChar = CH_STANDARD ;
		mAlign = PR_LEFT ;
		mRightIndent = 0 ;
		mLeftIndent = 0 ;
	} ;
	virtual ~CScriptShape() {} ;

	CString			mFont ;
	bool			mCaps ;
	WORD			mChar ;
	SAPARTYPE		mAlign ;
	UINT			mRightIndent ;
	UINT			mLeftIndent ;

};

class CDico  
{
public:
	CDico() 
	{
		mKey = "" ;
		mNext = NULL ; 
	} ;
	virtual ~CDico() {} ;
	CString			mKey ;
	CStringArray	mString ;
	CScriptShape	mStyle ;
	CDico*			mNext ;
};

class /*EBoardScriptParserDll*/ CSceneAnalysis  
{
public:
	CSceneAnalysis();
	virtual ~CSceneAnalysis();

	bool	Load(const char*) ;
	bool	Is(const char*,const char*) ;

	CString	Find( CString, CString&, int&, int& ) ;
	CDico*	FindDico( CString ) ;
	CDico*	FindKey( CString ) ;
	CDico*	GetPtr(const char*) ;

	CString	GetStyleName(const char*) ;
	int		GetStyleId(CString) ;
	char*	GetStyle(const char*, int*) ;

private:
	CMapStringToPtr mDico ;

	CDico*	_AddDico( CString, CString) ;

};

//#endif // !defined(AFX_SCENEANALYSIS_H__16DDB6B1_F3E7_4B06_A0A3_C8BBE4C75B14__INCLUDED_)
