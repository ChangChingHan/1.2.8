// MainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QueryCGI.h"
#include "MainDlg.h"

#include <algorithm>
#include <functional>
// CMainDlg dialog


IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
{
}

CMainDlg::~CMainDlg()
{
}

BOOL CMainDlg::OnInitDialog()
{
    CDialog::OnInitDialog();


    // TODO: Add extra initialization here
    m_lstMsg.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES|LVS_EX_FLATSB);

    m_lstMsg.InsertColumn(0, _T("CGI"));
    m_lstMsg.InsertColumn(1, _T("Key"));
    m_lstMsg.InsertColumn(2, _T("Value"));

    m_lstMsg.SetColumnWidth(0, 80);    
    m_lstMsg.SetColumnWidth(1, 80);    
    m_lstMsg.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);

    
    //m_qm = new QueryManager(this);

	m_qm->Initialize(
		(void(*)(void*,QueryManager*))&OnFinished, 
		(void(*)(void*, Data*))&OnResponse);

	string szIPAddress, szUsername, szPassword;

	m_qm->getAuth(szUsername, szPassword, szIPAddress, m_nPort);
	USES_CONVERSION;
	m_csIPAddress = A2W(szIPAddress.c_str());
	m_csUsername = A2W(szUsername.c_str());
	m_csPassword = A2W(szPassword.c_str());

	ShowAuth();


    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_AUTH, m_txtAuth);
    DDX_Control(pDX, IDC_LIST_MSG, m_lstMsg);
    DDX_Control(pDX, IDC_BTN_MODIFY, m_btnModify);
    DDX_Control(pDX, IDC_BTN_QUERY, m_btnQuery);

}

INT_PTR  CMainDlg::DoModal(QueryManager *data)
{
    m_qm = data;
    return CDialog::DoModal();
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_MODIFY, &CMainDlg::OnBnClickedBtnModify)
    ON_BN_CLICKED(IDC_BTN_QUERY, &CMainDlg::OnBnClickedBtnQuery)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void CMainDlg::OnBnClickedBtnModify()
{
	// TODO: Add your control notification handler code here
	vector<string> vcList, _vcSelCgi;
	vector<string> vcSrcCgi, vcSelCgi;
	m_qm->getCgiList(vcList);
	m_qm->getSelCgi(_vcSelCgi);
	USES_CONVERSION;
	for (vector<string>::iterator it = vcList.begin(); it != vcList.end(); it++)
	{
		if (find(_vcSelCgi.begin(), _vcSelCgi.end(), *it) != _vcSelCgi.end())
		{
			vcSelCgi.push_back(*it);
		}
		else
			vcSrcCgi.push_back(*it);
	}

	CSettingDlg *dlg = new CSettingDlg();
	dlg->setAuth(m_csIPAddress, m_csUsername, m_csPassword, m_nPort);
	dlg->setSelCgi(vcSelCgi);
	dlg->setSrcCgi(vcSrcCgi);
	INT_PTR pRes = dlg->DoModal();

	
	
	if (pRes == 1)
	{
		CString csAuth;
		vector<CString> vcAddCgi, vcRemoveCgi;

		dlg->getAddCgi(vcAddCgi);
		dlg->getRemoveCgi(vcRemoveCgi);
		dlg->getAuth(m_csIPAddress, m_csUsername, m_csPassword, m_nPort);

		ShowAuth();

		for (vector<CString>::iterator it = vcRemoveCgi.begin(); it != vcRemoveCgi.end(); it++)
		{
			for (int index = 0; index < m_lstMsg.GetItemCount();)
			{
				CString csCgi = m_lstMsg.GetItemText(index, 0);
				if (csCgi == (*it))
				{
					m_lstMsg.DeleteItem(index);
					m_qm->RemoveCgi(W2A(*it));
				}
				else
					++index;
			}
		}

		for (vector<CString>::iterator it = vcAddCgi.begin(); it != vcAddCgi.end(); it++)
		{
			int nItemCount = m_lstMsg.GetItemCount();
			m_lstMsg.InsertItem(nItemCount, *it);
			m_lstMsg.SetItemText(nItemCount, 1, _T("null"));
			m_lstMsg.SetItemText(nItemCount, 2, _T("null"));

			m_qm->AddCgi(W2A(*it));
		}
		m_qm->setAuth(string(W2A(m_csUsername)), string(W2A(m_csPassword)), string(W2A(m_csIPAddress)), m_nPort);

		delete dlg;
	}
}

void CMainDlg::OnBnClickedBtnQuery()
{
    // TODO: Add your control notification handler code here
    this->m_btnQuery.EnableWindow(FALSE);
    this->m_btnModify.EnableWindow(FALSE);
    m_lstMsg.DeleteAllItems();
    m_qm->Post();
}

void CMainDlg::OnResponse(CMainDlg *dlg, Data *data)
{
	int pos = data->szUrl.find_last_of("/");
	string szCgi = data->szUrl.substr(pos + 1, data->szUrl.length() - pos - 5);
    CListCtrl *lstCtrl = (CListCtrl *)dlg->GetDlgItem(IDC_LIST_MSG); 
    CButton *btnQuery = (CButton *)dlg->GetDlgItem(IDC_BTN_QUERY); 
	CButton *btnModify = (CButton *)dlg->GetDlgItem(IDC_BTN_MODIFY); 
    int nItemCount;
	vector<Item *>items;
    if(data->iErrorCode == 0 && data->iHttpCode == 200)
    {
		btnQuery->EnableWindow(FALSE);
		btnModify->EnableWindow(FALSE);
		DataReader reader;
		if (!reader.ParseCGI(szCgi, data->szResponse, items))
            return;

        lstCtrl->SetRedraw(FALSE);  

        for(vector<Item *>::iterator it = items.begin(); it != items.end(); it++)
        {
			Item *item = *it;

            nItemCount = lstCtrl->GetItemCount();
            lstCtrl->InsertItem(nItemCount, item->csCGI); 
            lstCtrl->SetItemText(nItemCount, 1, item->csKey); 
            lstCtrl->SetItemText(nItemCount, 2, item->csValue); 
            delete item;
        }
        lstCtrl->SetRedraw(TRUE);
    }
    else
    {
        nItemCount = lstCtrl->GetItemCount();
        USES_CONVERSION;  
		lstCtrl->InsertItem(nItemCount, A2W(szCgi.c_str()));
        lstCtrl->SetItemText(nItemCount, 1, _T("null")); 
        lstCtrl->SetItemText(nItemCount, 2, _T("null")); 

    }

}

void CMainDlg::OnFinished(CMainDlg *dlg, QueryManager *qm)
{
    CButton *btnQuery =  (CButton *)dlg->GetDlgItem(IDC_BTN_QUERY); 
    CButton *btnModify = (CButton *)dlg->GetDlgItem(IDC_BTN_MODIFY);
    btnQuery->EnableWindow(TRUE);
    btnModify->EnableWindow(TRUE);

}

void CMainDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_qm->Destory();
	CDialog::OnClose();
}

void CMainDlg::ShowAuth()
{
	int iCount = m_csPassword.GetLength();
	CString csStar, csAuth;
	do
	{
		csStar.Append(_T("*"));
		iCount--;
	} while (iCount > 0);

	csAuth.Format(_T("%s:%d     User:%s     Pass:%s"), m_csIPAddress, m_nPort, m_csUsername, csStar);
	m_txtAuth.SetWindowText(csAuth);
}