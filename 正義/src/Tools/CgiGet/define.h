#include <tchar.h>
#include <map>

#pragma warning(disable : 4996) 
#pragma warning(disable : 4129) 

typedef struct Url_t
{
    TCHAR IpAddr[1024];
    DWORD Port;
    TCHAR Username[16];
    TCHAR Password[16];
    TCHAR Info[255];
    TCHAR Response[4096];
}Url;

class URL
{
public:
    static CString GetRespose(const TCHAR *url);

private:
    TCHAR chIpAddr[1024];
    DWORD nPort;
    TCHAR chUsername[16];
    TCHAR chPassword[16];
    TCHAR chInfo[255];
    TCHAR chResponse[4096];
};