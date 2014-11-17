
#include "stdafx.h"

#include "DataReader.h"
#include "config.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace Json;

void DataReader::PushItem(string szCGI, string szKey, string szValue, vector<Item *> &vcItems)
{
	USES_CONVERSION;
	Item *item = new Item();
	item->csCGI = A2W(szCGI.c_str());
	item->csKey = A2W(szKey.c_str());
	item->csValue = A2W(szValue.c_str());
	vcItems.push_back(item);

}


void DataReader::ParseProfileArray(string szCGI,const Value response, string szKey, vector<Item *> &vcItems, PushItemCallback PushItem)
{
	Json::Value jsValue;
	if(!response.isArray())
		return;

	if(szCGI == "video")
	{	
		for(Json::Value::iterator sub_it = response[0U].begin(); sub_it != response[0U].end(); sub_it++)
		{
			string szRenameKey;
			int profile_index;                    

			std::string szSubKey = sub_it.key().asString();
			profile_index = GetIndexByProfileKey(szKey);
			szRenameKey = RenameKeyByProfileKey(profile_index, szSubKey);
			jsValue = response[0U][szSubKey];
			std::string szValue = jsValue.asString();

			PushItem(szCGI, szRenameKey, szValue, vcItems);
		}
	}
	else if(szCGI == "video_info")
	{
		string ff = response.toStyledString();
		
		for(int index = 0; index != response.size(); index++)
		{
			string szValue = response[index].asString();

			PushItem(szCGI, szKey, szValue, vcItems);
		}
	}
}

void DataReader::ParseArray(string szCGI,const Value response, string szKey, vector<Item *> &vcItems, PushItemCallback PushItem)
{
	//USES_CONVERSION;
	Json::Value jsValue;
	for(Json::Value::iterator it = jsValue.begin(); it != jsValue.end(); it++)
	{
		string szSubKey = it.key().asString();
		string szValue = jsValue[szSubKey].asString();
		string szRenameKey = RenameKeyByArray(szKey, szSubKey);
		

		PushItem(szCGI, szSubKey, szValue, vcItems);
	}
}

int DataReader::GetIndexByProfileKey(string szKey)
{
	int index = 0;
	sscanf(szKey.c_str(), "profile_id_%d", &index);
	return index;
}

string DataReader::RenameKeyByProfileKey(int index, string szSubKey)
{
	char buff[100];
	sprintf(buff, "profile%d_%s", index, szSubKey.c_str());
	return string(buff);
}

string DataReader::RenameKeyByArray(string &szKey, string szSubKey)
{
	char buff[100];
	sprintf(buff, "%s_%s", szKey.c_str(), szSubKey.c_str());
	return string(buff);
}

bool DataReader::IsProfileKey(string szKey)
{
	if(szKey.length() < 11)
		return false;
	return memcmp(szKey.c_str(), "profile_id_", 11) == 0;
}

void DataReader::InternalParse(string szCGI, Value response, vector<Item *> &vcItems, const char *chSubKey)
{
	if (response.isNull())
	{
		PushItem(szCGI, "null", "null", vcItems);
		return;
	}
	for(Json::Value::iterator it = response.begin(); it != response.end(); it++)
	{
		std::string szKey = it.key().asString();
		Json::Value jsValue;

		if(IsProfileKey(szKey))
			ParseProfileArray(szCGI, response[szKey], szKey, vcItems, &PushItem);            
		else
		{
			jsValue = response[szKey];
			if(jsValue.isArray())
				ParseArray(szCGI, response[szKey], szKey, vcItems, &PushItem);
			else
			{
				string res;
				if(jsValue.isString())
					res = jsValue.asString();
				else
					res= jsValue.toStyledString();
				if(jsValue.isObject())
				{
					InternalParse(szCGI, jsValue, vcItems, szKey.c_str());
				}
				else if (jsValue.isNull())
				{
					PushItem(szCGI, "null", "null", vcItems);
				}
				else
				{
					char chRenameKey[2048];
					if(chSubKey != NULL)
						sprintf(chRenameKey, "%s_%s", chSubKey, szKey.c_str());
					else
						sprintf(chRenameKey, "%s", szKey.c_str());
						

					PushItem(szCGI, chRenameKey, res, vcItems);
				}
			}
		}
	}
}

bool DataReader::ParseCGI(string szCGI, string szJson, vector<Item *> &vcItems)
{
    Reader reader;
    Value response;
	if (szJson.empty() || szJson == "")
	{
		PushItem(szCGI, "null", "null", vcItems);
		return true;
	}
    if(reader.parse(szJson, response))
    {
        InternalParse(szCGI, response, vcItems);
    }
    else
        return false;
    return true;

}
