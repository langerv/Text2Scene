// FDparser.h: interface for the CFDparser class.
//
//////////////////////////////////////////////////////////////////////
/*
#if !defined(AFX_FDPARSER_H__6E4BF6D0_491B_4FD3_906E_ED41DEF6CDB6__INCLUDED_)
#define AFX_FDPARSER_H__6E4BF6D0_491B_4FD3_906E_ED41DEF6CDB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
*/
//#include "EBoardScriptParserDll.h"
#include "SceneAnalysis.h"
#include "RTFparser.h"
#include "LEXmanager.h"
//#include "EBDoc.h"

#define	SC_ACTORSTAGE_RANGE			5
#define	SC_ACTORSTAGE_RANGE2		SC_ACTORSTAGE_RANGE*SC_ACTORSTAGE_RANGE
#define	SC_ANTENUM_STRING			"SC "
#define	SC_ANTENUM_STRING_SHORT		"SC"
#define	SC_NUMNAME_SEPARATOR		": "
#define	SC_NUMNAME_SEPARATOR_SHORT	":"
#define	SC_NUMNAME_SEPARATOR_CHUNK	"."
#define	SC_MAX_NUM_BUFFER			5
#define	SC_MAX_NUM_CHUNK			10
#define	SC_SCENE_NOTITLE			"SANS TITRE"
#define	SC_GENDER_FEMALE			"FEMELLE"
#define	SC_GENDER_MALE				"MALE"
#define	SC_INIT_FILE				"../_Data/Lexicon/scriptview.ini"
#define SC_LEXICON_FILE0			"../_Data/Lexicon/uk/propernoun_male.txt"
#define SC_LEXICON_FILE1			"../_Data/Lexicon/uk/propernoun_female.txt"
#define SC_LEXICON_FILE2			"../_Data/Lexicon/fr/metier_male.txt"
#define SC_LEXICON_FILE3			"../_Data/Lexicon/fr/metier_femelle.txt"

#include <iostream>
#include <string>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CFDobject class

class CFDobject
{
public:
	CFDobject() 
	{
		m_Name = "" ;
		m_pNext = NULL ; 
	} ;
	~CFDobject() {} ;

	CFDobject*		m_pNext ;
//	CString			m_Name ;
	string m_Name;

	// crop word
//	void	Crop(CString& s)
	void	Crop(string& s)
	{
//		CString buff = s ;
		string buff = s ;
		LPCTSTR ptr = buff ;
		while( s != "" && !isalpha(*ptr++) )
		{
				s = s.Mid(1, s.GetLength());
		}
		ptr = buff ;
		ptr += buff.GetLength() - 1 ;
		while( s!="" && !isalpha(*ptr--) )
		{
				s = s.Left(s.GetLength()-1);
		}
	};

};

/////////////////////////////////////////////////////////////////////////////
// CFDscene class

class CFDscene : public CFDobject
{
public:
	CFDscene() 
	{
		m_Title = "" ;
		m_Situation = "" ;
		m_Period = ""  ;
		m_SitCpStart = m_SitCpEnd = m_PerCpStart = m_PerCpEnd = -1 ;
	} ;

	~CFDscene() {} ;
//	CString		m_Title ;
//	CString		m_Situation ;
//	CString		m_Period ;
	string		m_Title ;
	string		m_Situation ;
	string		m_Period ;
	int			m_SitCpStart, m_SitCpEnd ;
	int			m_PerCpStart, m_PerCpEnd ;
} ;

/////////////////////////////////////////////////////////////////////////////
// CFDactor class

class CFDactor ;

typedef struct SFDreply
{
//	CString			rep ;
	string			rep ;
	CFDscene*		pScene ;
	CFDactor*		pSpeaker ;
	SFDreply*		pCoPrec ;
	SFDreply*		pCoSuiv ;
//	CEBDialogLine*	pDialogLine ;
	SFDreply*		pNext ;
} rep;

typedef struct SFDdialog_group
{
	CFDscene*			pScene ;
	SFDreply*			pReply ;
	SFDdialog_group*	pNext ;
} dg ;

class CFDactor : public CFDobject
{
public:
	CFDactor() 	
	{ 
		m_Sexe = "" ;
		m_CpStart = m_CpEnd = -1 ;
		m_Dialog = NULL ;
	} ;
	CFDactor( char*) ;
	~CFDactor() {} ;

//	CString				m_Sexe ;
	string				m_Sexe ;
	SFDdialog_group*	m_Dialog ;

	int					m_CpStart, m_CpEnd ;

