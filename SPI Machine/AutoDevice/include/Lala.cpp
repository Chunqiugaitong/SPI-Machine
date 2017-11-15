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
	//��ȡ��һ���Ӵ��ڣ�����һ���ؼ�
	pWnd = dlg->GetWindow(GW_CHILD);
	while (NULL != pWnd)
	{
		if(pWnd)//�ж��Ƿ�Ϊ�գ���Ϊ�Ի��򴴽�ʱ����ô˺���������ʱ�ؼ���δ����
		{
			CRect rect;   //��ȡ�ؼ��仯ǰ��С
			pWnd->GetWindowRect(rect);
			dlg->ScreenToClient(rect);//���ؼ���Сת��Ϊ�ڶԻ����е���������
			// cx/m_rect.Width()Ϊ�Ի����ں���ı仯����
			rect.left=rect.left*cx/m_rect.Width();/////�����ؼ���С
			rect.right=rect.right*cx/m_rect.Width();
			rect.top=rect.top*cy/m_rect.Height();
			rect.bottom=rect.bottom*cy/m_rect.Height();

			pWnd->MoveWindow(rect);//���ÿؼ���С

		}

		//��ȡ��һ���ؼ�
		pWnd = pWnd->GetNextWindow();
	}

	dlg->GetClientRect(m_rect);
}
