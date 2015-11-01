#include "XmlReader.h"

namespace Text2Scene
{
	bool CXmlReader::Read(const char* filename)
	{
		if (!m_Doc.load_file(filename))
		{
			return false;
		}
		return true;
	}

	/*
	TODO: remove the need of xml_node parameter for child: use xpath or do the search for a string
	*/
	void CXmlReader::ParseId(void* ptr, const xml_node tokens, void (*callback)(void* ptr, int value, string text))
	{
		int value;
		wstring wStr;
		string str;
		for (xml_node token = tokens.first_child(); token; token = token.next_sibling())
		{
			value = atoi(token.attribute("id").value());
			for (xml_node form = token.first_child(); form; form = form.next_sibling())
			{
				wStr = as_wide(form.child_value());
				str.assign(wStr.begin(), wStr.end());
				callback(ptr, value, str);
			}
		}
	}

}