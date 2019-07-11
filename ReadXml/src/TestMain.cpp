#include "TinyXml/tinyxml.h"
void parsePoolConfig(const char*configFile)
{
    if (configFile == NULL)
    {
	printf("configFile == NULL %d\n");
	return;
    }
    TiXmlDocument* m_xmlDoc = new TiXmlDocument(configFile);
    if (!m_xmlDoc->LoadFile())
    {
	printf("parsePoolConfig  LoadFile  %s failed\n", configFile);
	delete m_xmlDoc;
	return ;
    }
    TiXmlElement *root = m_xmlDoc->FirstChildElement("config");
    if (root)
    {
	TiXmlElement* Node=root->FirstChildElement("pond_config");
	if(Node)
	{
            const char*str=Node->Attribute("current_pond");
            if(str!=NULL)
            {
                int current_pond = atof(str);
                printf("current_pond %d\n", current_pond);
            }
            else
            {
             printf("current_pond str isnull\n");
            }
        }
         Node=root->FirstChildElement("table_info");
        if(Node)
        {			
            TiXmlElement* SubNode=Node->FirstChildElement("table_info_item");
            for(SubNode;SubNode;SubNode=SubNode->NextSiblingElement("table_info_item"))
            {
                int table_no = 0;
                const char*str=SubNode->Attribute("table_id");
                if(str!=NULL)
                {
                    table_no = atoi(str);
                }

				
            }
        }
    }

   
}

int main()
{
    parsePoolConfig("TableConfig.xml");
}
