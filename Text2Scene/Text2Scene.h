#pragma once
#include "FDParser.h"
#include "Lexicon.h"
using namespace std;

namespace Text2Scene
{
	class CText2Scene : public CLexicon
    {
	public:
		CText2Scene();
		virtual ~CText2Scene();
		bool Parse(const char*);

		CStage* GetStage() { return m_pStage; };
		TScreenplay GetSceenplay() { return m_pParser->GetScreenplay(); };

	protected:
		CStage *m_pStage;
		void DeleteStage();

	private:
		IParser *m_pParser ;
		CScene*	CreateScene(const char*);
		CActor*	CreateActor(const char*);

	};
}
