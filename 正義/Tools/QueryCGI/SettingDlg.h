
#pragma once
#include "afxwin.h"
#include "QueryManager.h"
#include <list>

using namespace std;

// CSettingDlg dialog

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingDlg();

// Dialog Data
	enum { IDD = IDD_SETTING_DIALOG };

protected:
    virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	void Add_Remove_Ctrl(CListBox *AddCtrl, CListBox *RemoveCtrl, vector<CString> &vcAdd, vector<CString> &vcRemove);

public:
//    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
    CEdit m_edtPort;
    CEdit m_edtUser;
    CEdit m_edtPW;
    CListBox m_lstSrcCgi;
    CListBox m_lstSelCgi;

	CString m_csIPAddress;
	CString m_csPassword;
    CString m_csUsername;
    short m_nPort;

	vector<string> m_vcSrcCgi;
	vector<string> m_vcSelCgi;
	vector<CString> m_vcAddCgi;
	vector<CString> m_vcRemoveCgi;

public:
    afx_msg void OnBnClickedBtnAdd();
    afx_msg void OnBnClickedBtnRemove();

    afx_msg void OnBnClickedOk();

	void getSrcCgi(vector<string> &);
	void setSrcCgi(vector<string> &);

	void getSelCgi(vector<string> &);
	void setSelCgi(vector<string> &);

	void getAddCgi(vector<CString> &);
	void getRemoveCgi(vector<CString> &);

	void getAuth(CString &csIPAddress, CString &csUsername, CString &csPassword, short &nPort);
	void setAuth(CString &csIPAddress, CString &csUsername, CString &csPassword, short &nPort);
	//void getAddList(vector<CString> &);
	//void getRemoveList(vector<CString> &);
private:
	CEdit m_edtIPAddress;
};
