// PadInspectItemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EditComponent.h"
#include "PadInspectItemDlg.h"
#include "afxdialogex.h"

// CPadInspectItemDlg dialog

IMPLEMENT_DYNAMIC(CPadInspectItemDlg, CDialogEx)

CPadInspectItemDlg::CPadInspectItemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPadInspectItemDlg::IDD, pParent)
{
	m_fStencilThickness = 150.00;
	m_fHeightUp = 150;
	m_fHeightDown = 50;
	m_fAreaUp = 150;
	m_fAreaDown = 50;
	m_fVolumeUp = 150;
	m_fVolumeDown = 50;

	m_fXOffsetUp = 200;
	m_fYOffsetUp = 200;
	m_fPasteStyleUp = 80;

	m_fShortCircuitWidth = 42;
	m_fShortCircuitHeight = 45;
 	m_fShortCircuitGap = 5;

 	m_nElementIndex = 0;
 	m_nPinIndex = 0;
 	m_fPackageWidth = 10;

	m_fPadHeight = 15;
}

CPadInspectItemDlg::~CPadInspectItemDlg()
{
}

void CPadInspectItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX,IDC_EDIT_STENCIL_THICKNESS,m_fStencilThickness);
	DDX_Text(pDX,IDC_EDIT_HEIGHT_UP,m_fHeightUp);
	DDX_Text(pDX,IDC_EDIT_HEIGHT_DOWN,m_fHeightDown);
	DDX_Text(pDX,IDC_EDIT_AREA_UP,m_fAreaUp);
	DDX_Text(pDX,IDC_EDIT_AREA_DOWN,m_fAreaDown);
	DDX_Text(pDX,IDC_EDIT_VOLUME_UP,m_fVolumeUp);
	DDX_Text(pDX,IDC_EDIT_VOLUME_DOWN,m_fVolumeDown);
	DDX_Text(pDX,IDC_EDIT_X_OFFSET_UP,m_fXOffsetUp);
	DDX_Text(pDX,IDC_EDIT_Y_OFFSET_UP,m_fYOffsetUp);
	DDX_Text(pDX,IDC_EDIT_PASTE_UP,m_fPasteStyleUp);
	DDX_Text(pDX,IDC_EDIT_SHORTCIRCUIT_WIDTH,m_fShortCircuitWidth);
	DDX_Text(pDX,IDC_EDIT_SHORTCIRCUIT_HEIGHT,m_fShortCircuitHeight);
	DDX_Text(pDX,IDC_EDIT_SHORTCIRCUIT_GAP,m_fShortCircuitGap);
	DDX_Text(pDX,IDC_EDIT_ELEMENT_INDEX,m_nElementIndex);
	DDX_Text(pDX,IDC_EDIT_PIN_INDEX,m_nPinIndex);
	DDX_Text(pDX,IDC_EDIT_PACKAGE_WIDTH,m_fPackageWidth);
	DDX_Text(pDX,IDC_EDIT_PAD_HEIGHT,m_fPadHeight);

	DDX_Control(pDX,IDC_CHECK_HEIGHT_SIZE,m_btnCheckHeightSize);
	DDX_Control(pDX,IDC_CHECK_AREA_SIZE,m_btnCheckAreaSize);
	DDX_Control(pDX,IDC_CHECK_VOLUME,m_btnCheckVolume);
	DDX_Control(pDX,IDC_CHECK_OFFSET,m_btnCheckOffset);
	DDX_Control(pDX,IDC_CHECK_PASTESTYLE,m_btnCheckPasteStyle);
	DDX_Control(pDX,IDC_CHECK_SHORT_CIRCUIT,m_btnCheckShortCircuit);
	DDX_Control(pDX,IDC_CHECK_SHAPE,m_btnCheckShape);
	DDX_Control(pDX,IDC_CHECK_PACKAGESTYLE,m_btnCheckPackageStyle);
	
}

BEGIN_MESSAGE_MAP(CPadInspectItemDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OK, &CPadInspectItemDlg::OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CPadInspectItemDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_SELECTALL, &CPadInspectItemDlg::OnBnClickedButtonSelectall)
	ON_BN_CLICKED(IDC_BUTTON_RESTOREDEFAULT, &CPadInspectItemDlg::OnBnClickedButtonRestoredefault)

	ON_BN_CLICKED(IDC_RADIO_SINGLE_PAD, &CPadInspectItemDlg::OnBnClickedRadioSinglePad)
	ON_BN_CLICKED(IDC_RADIO_SELECTED_PADS, &CPadInspectItemDlg::OnBnClickedRadioSelectedPads)
	ON_BN_CLICKED(IDC_RADIO_ALL_PADS, &CPadInspectItemDlg::OnBnClickedRadioAllPads)
END_MESSAGE_MAP()

