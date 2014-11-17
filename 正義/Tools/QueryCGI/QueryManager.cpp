#include "stdafx.h"
#include "QueryManager.h"
#include "debug.h"
#include "json.h"
#include <algorithm>

#define THREAD_TIMEOUT 20000

QueryManager::QueryManager(void *parent):
m_Parent(parent),
m_ResponseCallback(NULL),
m_FinishedCallback(NULL),
m_bExit(false),
m_iQueryCount(-1)
{

}

QueryManager::~QueryManager()
{

}



void QueryManager::GetProfilePath(char *chPath)
{
    //Get exec path
    GetModuleFileNameA(NULL, chPath, MAX_PATH);
    (strrchr(chPath, _T('\\')))[1] = 0;
    sprintf(chPath, "%s\%s", chPath, "profile.json");
}


DWORD WINAPI QueryManager::wait_thread_func(QueryManager *qm)
{
	int running_count = 0;
	int count = 0;
	Data *data = NULL;
	do
	{
		if (qm->m_bExit)
			return 0;

			data = NULL;

			::WaitForSingleObject(qm->m_hPostEvent, INFINITE);	
			//pop data
			::WaitForSingleObject(qm->m_hCoreMtx, INFINITE);	
			
			if (qm->m_queData.size())
			{
				data = qm->m_queData[0];
				qm->m_queData.pop_front();
				qm->m_iQueryCount--;

			}
			running_count = qm->m_iQueryCount;
			::ReleaseMutex(qm->m_hCoreMtx);

			//update to UI
			if ((qm->m_ResponseCallback != NULL) && !qm->m_bExit && data)
			{
				::WaitForSingleObject(qm->m_hUserMtx, INFINITE);
				
				qm->m_ResponseCallback(qm->m_Parent, data);
				delete data;
				::ReleaseMutex(qm->m_hUserMtx);

				if (qm->m_FinishedCallback && !running_count)
				{
					::WaitForSingleObject(qm->m_hUserMtx, INFINITE);
					qm->m_FinishedCallback(qm->m_Parent, qm);
					::ResetEvent(qm->m_hPostEvent);
					::ReleaseMutex(qm->m_hUserMtx);					
				}
			}

			::Sleep(1);
	} while (!qm->m_bExit);
	return 0;
}

void QueryManager::OnQueryResponse(QueryManager *qm, Query *query)
{
	if (!qm->m_bExit)
	{
		::WaitForSingleObject(qm->m_hCoreMtx, INFINITE);
		Data *data = new Data(query->getData());
		qm->m_queData.push_back(data);		
		::ReleaseMutex(qm->m_hCoreMtx);
	}
}

void QueryManager::Initialize(
        void(*finished_callback)(void *, QueryManager *), 
        void(*response_callback)(void *, Data *))
{
	CHAR file[MAX_PATH];

	m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)wait_thread_func, this, 0, &m_dwThreadID);
	__dbg_SetThreadName(m_dwThreadID, "QueryManager Thread");
	m_hPostEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hCoreMtx = CreateMutex(NULL, FALSE, NULL);
	m_hUserMtx = CreateMutex(NULL, FALSE, NULL);

    GetProfilePath(file);

    m_ResponseCallback = response_callback;
    m_FinishedCallback = finished_callback;

	char *chJson = LoadFile(file);

	if (chJson == NULL)
	{
		char *cgi_list[] ={	"info",	"camera_info",	"datetime",	"user_list","group_list","stream","video","audio_in",
			"video_info","sensor_info","sensor","sensor_night","dn_set","audio","audiopost","network","ddns","httpport",
			"motion","event_input","action_mail","nas","io","led","lightsensor","rs485","sdcard","record_prepost"};
		Value root;
		root["username"] = "root";
		root["password"] = "pass";
		root["ipaddr"] = "127.0.0.1";
		root["port"] = 80;

		int count = sizeof(cgi_list)/sizeof(char*);
		for(int i = 0; i < count; i++)
		{
			root["cgi_list"].append(cgi_list[i]);
		}
		root["cgi"] = Value.null;
		string szJson = root.toStyledString();
		ParseProfile(szJson.c_str());

	}
	else
	{
		ParseProfile(chJson);		
		::free(chJson);
	}

	Query::Initialize();

	if(m_FinishedCallback != NULL)
		m_FinishedCallback(m_Parent, this);
}


void QueryManager::Destory()
{
    CHAR file[MAX_PATH];
    GetProfilePath(file);
    SaveFile(file);

	m_ResponseCallback = NULL;
	m_FinishedCallback = NULL;

	m_bExit = true;
	m_iQueryCount = 0;
	::SetEvent(m_hPostEvent);

	vector<HANDLE> vchQuery;
	for (map<string, Query *>::iterator it = m_mapQuery.begin(); it != m_mapQuery.end(); it++)
	{
		Query *query = it->second;
		vchQuery.push_back(query->Abort());
	}

	if(vchQuery.size())
		WaitForMultipleObjects(vchQuery.size(), &vchQuery[0], TRUE, INFINITE);

	if(WaitForSingleObject(m_hThread, THREAD_TIMEOUT) == WAIT_TIMEOUT)
		::TerminateThread(m_hThread, m_dwThreadID);

	//Release Query
	for (map<string, Query *>::iterator it = m_mapQuery.begin(); it != m_mapQuery.end(); it++)
	{
		Query *query = it->second;

		if(query != NULL)
			delete query;
	}

	//Release Data
	for (deque<Data *>::iterator it = m_queData.begin(); it != m_queData.end(); it++)
	{
		Data *data = *it;

		if(data != NULL)
			delete data;
	}


	::CloseHandle(m_hPostEvent);
	::CloseHandle(m_hThread);
	::CloseHandle(m_hCoreMtx);
	::CloseHandle(m_hUserMtx);
}


