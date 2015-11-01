#include "Lexicon.h"
#include "XmlReader.h"

using namespace std;

namespace Text2Scene
{
	CLexicon::CLexicon(void)
	{
	}

	CLexicon::~CLexicon(void)
	{
	}

	bool CLexicon::Init(const char* filename)
	{
		CXmlReader xml;
		if (!xml.Read(filename))
		{
			return false;
		}
		xml.ParseId((void*)this, xml.Doc().child("french"), CLexicon::SetLexicon);
		return true;
	}

	Pos CLexicon::GetPos(const char* s)
	{
		TLexicon::iterator p;
		vector<string>::iterator q;
		string t(s);
		ToUpper(t);
		string u(t);
		RemoveAccents(u);
		int pos;
		for(p = m_Lexicon.begin(); p != m_Lexicon.end(); ++p)
		{
			for(q = p->second.begin(); q != p->second.end(); ++q)
			{
				pos = t.find(*q);
				if (pos != string::npos)
				{
					return p->first;
				}

				pos = u.find(*q);
				if (pos != string::npos)
				{
					return p->first;
				}
			}
		}
		return Pos::UNDEFINED;
	}
}