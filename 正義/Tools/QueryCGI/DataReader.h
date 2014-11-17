#ifndef _DATAPARSER_H
#define  _DATAPARSER_H

#include <vector>
#include "json.h"
#include "Data.h"

using namespace Json;
using namespace std;

class Item
{
public:
    CString csCGI;
    CString csKey;
    CString csValue;
};

class DataReader
{

public:


    bool ParseCGI(string chCGI, string chJson, vector<Item *> &vcItems);
private:
	typedef void (*PushItemCallback)(string szCGI, string szhKey, string szValue, vector<Item *> &vcItems);

	void InternalParse(string szCGI, Value response, vector<Item *> &vcItems, const char *chSubKey = NULL);
	void ParseProfileArray(string szCGI,const Value response, string szKey, vector<Item *> &vcItems, PushItemCallback callback);
	void ParseArray(string szCGI,const Value response, string szKey, vector<Item *> &vcItems, PushItemCallback callback);
	static void PushItem(string szCGI, string szKey, string szValue, vector<Item *> &vcItems);
	string RenameKeyByProfileKey(int index, string szSubKey);
	string RenameKeyByArray(string &szKey, string szSubKey);
	bool IsProfileKey(string szKey);
	int GetIndexByProfileKey(string szKey);
};

#endif 