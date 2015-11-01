#include "FDParser.h"
#include "RTFparser.h"
#include "XmlReader.h"

namespace Text2Scene
{
	CFDParser::CFDParser()
	{
		CXmlReader xml;
		if (xml.Read("..\\Text2Scene\\Tokens.xml"))
		{
			xml.ParseId((void*)this, xml.Doc().child("tokens").child("FinalDraft").child("description"), SetDescription);
			xml.ParseId((void*)this, xml.Doc().child("tokens").child("FinalDraft").child("location"), SetLocation);
			xml.ParseId((void*)this, xml.Doc().child("tokens").child("FinalDraft").child("time"), SetTime);
		}
	}

	CFDParser::~CFDParser()
	{
		DeleteVector(&m_Screenplay);
	}

	bool CFDParser::Read(const char* filename)
	{
		FILE *fp;
		if ( !(fp = fopen(filename, "r")) )
		{
			return false ;
		}
		CRTFParser rtf;
		int ec = rtf.ecRtfParse(fp);
		fclose(fp);
		if (ec != ecOK)
		{
			return false;
		}
		// Parse RTF objects to create the Screenplay structure
		SRTFObject*	pRTFobj = rtf.GetObjects();
		while( pRTFobj )
		{
			if (pRTFobj->szObjectInfo != NULL)
			{
				m_Screenplay.push_back(new pair<Description, string>(
					pRTFobj->szObjectStyle ? GetDescription(pRTFobj->szObjectStyle->szStyleName) : Description::Unformated, 
					pRTFobj->szObjectInfo));
			}
			pRTFobj = pRTFobj->pNext ;
		}
		return true;
	}

	TScreenplay& CFDParser::GetScreenplay()
	{
		return m_Screenplay;
	}

	Description CFDParser::GetDescription(const char* s)
	{
		TDescMap::iterator p;
		vector<string>::iterator q;
		string t(s);
		ToUpper(t);
		for(p = m_Descriptions.begin(); p != m_Descriptions.end(); ++p)
		{
			for(q = p->second.begin(); q != p->second.end(); ++q)
			{
				if (q->c_str() == t)
				{
					return p->first;
				}
			}
		}
		return Description::Unformated;
	}

	Time CFDParser::GetTime(const char* s, string& r)
	{
		TTimeMap::iterator p;
		vector<string>::iterator q;
		string t(s);
		ToUpper(t);
		int pos;
		for(p = m_Times.begin(); p != m_Times.end(); ++p)
		{
			for(q = p->second.begin(); q != p->second.end(); ++q)
			{
				pos = t.find(*q);
				if (pos != string::npos)
				{
					r.erase(pos, q->length());
					return p->first;
				}
			}
		}
		return Time::Undefined;
	}

	Location CFDParser::GetLocation(const char* s, string& r)
	{
		TLocMap::iterator p;
		vector<string>::iterator q;
		string t(s);
		ToUpper(t);
		int pos;
		for(p = m_Locations.begin(); p != m_Locations.end(); ++p)
		{
			for(q = p->second.begin(); q != p->second.end(); ++q)
			{
				pos = t.find(*q);
				if (pos != string::npos)
				{
					r.erase(pos, q->length());
					return p->first;
				}
			}
		}
		return Location::Unknown;
	}
}
