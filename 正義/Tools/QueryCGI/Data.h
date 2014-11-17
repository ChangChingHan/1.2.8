#ifndef _DATA_H
#define  _DATA_H
#include "curl.h"
#include <iostream>
using namespace std;

struct Data
{
	string szIPAddress;	
	string szUsername;
	string szPassword;
	short nPort;
	string szResponse;
	string szUrl;

	int iErrorCode;
	int iHttpCode;

	Data():
		iErrorCode(0),
		iHttpCode(200)
	{}

	void operator =(Data &data)
	{
		szPassword = data.szPassword;
		szUsername = data.szUsername;
		szIPAddress = data.szIPAddress;
		nPort = data.nPort;
		szUrl = data.szUrl;
		szResponse = data.szResponse;
		iErrorCode = data.iErrorCode;
		iHttpCode = data.iHttpCode;
	}

	Data(string &IPAddress, string &Username,	string &Password, short &Port)
	{
		szPassword = Password;
		szUsername = Username;
		szIPAddress = IPAddress;
		nPort = Port;
		
		iErrorCode = 0;
		iHttpCode = 200;
	}

	Data(string &IPAddress, string &Username, string &Password, short &Port, string &Response)
	{
		szPassword = Password;
		szUsername = Username;
		szIPAddress = IPAddress;
		nPort = Port;
		szResponse = Response;		
		iErrorCode = 0;
		iHttpCode = 200;

	}

	string getUrl()
	{
		char url[1024];
		if (szPassword.length() < 1 || szUsername.length() < 1)
			sprintf(url, "http://%s:%d%s",
			szIPAddress.c_str(), nPort, szUrl.c_str());
		else
			sprintf(url, "http://%s:%s@%s:%d%s",
			szUsername.c_str(), szPassword.c_str(), szIPAddress.c_str(), nPort, szUrl.c_str());
		return string(url);
	}
};



#endif