	SFDreply*	AddReply( CFDscene*, char* ) ;

private:
	SFDreply*	_AddReply(SFDdialog_group*, char*) ;

} ;

/////////////////////////////////////////////////////////////////////////////
// CFDparser class
class /*EBoardScriptParserDll*/ CFDparser  : public CSceneAnalysis, protected CRTFparser , protected CLEXManager
{

public:
	CFDparser();
	virtual ~CFDparser();

// import 
public:
	bool		Open(char*) ;
//	bool		Import( CEBDoc* ) ;
	void		Close() ;

private:
//	CEBIdCard*	_FindIdCard(CEBDoc*, CFDactor*) ;
	CFDactor*	_CreateActor( char* ) ;
	CFDscene*	_CreateScene( char* ) ;

// analyse scene
public:
//	CEBOScene*	AnalyseScene(CEBDoc*, CString) ;
//	CEBOScene*	IsSceneExist(CEBDoc*, CString) ;
//	bool		IsSceneIdExist(CEBDoc*, CString ) ;

// analyse actor
public:
//	CEBOChar*	AnalyseActor(CEBDoc*, CEBOScene*, CString) ;
//	CEBOChar*	IsActorInScene(CEBDoc*, CEBOScene*, CString ) ;

private:
//	CEBOScene*	_GetInsertAfterScene(CEBDoc*, CString) ;
//	CEBOScene*	_CreateDocScene( CEBDoc*, CFDscene* ) ;
//	void		_CreateDocIdCards( CEBDoc* ) ;
//	void		_CreateDocActors( CEBDoc*, CFDscene*, CEBOScene* ) ;
	void		_Parse(LPCTSTR, CFDscene*, int numScene = -1 ) ;
	void		_Parse(LPCTSTR, CFDactor*) ;

// get objects
public:
	unsigned	GetActorNum()	{ return _m_NumActor ; } ;
	unsigned	GetSceneNum()	{ return _m_NumScene ; } ;
	unsigned	GetObjectNum()	{ return _m_NumObject ; } ;
	CFDactor*	GetActor()		{ return _m_pActor ; } ;
	CFDscene*	GetScene()		{ return _m_pScene ; } ;
	SFDreply*	GetDialog()		{ return _m_pDialog ; } ;
	SRTFStyle*	GetStyles()		{ return m_pStyleList ; } ;
	SRTFObject*	GetObjects()	{ return m_pObjectList ; } ;

// numérotation
public:
	void		IncreaseId(CString&, int ) ;
	void		FindInBetweenId(CString, CString, CString&) ;
	void		SetSceneId(CString&, CString Id = "1" ) ;
	void		SetSceneNum(CString&, int count = 1 ) ;
	CString		GetSceneId(CString) ;
	int			GetSceneIdShift(CString) ;
	void		DeleteSceneId(CString&) ;

private:
	void		_IncreaseId(CString&) ;
	CString		_GetChunkId(CString, int ) ;
	int			_CompareId( CString, CString ) ;

// données membres
public:
	int			m_SitCpStart, m_SitCpEnd ;
	int			m_PerCpStart, m_PerCpEnd ;

private:
	CFDactor*	_m_pActor ;
	CFDscene*	_m_pScene ;
	SFDreply*	_m_pDialog ;

	unsigned	_m_NumActor ;
	unsigned	_m_NumScene ;
	unsigned	_m_NumObject ;

	inline	_DELETE_ACTORS()
	{
		CFDactor* ptr = NULL ;
		while(_m_pActor) 
		{ 
			// delete dialog groups
			SFDdialog_group* pD = _m_pActor->m_Dialog ;
			while( pD )
			{
				// delete replies
				SFDreply* pR = pD->pReply ;
				while( pR )
				{
					SFDreply* r = pR ;
					pR = pR->pNext ;
					delete r ;
				};
				SFDdialog_group* d = pD ;
				pD = pD->pNext ;
				delete d ;
			};
			// then delete the actor
			ptr = (CFDactor*)_m_pActor->m_pNext ;
			delete _m_pActor ;		
			_m_pActor = (CFDactor*)ptr ; 
		};
		_m_NumActor = 0 ;
	};

	inline	_DELETE_SCENES()
	{
		CFDscene* ptr = NULL ;
		while(_m_pScene) {	ptr = (CFDscene*)_m_pScene->m_pNext ;	delete _m_pScene ;		_m_pScene = (CFDscene*)ptr ; };
		_m_NumScene = 0 ;
	};

};

//#endif // !defined(AFX_FDPARSER_H__6E4BF6D0_491B_4FD3_906E_ED41DEF6CDB6__INCLUDED_)
