#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "Data.h"
#include "QueryManager.h"
#include "SettingDlg.h"
// CMainDlg dialog
#define WM_RESPONSE WM_USER
#define WM_FINISHED WM_USER + 1
#define WM_AUTH WM_USER + 2
class CMainDlg  : public CDialog
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMainDlg();

// Dialog Data
	enum { IDD = IDD_MAIN_DIALOG };

protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()

	static void OnResponse(CMainDlg *dlg, Data *qm);
    static void OnFinished(CMainDlg *dlg, QueryManager *qm);
	void ShowAuth();
private:
    CStatic m_txtAuth;
public:
    afx_msg void OnBnClickedBtnModify();
    afx_msg void OnBnClickedBtnQuery();
    INT_PTR DoModal(QueryManager *data);
private:
    CListCtrl m_lstMsg;
    CButton m_btnModify;
    CButton m_btnQuery;

    QueryManager *m_qm;

	CString m_csIPAddress;
	CString m_csPassword;
	CString m_csUsername;
	short m_nPort;


public:
	afx_msg void OnClose();
};
