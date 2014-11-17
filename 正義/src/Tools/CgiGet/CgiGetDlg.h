
// CgiGetDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "json.h"
#include <vector>
#include "define.h"


// CCgiGetDlg dialog
class CCgiGetDlg : public CDialog
{
// Construction
public:
	CCgiGetDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CGIGET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

    //void AFXAPI DDX_TextByJson(CDataExchange* pDX, int nIDC, Json::Value& value, Json::ValueType _type);

    static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream);
    void GetCgiResponse(Url url);
    bool SetListCtrlItemsByJson(const char *chResponse, const char *chUrl);
public:
    afx_msg void OnBnClickedButtonGet();

private: 
#define URL_COUNT  10

    Url m_UrlList[URL_COUNT];
    int m_iUrlSize;
    TCHAR m_szExecPath[MAX_PATH + 1];
    Json::Value m_csResponse;
};
