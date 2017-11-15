#include "stdafx.h"
#include <afxwin.h>
#include "DrawMenu.h"


CNewMenu::CNewMenu(void)
{
}

CNewMenu::~CNewMenu(void)
{
	if (pText!=NULL)
	{
		delete pText;
		pText = NULL;
	}
	
}

void CNewMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 28;//���
	lpMeasureItemStruct->itemWidth = 70;//���
}

void CNewMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rect = lpDrawItemStruct->rcItem;
	rect.left += 20;
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	//dc.FillSolidRect(rect, RGB(0,166,170));
	CFont Font;
	//Font.CreatePointFont(120, _T("Tahoma"));//��������
	LOGFONT    lf;
	lf.lfWidth          = 0;
	lf.lfEscapement     = 0;
	lf.lfOrientation    = 0;
	lf.lfHeight         = 150;          // 14 point
	//lf.lfWeight         = FW_BOLD;
	lf.lfWeight         = FW_LIGHT; 
	lf.lfItalic         = FALSE;
	lf.lfUnderline      = FALSE;
	lf.lfStrikeOut      = 0;
	lf.lfCharSet        = ANSI_CHARSET;
	lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lf.lfQuality        = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	lstrcpy(lf.lfFaceName, TEXT("Tahoma"));
	Font.CreatePointFontIndirect(&lf);
	dc.SelectObject(&Font);
	CString *pText1=(CString *)lpDrawItemStruct->itemData;
	//if (lpDrawItemStruct->itemState&ODS_SELECTED)
	//	dc.FillSolidRect(rect, RGB(80,89,202));//�˵���ѡ��
	//dc.SetTextColor(RGB(10,0,181));//�����ı���ɫ
	dc.DrawText(*pText1, rect, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
	dc.Detach();
}

void CNewMenu::ChangeMenuItem(CMenu *pMenu)
{
   	int itemCount = pMenu->GetMenuItemCount();
   	for (int i=0; i < itemCount; i++)
   	{
   		UINT itemID = pMenu->GetMenuItemID(i);//��ȡ�˵���ID��
   		pMenu->GetMenuString(i, *pText, MF_BYPOSITION);//��ȡ�˵��ı�
   
   		//ModifyMenu�������һ��������ӦDRAWITEMSTRUCT�ṹ���itemData����
   		pMenu->ModifyMenu(i,MF_OWNERDRAW|MF_BYPOSITION|MF_STRING,itemID, (LPCTSTR)pText); 
   		//pMenu->ModifyMenu(i,MF_OWNERDRAW|MF_BYPOSITION|MF_STRING,itemID,(LPSTR)pText);
   		if (itemID == -1)//�����һ������ʽ�˵�
   		{
   			//ChangeMenuItem(pMenu->GetSubMenu(i));
   		}
   	}  
}