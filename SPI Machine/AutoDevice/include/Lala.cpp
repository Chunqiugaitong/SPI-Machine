#include "StdAfx.h"
#include "Lala.h"

CLala::CLala(void)
{
}

CLala::~CLala(void)
{
}

void CLala::Show(CRect& m_rect, CDialog* dlg, int cx, int cy)
{
	CRect rect2;
	dlg->GetClientRect(&rect2);
	if (0 == rect2.Width())
		return;

	CWnd* pWnd;
	//获取第一个子窗口，即第一个控件
	pWnd = dlg->GetWindow(GW_CHILD);
	while (NULL != pWnd)
	{
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;   //获取控件变化前大小
			pWnd->GetWindowRect(rect);
			dlg->ScreenToClient(rect);//将控件大小转换为在对话框中的区域坐标
			// cx/m_rect.Width()为对话框在横向的变化比例
			rect.left=rect.left*cx/m_rect.Width();/////调整控件大小
			rect.right=rect.right*cx/m_rect.Width();
			rect.top=rect.top*cy/m_rect.Height();
			rect.bottom=rect.bottom*cy/m_rect.Height();

			pWnd->MoveWindow(rect);//设置控件大小

		}

		//获取下一个控件
		pWnd = pWnd->GetNextWindow();
	}

	dlg->GetClientRect(m_rect);
}