bool QueryManager::ParseProfile(const char *chJson)
{
    Reader reader;
    Value jsData;
    if (!reader.parse(chJson, jsData))
        return false;

	m_mapQuery.clear();

    if (!jsData.isMember("ipaddr") || !jsData.isMember("port") || !jsData.isMember("username") || !jsData.isMember("password"))
        return false;


	m_szIPAddress = jsData["ipaddr"].asString();
	m_szUsername = jsData["username"].asString();
	m_szPassword = jsData["password"].asString();
	m_nPort = (short)jsData["port"].asUInt();
    Value cgi_list = jsData["cgi_list"];

	for (Value::iterator it = cgi_list.begin(); it != cgi_list.end(); it++)
		m_vcCgiList.push_back((*it).asString());

	if (!jsData.isMember("cgi"))
		return true;
    Value sel_cgi = jsData["cgi"];

	m_iQueryCount = sel_cgi.size();
	for (Value::iterator it = sel_cgi.begin(); it != sel_cgi.end(); it++)
	{
		string szCgi = it.key().asString();
		string szResponse = sel_cgi[szCgi].toStyledString();

		Query *query = new Query(this);
		Data data(m_szIPAddress, m_szUsername, m_szPassword, m_nPort, szResponse);

		data.szUrl = "/config/" + szCgi + ".cgi";
		query->setResponseCallback((void(*)(void *, Query *))&OnQueryResponse);
		query->setData(data);


		m_mapQuery[szCgi] = query;
		OnQueryResponse(this, query);
    }

	::SetEvent(m_hPostEvent);

    return true;
}

char *QueryManager::LoadFile(const char *file)
{
	ifstream fin;
	fin.open(file);
	if (!fin.is_open())
		return NULL;

	// get file size 
	fin.seekg(0, ifstream::end);
	int size = fin.tellg();
	fin.seekg(0);
	char *chTemp = (char *)::malloc(size);
	memset(chTemp, 0, size);

	fin.read(chTemp, size);
	fin.close();
	return chTemp;
}

void QueryManager::SaveFile(const char *file)
{
    Value root;
    Reader reader;
    for(vector<string>::iterator it = m_vcCgiList.begin();it != m_vcCgiList.end(); it++)
        root["cgi_list"].append(*it);

    root["username"] = m_szUsername;
    root["password"] = m_szPassword;
    root["ipaddr"] = m_szIPAddress;
    root["port"] = m_nPort;

	for (map<string, Query *>::iterator it = m_mapQuery.begin(); it != m_mapQuery.end(); it++)
    {
        Value response;
		Query *query = it->second;
		Data data = query->getData();
		string szResponse = data.szResponse;
		string szUrl = data.szUrl;

		int pos = szUrl.find_last_of("/");
		string cgi = szUrl.substr(pos + 1, szUrl.length() - pos - 5);

		if (!szResponse.empty())
        {
			if (reader.parse(szResponse, response))
				root["cgi"][cgi] = response;
            else
				root["cgi"][cgi] = Value::null;
        }
        else
			root["cgi"][cgi] = Value::null;
    }

    string json = root.toStyledString();
    ofstream fout;
    fout.open(file);
    fout << json;    
    fout.close();
    return;
}

Query *QueryManager::CreateQuery(string szCgi)
{
	Query *query = new Query(this);
	Data data(m_szIPAddress, m_szUsername, m_szPassword, m_nPort);
	
	data.szUrl = "/config/" + szCgi + ".cgi";
	query->setData(data);

	
	query->setResponseCallback((void(*)(void *, Query *))&OnQueryResponse);
	m_mapQuery[szCgi] = query;

	return query;
}

void QueryManager::Post()
{
	if (m_mapQuery.size() <= 0)
		return;

	m_iQueryCount = (int)m_mapQuery.size();
	for (size_t index = 0; index < m_queData.size(); index++)
	{
		delete m_queData[index];
	}

	m_queData.clear();
	for (map<string, Query *>::iterator it = m_mapQuery.begin(); it != m_mapQuery.end(); it++)
	{
		it->second->setAuth(m_szUsername, m_szPassword, m_szIPAddress, m_nPort);
		it->second->Post();
	}
	::SetEvent(m_hPostEvent);
}

void QueryManager::getAuth(string &chUser, string &chPW, string &chIP, short &nPort)
{
    chIP = m_szIPAddress;
    chPW = m_szPassword;
    chUser = m_szUsername;
    nPort = m_nPort;
}

void QueryManager::setAuth(string chUser, string chPW, string chIP, short nPort)
{
	m_szIPAddress = chIP;
	m_szPassword = chPW;
	m_szUsername = chUser;
	m_nPort =nPort;
}


bool QueryManager::getCgiList(vector<string> &cgi_list)
{
	cgi_list = m_vcCgiList;

    return true;

}

void QueryManager::getSelCgi(vector<string> &cgi_list)
{
	cgi_list.clear();
	for (map<string, Query *>::iterator it = m_mapQuery.begin(); it != m_mapQuery.end(); it++)
	{
		cgi_list.push_back(it->first);
	}
}

void QueryManager::AddCgi(string szCgi)
{
	m_mapQuery[szCgi] = CreateQuery(szCgi);
}
void QueryManager::RemoveCgi(string szCgi)
{

	for (map<string, Query *>::iterator it = m_mapQuery.begin(); it != m_mapQuery.end(); it++)
	{
		if (it->first == szCgi)
		{
			Query *query = it->second;
			delete query;
			m_mapQuery.erase(it);
			break;
		}
	}
	
}