vector<RectF> g_vecSelectPad;
// CPadInspectItemDlg message handlers
 void CPadInspectItemDlg::OnOK()
{
	CDialogEx::OnOK();
}
 void CPadInspectItemDlg::OnCancel()
{
	CDialogEx::OnCancel();
}

 CString g_ProjectPathName;
 //焊盘检测参数设置完成，修改数据库表格中对应字段的值
void CPadInspectItemDlg::OnBnClickedButtonOk()
{
 	try
 	{
 		m_db.ConnectAdo(g_ProjectPathName);
 		CString strKeyValue;
 		if (g_vecSelectPad.size() < 1)
 		{
 			AfxMessageBox(L"请先选择焊盘！");
 			OnOK();
			return;
 		}
 		UpdateData(false);
 		for (int i = 0; i < g_vecSelectPad.size(); i++)
 		{
 			strKeyValue.Format(L"%d",i);
 			//面积、高度、体积、偏移
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.CheckArea,Int2CString(m_btnCheckAreaSize.GetCheck()));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.CheckHeight,Int2CString(m_btnCheckHeightSize.GetCheck()));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.CheckOffset,Int2CString(m_btnCheckOffset.GetCheck()));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.CheckVol,Int2CString(m_btnCheckVolume.GetCheck()));
 			//锡型，拉尖
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.CheckCoplanarity,Int2CString(m_btnCheckPasteStyle.GetCheck()));
 			//短路检测
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.CheckBridge,Int2CString(m_btnCheckShortCircuit.GetCheck()));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.CheckShape,Int2CString(m_btnCheckShape.GetCheck()));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.Check,Int2CString(m_btnCheckPackageStyle.GetCheck()));
 		
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.AreaL,Int2CString(m_fAreaDown));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.AreaU,Int2CString(m_fAreaUp));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.HeightL,Int2CString(m_fHeightDown));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.HeightU,Int2CString(m_fHeightUp));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.VolL,Int2CString(m_fVolumeDown));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.VolU,Int2CString(m_fVolumeUp));
 
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.ShiftXU,Int2CString(m_fXOffsetUp));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.ShiftYU,Int2CString(m_fYOffsetUp));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.ShapeDeltaU,Int2CString(m_fPasteStyleUp));//锡型高度上限
 	//		m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.OffsetMark,Int2CString(m_btnCheckShape.GetCheck()));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.StencilHeight,Int2CString(m_fStencilThickness));
 
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.BridgeHeight,Int2CString(m_fShortCircuitHeight));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.BridgeLength,Int2CString(m_fShortCircuitGap));
 			m_db.SetItemByKey(L"Pad",CGxmcDatabase::s_padElement.ID,strKeyValue,CGxmcDatabase::s_padElement.BridgeWidth,Int2CString(m_fShortCircuitWidth));
 		}
 	}
 	catch(_com_error e)
 	{
        AfxMessageBox(e.ErrorMessage());
 	}
 	
	OnOK();
}

CString CPadInspectItemDlg::Int2CString(int nValue)
{
	CString strFieldValue;
	strFieldValue.Format(L"%d",nValue);
	return strFieldValue;
}
//取消焊盘检测参数的当前设置，关闭对话框
void CPadInspectItemDlg::OnBnClickedButtonCancel()
{
	OnCancel();
}

//全选
void CPadInspectItemDlg::OnBnClickedButtonSelectall()
{
	// TODO: Add your control notification handler code here
	m_btnCheckHeightSize.SetCheck(1);
	m_btnCheckAreaSize.SetCheck(1);
	m_btnCheckVolume.SetCheck(1);
	m_btnCheckOffset.SetCheck(1);
	m_btnCheckPasteStyle.SetCheck(1);
	m_btnCheckShortCircuit.SetCheck(1);
	m_btnCheckShape.SetCheck(1);
	m_btnCheckPackageStyle.SetCheck(1);
}
//恢复默认
void CPadInspectItemDlg::OnBnClickedButtonRestoredefault()
{
	// TODO: Add your control notification handler code here
	m_btnCheckHeightSize.SetCheck(1);
	m_btnCheckAreaSize.SetCheck(1);
	m_btnCheckVolume.SetCheck(1);
	m_btnCheckOffset.SetCheck(1);
	m_btnCheckPasteStyle.SetCheck(1);
	m_btnCheckShortCircuit.SetCheck(0);
	m_btnCheckShape.SetCheck(0);
	m_btnCheckPackageStyle.SetCheck(0);
}

#pragma region 单选框
void CPadInspectItemDlg::OnBnClickedRadioSinglePad()
{
	// TODO: Add your control notification handler code here
}

void CPadInspectItemDlg::OnBnClickedRadioSelectedPads()
{
	// TODO: Add your control notification handler code here
}

void CPadInspectItemDlg::OnBnClickedRadioAllPads()
{
	// TODO: Add your control notification handler code here
}
#pragma endregion 
