#include<iostream>
#include<string>
#include <sstream>
#include <fstream>
#include "document.h"
#include "stringbuffer.h"
#include "prettywriter.h"
using namespace std;
using namespace rapidjson;
void ReadPrizeConfig()
{		
    string poolConfigStr = "";
    ifstream fin;
    string temp;
    fin.open("test.json", ios::in);
    while (fin.good()){
	temp.clear();
	getline(fin, temp);
	poolConfigStr += temp; // += 已被重载
    }
    fin.close();
    Document doc;
    doc.Parse<0>(poolConfigStr.c_str());
    if (doc.HasMember("ARRAY_CONFIG") && doc["ARRAY_CONFIG"].IsArray())
    {
        for (int i = 0; i < doc["ARRAY_CONFIG"].Size(); i++)
	{
            if (doc["ARRAY_CONFIG"][i].HasMember("type"))
            {
		int iTypeValue = doc["ARRAY_CONFIG"][i]["type"].GetInt();
                cout<<"iTypeValue: "<<iTypeValue<<endl;
            }
        }            
    }
    if (doc.HasMember("MYINTTYPE") )
    {
        int iIntValue = doc["MYINTTYPE"].GetInt();
        cout<<"iIntValue: "<<iIntValue<<endl;
    }
    if (doc.HasMember("MYSTRINGTYPE") )
    {
        string StringValue = doc["MYSTRINGTYPE"].GetString();
        cout<<"StringValue: "<<StringValue<<endl;
    }
}
int main()
{
    ReadPrizeConfig();
    return 0;
}