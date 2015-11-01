#pragma once
#include <string>
#include <vector>
#include <map>
#include "Utils.h"
using namespace std;

namespace Text2Scene
{
	enum Pos
	{
		UNDEFINED = 0,
		NN	 = 1,	// wordnet NOUN 
		VB,			// wordnet VERB
		JJ,			// wordnet ADJ
		RB,			// wordnet ADV
		NNP,		// proper noun
		NNPF = 6,	// proper noun Female
		NNPM = 7,	// proper noun Male
		DT,			// determiner
		INRP,		// preposition + particle
		CC,			// coordination
		PP,			// personal pronoun
		PP$,		// other pronoun classes
		UH,			// interjection
		PU,			// ponctuation
		AUX,		// auxiliary VERB
		NNJF = 16,	// job noun Female
		NNJM = 17	// job noun Male
	};

	typedef map<Pos, vector<string>> TLexicon;

	class CLexicon
	{
	public:
		CLexicon(void);
		virtual ~CLexicon(void);
		bool Init(const char*);
		Pos GetPos(const char*);

	protected:
		TLexicon m_Lexicon;

	private:
		static void SetLexicon(void* ptr, int value, string str)
		{
			CLexicon* self = (CLexicon*) ptr;
			ToUpper(str);
			self->m_Lexicon[(Pos)value].push_back(str);
		}

	};
}
