#ifndef _QUERY_H
#define  _QUERY_H
#include <Windows.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <assert.h>   
#include "Data.h"
#include "curl.h"
//#include "interface.h"

using namespace std;

class Query// : public IQuery
{
public:
    //Query();
	Query(void *parent);
    virtual ~Query();
	void Post();

	HANDLE Abort();

	Data getData();
	void setData(Data &data);

	void setAuth(string &chUser, string &chPW, string &chIP, short &nPort);

    void getTimeout(UINT &nTimeout);
    void setTimeout(const UINT nTimeout);

	void setResponseCallback(void (*)(void*, Query *));

    static void Initialize();

protected:
	static size_t write_callback(void *buffer, size_t size, size_t nmemb, void *stream);
    static size_t progress_callback(Query *status, double dLenght, double dNow, double ultotal, double ulnow);
    static DWORD WINAPI post_thread_func(Query *) ;

private:

    UINT m_iTimeout;
    HANDLE m_hThread;
    HANDLE m_hPostEvent;

    volatile bool m_bExit;
    DWORD m_dwThreadID;

    CURL *m_Curl;

	Data m_Data;

	void(*m_ResponseCallback)(void *, Query *);
	void *m_Parent;
};
#endif