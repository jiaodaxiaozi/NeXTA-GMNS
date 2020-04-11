#pragma once


// CDlg_Find_Agent dialog

class CDlg_Find_Agent : public CDialog
{
	DECLARE_DYNAMIC(CDlg_Find_Agent)

public:
	CDlg_Find_Agent(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_Find_Agent();

// Dialog Data
	enum { IDD = IDD_DIALOG_Find_Agent };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_AgentID;
};
