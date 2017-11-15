#pragma once

class CNewMenu : public CMenu
{
	// Construction
public:
	CNewMenu();
	virtual ~CNewMenu();

public:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void ChangeMenuItem(CMenu *pMenu);
	CString *pText;
protected:
};
