#pragma once

#include "../Common/GxmcDatabase.h"
// CPadInspectItemDlg dialog

class CPadInspectItemDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPadInspectItemDlg)

public:
	CPadInspectItemDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPadInspectItemDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PADINSPECTITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonSelectall();
	afx_msg void OnBnClickedButtonRestoredefault();

	afx_msg void OnBnClickedRadioSinglePad();
	afx_msg void OnBnClickedRadioSelectedPads();
	afx_msg void OnBnClickedRadioAllPads();
private:
	////焊盘检测项目相关参数变量
	float m_fStencilThickness;
	float m_fHeightUp;
	float m_fHeightDown;
	float m_fAreaUp;
	float m_fAreaDown;
	float m_fVolumeUp;
	float m_fVolumeDown;
	float m_fXOffsetUp;
	float m_fYOffsetUp;
	float m_fPasteStyleUp;
	float m_fShortCircuitWidth;
	float m_fShortCircuitHeight;
	float m_fShortCircuitGap;
	int m_nElementIndex;
	int m_nPinIndex;
	float m_fPackageWidth;
	float m_fPadHeight;
	//焊盘检测项目复选框按钮
	CButton m_btnCheckHeightSize;
	CButton m_btnCheckAreaSize;
	CButton m_btnCheckVolume;
	CButton m_btnCheckOffset;
	CButton m_btnCheckPasteStyle;
	CButton m_btnCheckShortCircuit;
	CButton m_btnCheckShape;
	CButton m_btnCheckPackageStyle; 

	//数据库对象
	CGxmcDatabase m_db;
	
public:
	
private:
	CString Int2CString(int nValue);
};
