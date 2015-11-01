#ifndef _IN_LEXMANAGER_
#define _IN_LEXMANAGER_

//#include	<afx.h>
//#include "EBoardScriptParserDll.h"

#define	MAX_FILESIZE	100000
#define	MAX_BUFFER		1024

// simplified Penn Treebank part-of-speech tags

#define		NN					0x000002	// wordnet NOUN 
#define		VB					0x000004	// wordnet VERB
#define		JJ					0x000008	// wordnet ADJ
#define		RB					0x000010	// wordnet ADV
#define		NNP					0x000100	// proper noun
#define		NNPF				0x000200	// proper noun Female
#define		NNPM				0x000400	// proper noun Male
#define		DT					0x000800	// determiner
#define		INRP				0x001000	// preposition + particle
#define		CC					0x002000	// coordination
#define		PP					0x004000	// personal pronoun
#define		PP$					0x008000	// other pronoun classes
#define		UH					0x010000	// interjection
#define		PU					0x020000	// ponctuation
#define		AUX					0x040000	// auxiliary VERB

// other tags
#define		NNJF				0x080000	// job noun Female
#define		NNJM				0x100000	// job noun Male

//

#define		ISNN( x )	( (x & NN) ? true : false )
#define		ISVB( x )	( (x & VB) ? true : false )
#define		ISAUX( x )	( (x & AUX) ? true : false )
#define		ISJJ( x )	( (x & JJ) ? true : false )
#define		ISRB( x )	( (x & RB) ? true : false )
#define		ISNNP( x )	( (x & NNP) ? true : false )
#define		ISNNPF( x )	( (x & NNPF) ? true : false )
#define		ISNNPM( x )	( (x & NNPM) ? true : false )
#define		ISDT( x )	( (x & DT) ? true : false )
#define		ISINRP( x )	( (x & INRP) ? true : false )
#define		ISCC( x )	( (x & CC) ? true : false )
#define		ISPP( x )	( (x & PP) ? true : false )
#define		ISPP$( x )	( (x & PP$) ? true : false )
#define		ISUH( x )	( (x & UH) ? true : false )
#define		ISPU( x )	( (x & PU) ? true : false )
#define		ISNNJF( x )	( (x & NNJF) ? true : false )
#define		ISNNJM( x )	( (x & NNJM) ? true : false )


typedef struct SLexicon
{
	unsigned int		pos ;
	unsigned int		count ;
	CString*			lexeme ;
}lex ;

class /*EBoardScriptParserDll*/  CLEXManager  
{
public:
	CLEXManager();
	virtual ~CLEXManager();

protected:
	void*			LEXLoad(char*, unsigned ) ;
	void*			LEXCreateLexicon( char*, unsigned int) ;
	unsigned int	LEXFindPos( CString& ) ;

private:
	unsigned int	_m_numLEX ;
	SLexicon**		_m_ptrLEX ;

	unsigned int	_CompString( CString&, CString& ) ;

};

#endif
