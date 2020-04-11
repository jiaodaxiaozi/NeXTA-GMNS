// DlgLinkProperties.cpp : implementation file
//

#include "stdafx.h"
#include "TLite.h"
#include "DlgLinkProperties.h"


// CDlgLinkProperties dialog

IMPLEMENT_DYNAMIC(CDlgLinkProperties, CDialog)

CDlgLinkProperties::CDlgLinkProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLinkProperties::IDD, pParent)
	, FromNode(0)
	, ToNode(0)
	, LinkLength(0)
	, FreeSpeed(0)
	, FreeFlowTravelTime(0)
	, LaneCapacity(0)
	, nLane(0)
	, m_LinkID(0)
	, StreetName(_T(""))
{
m_bEditChange = false;
}

CDlgLinkProperties::~CDlgLinkProperties()
{
}

void CDlgLinkProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FROM_NODE, FromNode);
	DDX_Text(pDX, IDC_EDIT_To_NODE, ToNode);
	DDX_Text(pDX, IDC_EDIT_LENGTH, LinkLength);
	DDV_MinMaxFloat(pDX, LinkLength, 0.0001f, 10000);
	DDX_Text(pDX, IDC_EDIT_SPEEDLIMIT, FreeSpeed);
	DDV_MinMaxFloat(pDX, FreeSpeed, 5, 120);
	DDX_Text(pDX, IDC_EDIT_FFTT, FreeFlowTravelTime);
	DDX_Text(pDX, IDC_EDIT_LANE_CAPACITY, LaneCapacity);
	DDV_MinMaxInt(pDX, LaneCapacity, 0, 300000);
	DDX_Text(pDX, IDC_EDIT_NUMLANES, nLane);
	DDV_MinMaxInt(pDX, nLane, 0, 10);
	DDX_Control(pDX, IDC_COMBO1, m_LinkTypeComboBox);
	DDX_Text(pDX, IDC_EDIT_LINKID, m_LinkID);
	DDX_Text(pDX, IDC_EDIT_STREET_NAME, StreetName);
	DDV_MaxChars(pDX, StreetName, 100);

}


BEGIN_MESSAGE_MAP(CDlgLinkProperties, CDialog)
	ON_CBN_EDITCHANGE(IDC_COMBO1, &CDlgLinkProperties::OnCbnEditchangeCombo1)
	ON_BN_CLICKED(IDOK, &CDlgLinkProperties::OnBnClickedOk)
	ON_BN_CLICKED(IDSAVEASDEFAULT, &CDlgLinkProperties::OnBnClickedSaveasdefault)
	ON_BN_CLICKED(ID_CANCEL, &CDlgLinkProperties::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CDlgLinkProperties::OnCbnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT_LENGTH, &CDlgLinkProperties::OnEnChangeEditLength)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CDlgLinkProperties::OnBnClickedButtonUpdate)
	ON_EN_CHANGE(IDC_EDIT_STREET_NAME, &CDlgLinkProperties::OnEnChangeEditStreetName)
	ON_EN_CHANGE(IDC_EDIT_SPEEDLIMIT, &CDlgLinkProperties::OnEnChangeEditSpeedlimit)
	ON_EN_CHANGE(IDC_EDIT_NUMLANES, &CDlgLinkProperties::OnEnChangeEditNumlanes)
	ON_EN_CHANGE(IDC_EDIT_LANE_CAPACITY, &CDlgLinkProperties::OnEnChangeEditLaneCapacity)
	ON_EN_CHANGE(IDC_EDIT6, &CDlgLinkProperties::OnEnChangeEdit6)
	ON_EN_CHANGE(IDC_EDIT7, &CDlgLinkProperties::OnEnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT1, &CDlgLinkProperties::OnEnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, &CDlgLinkProperties::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT5, &CDlgLinkProperties::OnEnChangeEdit5)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT8, &CDlgLinkProperties::OnEnChangeEdit8)
	ON_BN_CLICKED(IDC_CHECK2, &CDlgLinkProperties::OnBnClickedCheck2)
END_MESSAGE_MAP()


// CDlgLinkProperties message handlers

BOOL CDlgLinkProperties::OnInitDialog()
{
	CDialog::OnInitDialog();
	std::map<int, DTALinkType>:: const_iterator itr;

	for(itr = m_pDoc->m_LinkTypeMap.begin(); itr != m_pDoc->m_LinkTypeMap.end(); itr++)
	{
	
		CString str;
		str.Format("%d,%s",itr->first,itr->second .link_type_name.c_str ());
		m_LinkTypeComboBox.AddString (str);

		if(LinkType == itr->first)
		{
			m_LinkTypeComboBox.SetCurSel (m_LinkTypeComboBox.GetCount ()-1);
		}
	}

	EnableDataBasedOnLinkType();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgLinkProperties::EnableDataBasedOnLinkType()
{
		int SelectLinkType = m_LinkTypeComboBox.GetCurSel ();

	std::map<int, DTALinkType>:: const_iterator itr;

	int count = 0;
	int link_type = -1;
	for(itr = m_pDoc->m_LinkTypeMap.begin(); itr != m_pDoc->m_LinkTypeMap.end(); itr++)
	{
	
		if(SelectLinkType == count)
		{
		link_type = itr->first;
			break;
		}
		count++;
	}

	if(link_type ==-1)
		return ;
}
void CDlgLinkProperties::OnCbnEditchangeCombo1()
{

}

void CDlgLinkProperties::OnBnClickedOk()
{
	char m_Text[100];

	int CurSelNo = m_LinkTypeComboBox.GetCurSel ();


	char LinkTypeString[50]; 
	
	m_LinkTypeComboBox.GetLBText (CurSelNo,m_Text);
	sscanf_s(m_Text, "%d,%s", &LinkType, LinkTypeString,sizeof(LinkTypeString));
	TRACE("updated link type = %d", LinkType);

	OnOK();
}

void CDlgLinkProperties::OnBnClickedSaveasdefault()
{
	UpdateData(false);

	DefaultSpeedLimit = FreeSpeed;
	DefaultCapacity = LaneCapacity;
	DefaultnLane = nLane;
    DefaultLinkType = LinkType;

}

void CDlgLinkProperties::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgLinkProperties::OnCbnSelchangeCombo1()
{

	UpdateData(true);
}

void CDlgLinkProperties::OnEnChangeEditLength()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnBnClickedButtonUpdate()
{
	UpdateData(1);
	FreeFlowTravelTime = LinkLength / max(0.001,FreeSpeed);
	UpdateData(0);

}

void CDlgLinkProperties::OnEnChangeEditStreetName()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditSpeedlimit()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditNumlanes()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditLaneCapacity()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditSaturationFlowRate()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditEffectiveGreenTime()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit6()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit7()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit1()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit2()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEdit4()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnEnChangeEdit5()
{
	m_bEditChange = true;

}

void CDlgLinkProperties::OnEnChangeEditAadt()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnEnChangeEditPeakhourlyvolume()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnClose()
{
	// TODO: Add your message handler code here and/or call default

//	CDialog::OnClose();

   int nRet = 5; 
   EndDialog(nRet); 

}

void CDlgLinkProperties::OnEnChangeEditNumlanesLeftTurn()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnEnChangeEditNumlanesRightTurn()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnEnChangeEdit8()
{
	m_bEditChange = true;
}

void CDlgLinkProperties::OnBnClickedCheck2()
{
	m_bEditChange = true;

}


