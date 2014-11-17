// SettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QueryCGI.h"
#include "SettingDlg.h"
#include <algorithm>
#include <list>
using namespace std;

// CSettingDlg dialog

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDT_IP, m_edtIPAddress);
	DDX_Control(pDX, IDC_EDT_PORT, m_edtPort);
	DDX_Control(pDX, IDC_EDT_USER, m_edtUser);
	DDX_Control(pDX, IDC_EDT_PW, m_edtPW);

	DDX_Text(pDX, IDC_EDT_IP, m_csIPAddress);
	DDX_Text(pDX, IDC_EDT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDT_USER, m_csUsername);
	DDX_Text(pDX, IDC_EDT_PW, m_csPassword);

	DDV_MaxChars(pDX, m_csUsername, 15);
	DDV_MaxChars(pDX, m_csPassword, 15);
	DDV_MaxChars(pDX, m_csIPAddress, 1024);
	DDV_MinMaxShort(pDX, m_nPort, 1, 9999);
	DDX_Control(pDX, IDC_LIST1, m_lstSrcCgi);
	DDX_Control(pDX, IDC_LIST2, m_lstSelCgi);


}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
//    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_BTN_ADD, &CSettingDlg::OnBnClickedBtnAdd)
    ON_BN_CLICKED(IDC_BTN_REMOVE, &CSettingDlg::OnBnClickedBtnRemove)
    ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    CListBox *lstSelCgi = (CListBox *)GetDlgItem(IDC_LIST2);
    CListBox *lstSrcCgi = (CListBox *)GetDlgItem(IDC_LIST1);
	USES_CONVERSION;
	for (vector<string>::iterator it = m_vcSelCgi.begin(); it != m_vcSelCgi.end(); it++)
    {
		lstSelCgi->InsertString(lstSelCgi->GetCount(), A2W(it->c_str()));
    }

    
	for (vector<string>::iterator it = m_vcSrcCgi.begin(); it != m_vcSrcCgi.end(); it++)
    {
		lstSrcCgi->InsertString(lstSrcCgi->GetCount(), A2W(it->c_str()));
    }

    return TRUE;

}

void CSettingDlg::Add_Remove_Ctrl(CListBox *AddCtrl, CListBox *RemoveCtrl, vector<CString> &vcAdd, vector<CString> &vcRemove)
{
	int iTotal = RemoveCtrl->GetCount() + AddCtrl->GetCount();
	int *lp = new int[iTotal];

	int iSelCount = RemoveCtrl->GetSelItems(iTotal, lp);
	USES_CONVERSION;
	for(int i = 0; i < iSelCount; i++)
	{
		CString str;
		RemoveCtrl->GetText(lp[i] - i, str);
		RemoveCtrl->DeleteString(lp[i] - i);
		AddCtrl->AddString(str);

		vcAdd.push_back(str);

		if (find(vcRemove.begin(), vcRemove.end(), str) != vcRemove.end())
			vcRemove.erase(remove(vcRemove.begin(), vcRemove.end(), str));
	}
	delete[] lp;
}

void CSettingDlg::OnBnClickedBtnAdd()
{
	USES_CONVERSION;
	if(m_lstSrcCgi.GetSelCount() <= 0)
		return;

	Add_Remove_Ctrl(&m_lstSelCgi, &m_lstSrcCgi, m_vcAddCgi, m_vcRemoveCgi);
}

void CSettingDlg::OnBnClickedBtnRemove()
{
	USES_CONVERSION;
    if(m_lstSelCgi.GetSelCount() <= 0)
		return;
	Add_Remove_Ctrl(&m_lstSrcCgi, &m_lstSelCgi, m_vcRemoveCgi, m_vcAddCgi);
}

void CSettingDlg::OnBnClickedOk()
{
    // TODO: Add your control notification handler code here
    UpdateData();
    OnOK();
}

void CSettingDlg::getSrcCgi(vector<string> &vcSrcCgi)
{
	vcSrcCgi = m_vcSrcCgi;
}

void CSettingDlg::setSrcCgi(vector<string> &vcSrcCgi)
{
	m_vcSrcCgi = vcSrcCgi;
}

void CSettingDlg::getSelCgi(vector<string> &vcSelCgi)
{
	vcSelCgi = m_vcSelCgi;
}
void CSettingDlg::setSelCgi(vector<string> &vcSelList)
{
	m_vcSelCgi = vcSelList;
}

void CSettingDlg::setAuth(CString &csIPAddress, CString &csUsername, CString &csPassword, short &nPort)
{
	m_csUsername = csUsername;
	m_csIPAddress = csIPAddress;
	m_csPassword = csPassword;
	m_nPort = nPort;
}
void CSettingDlg::getAuth(CString &csIPAddress, CString &csUsername, CString &csPassword, short &nPort)
{
	csUsername = m_csUsername;
	csIPAddress = m_csIPAddress;
	csPassword = m_csPassword;
	nPort = m_nPort;
}

void CSettingDlg::getAddCgi(vector<CString> &vcAddCgi)
{
	vcAddCgi = m_vcAddCgi;
}
void CSettingDlg::getRemoveCgi(vector<CString> &vcRemoveCgi)
{
	vcRemoveCgi = m_vcRemoveCgi;
}
