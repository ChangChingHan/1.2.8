#include "stdafx.h"
#include "Query.h"
#include "debug.h"
#include <string.h>
#include <Winsock2.h>
#pragma comment(lib,"Ws2_32.lib")

#define THREAD_BASE_TIMEOUT 3000

Query::Query(void *parent) :
m_iTimeout(10000),
m_bExit(false),
m_Parent(parent)
{
    m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)post_thread_func, this, CREATE_SUSPENDED, &m_dwThreadID);
	
    m_hPostEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    ::ResumeThread(m_hThread);
}

Query::~Query()
{
	if(!m_bExit)
	{
		m_bExit = true;
		SetEvent(m_hPostEvent);
		if(::WaitForSingleObject(m_hThread, THREAD_BASE_TIMEOUT + m_iTimeout) == WAIT_TIMEOUT)
			::TerminateThread (m_hThread, m_dwThreadID);  
	}
	if(m_hThread != NULL)
		::CloseHandle(m_hThread);

	if(m_hPostEvent != NULL)
		::CloseHandle(m_hPostEvent);
}

void Query::Initialize()
{
    curl_global_init(CURL_GLOBAL_ALL);  
}

void Query::Post()
{
    SetEvent(m_hPostEvent);
}

DWORD WINAPI Query::post_thread_func(Query *query) {


    int i=0;
    int success=0;
    int failed=0;
    int second=0;
    do 
    {
        ::WaitForSingleObject(query->m_hPostEvent, INFINITE);
        if(query->m_bExit)
            break;

        CURLcode result;

		query->m_Data.szResponse = "";
		string szUrl = query->m_Data.getUrl();

        query->m_Curl = curl_easy_init();

		result = curl_easy_setopt(query->m_Curl, CURLOPT_URL, szUrl.c_str());

        result = curl_easy_setopt(query->m_Curl, CURLOPT_NOSIGNAL, 1L); 

        result = curl_easy_setopt(query->m_Curl, CURLOPT_TIMEOUT_MS, query->m_iTimeout); 

        result = curl_easy_setopt(query->m_Curl, CURLOPT_NOPROGRESS, false);

        result = curl_easy_setopt(query->m_Curl, CURLOPT_PROGRESSFUNCTION, Query::progress_callback);

        result = curl_easy_setopt(query->m_Curl, CURLOPT_PROGRESSDATA, query);

        result = curl_easy_setopt(query->m_Curl, CURLOPT_WRITEFUNCTION, Query::write_callback);

		result = curl_easy_setopt(query->m_Curl, CURLOPT_WRITEDATA, query);

        result = curl_easy_perform(query->m_Curl);
        if(result == CURLE_OK && !query->m_bExit)
        {
			curl_easy_getinfo(query->m_Curl, CURLINFO_RESPONSE_CODE, &query->m_Data.iHttpCode);
        }
		query->m_Data.iErrorCode = (int)result;

		curl_easy_cleanup(query->m_Curl);

		if (query->m_ResponseCallback)
			query->m_ResponseCallback(query->m_Parent, query);

    } while (!query->m_bExit);
	
    return NULL;
}

size_t Query::progress_callback(Query *query, double dLenght, double dNow, double ultotal, double ulnow)
{
    return 0;
}

size_t Query::write_callback(void *lpbuffer, size_t size, size_t nmemb, void *stream) 
{
	Query *query = (Query *)stream;
	query->m_Data.szResponse.append((char *)lpbuffer);
    size_t block = size * nmemb;

	if(query->m_bExit)
		return CURLE_WRITE_ERROR;
	else 
		return block;
}

HANDLE Query::Abort()
{
	m_bExit = true;
	SetEvent(m_hPostEvent);
	return m_hThread;
}

Data Query::getData()
{
	return m_Data;
}

void Query::setData(Data &data)
{
	m_Data = data;
}

void Query::getTimeout(UINT &nTimeout)
{
    nTimeout = m_iTimeout;

}
void Query::setTimeout(const UINT nTimeout)
{
    m_iTimeout = nTimeout;
}

void Query::setResponseCallback(void(*callback)(void*, Query *))
{
	m_ResponseCallback = callback;
}

void Query::setAuth(string &szUsername, string &szPassword, string &szIPAddress, short &nPort)
{
	m_Data.szUsername = szUsername;
	m_Data.szPassword = szPassword;
	m_Data.szIPAddress = szIPAddress;
	m_Data.nPort = nPort;
}
