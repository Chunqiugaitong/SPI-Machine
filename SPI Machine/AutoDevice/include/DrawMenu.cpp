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
	lpMeasureItemStruct->itemHeight = 28;//项高
	lpMeasureItemStruct->itemWidth = 70;//项宽
}

void CNewMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rect = lpDrawItemStruct->rcItem;
	rect.left += 20;
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);
	//dc.FillSolidRect(rect, RGB(0,166,170));
	CFont Font;
	//Font.CreatePointFont(120, _T("Tahoma"));//创建字体
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
	//	dc.FillSolidRect(rect, RGB(80,89,202));//菜单被选中
	//dc.SetTextColor(RGB(10,0,181));//设置文本颜色
	dc.DrawText(*pText1, rect, DT_VCENTER|DT_LEFT|DT_SINGLELINE);
	dc.Detach();
}

void CNewMenu::ChangeMenuItem(CMenu *pMenu)
{
   	int itemCount = pMenu->GetMenuItemCount();
   	for (int i=0; i < itemCount; i++)
   	{
   		UINT itemID = pMenu->GetMenuItemID(i);//获取菜单项ID号
   		pMenu->GetMenuString(i, *pText, MF_BYPOSITION);//获取菜单文本
   
   		//ModifyMenu函数最后一个参数对应DRAWITEMSTRUCT结构里的itemData变量
   		pMenu->ModifyMenu(i,MF_OWNERDRAW|MF_BYPOSITION|MF_STRING,itemID, (LPCTSTR)pText); 
   		//pMenu->ModifyMenu(i,MF_OWNERDRAW|MF_BYPOSITION|MF_STRING,itemID,(LPSTR)pText);
   		if (itemID == -1)//如果是一个弹出式菜单
   		{
   			//ChangeMenuItem(pMenu->GetSubMenu(i));
   		}
   	}  
}