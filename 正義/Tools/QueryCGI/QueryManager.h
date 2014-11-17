#ifndef _QUERYMANAGER_H
#define  _QUERYMANAGER_H
#include "Query.h"
#include "DataReader.h"
#include "Data.h"

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <deque>
using namespace Json;
using namespace std; 

class QueryManager
{
public:
	QueryManager(void *parent = NULL);
	virtual ~QueryManager();

    void Initialize(
        void(*finished_callback)(void *, QueryManager *), 
        void(*response_callback)(void *, Data *));

	void Destory();

	void getAuth(string &chUser, string &chPW, string &chIP, short &nPort);
	void setAuth(string chUser, string chPW, string chIP, short nPort);

	void Post();

    bool getCgiList(vector<string> &cgi_list);
    void getSelCgi(vector<string> &cgi_list);

	void AddCgi(string szCgi);
	void RemoveCgi(string szCgi);

protected:
    void GetProfilePath(char *chPath);
    char *LoadFile(const char *file);
    void SaveFile(const char *file);
	bool ParseProfile(const char *chJson);
	Query *CreateQuery(string);
	
	static DWORD WINAPI wait_thread_func(QueryManager *);
	static void OnQueryResponse(QueryManager *qm, Query *query);
private:
	deque<Data *> m_queData;
	map<string, Query *> m_mapQuery;

    vector<string> m_vcCgiList;
	//vector<string> m_vcSelCgi;

	HANDLE m_hThread;
	HANDLE m_hPostEvent;
	DWORD m_dwThreadID;

	string m_szIPAddress;
	string m_szUsername;
	string m_szPassword;
	short m_nPort;

	HANDLE m_hCoreMtx;
	HANDLE m_hUserMtx;
	void *m_Parent;

	int m_iQueryCount;

	bool m_bExit;

	void(*m_FinishedCallback)(void *, QueryManager *);
    void(*m_ResponseCallback)(void *, Data *);

};
#endif