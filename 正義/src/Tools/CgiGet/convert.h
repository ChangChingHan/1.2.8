void C2T(TCHAR** dest, const CHAR* src)
{
#ifdef _UNICODE
    if (src == NULL)
    {
        return ;
    }

    size_t alen = strlen(src) + 1;
    size_t  ulen = (size_t)MultiByteToWideChar(CP_ACP, 0, src,alen,NULL, 0 )+1;

    *dest = new WCHAR[ulen];
    ::MultiByteToWideChar(CP_ACP, 0, src, alen, *dest, ulen);
#else 
    int len = strlen(src)+1;
    *dest = new char[len];
    strcpy(*dest, src);
#endif
}

void T2C(CHAR** dest, const TCHAR* src)
{
    if(src == NULL)
        return ;
#ifdef _UNICODE
    size_t len = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0 , NULL, NULL);
    if (len == 0)
    {
        return;
    }
    *dest = new char[len];
    WideCharToMultiByte( CP_ACP, 0, src, -1, *dest, len, NULL, NULL );
#else
    int len = _tcslen(src) + 1;
    *dest = new TCHAR[len];
    strcpy(*dest, src);
#endif
}

TCHAR *CS2T(CString &src)
{
    int iLen = src.GetLength();
    TCHAR *dest = new TCHAR[iLen];
    lstrcpy(dest, src.GetBuffer(iLen));
    src.ReleaseBuffer();
    return dest;
}

