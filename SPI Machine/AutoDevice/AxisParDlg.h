#pragma once


// CAxisParDlg dialog

class CAxisParDlg : public CDialog
{
	DECLARE_DYNAMIC(CAxisParDlg)

public:
	CAxisParDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAxisParDlg();

// Dialog Data
	enum { IDD = IDD_AXISPAR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	CComboBox m_StrComboXNo;
	WORD   m_wXAxis;
	double m_dXPulsePer;
	double m_dXPitch;
	double m_dXTrip;
	double m_dXTripNegaTive;
	double m_dXMinVel;
	double m_dXMaxVel;
	double m_dXTAcc;
	double m_dXTDec;
	double m_dXTs;
	CComboBox m_StrComboYNo;
	WORD   m_wYAxis;
	double m_dYPulsePer;
	double m_dYPitch;
	double m_dYTrip;
	double m_dYTripNegaTive;
	double m_dYMinVel;
	double m_dYMaxVel;
	double m_dYTAcc;
	double m_dYTDec;
	double m_dYTs;
	CComboBox m_StrComboZNo;
	WORD   m_wZAxis;
	double m_dZPulsePer;
	double m_dZPitch;
	double m_dZTrip;
	double m_dZTripNegaTive;
	double m_dZMinVel;
	double m_dZMaxVel;
	double m_dZTAcc;
	double m_dZTDec;
	double m_dZTs;
	CComboBox m_StrComboWNo;
	WORD   m_wWAxis;
	double m_dWPulsePer;
	double m_dWPitch;
	double m_dWMinVel;
	double m_dWMaxVel;
	double m_dWTAcc;
	double m_dWTDec;
	double m_dWTs;
	double m_dWTrip;
	double m_dWMinTrip;
	double m_dLx;
	double m_dLy;
	double m_dRx;
	double m_dRy;
	WORD wDirect;
	afx_msg void OnBnClickedBtnLeftupdate();
	afx_msg void OnBnClickedBtnRightupdate();
};
