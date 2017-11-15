
// AutoDeviceDoc.h : CAutoDeviceDoc ��Ľӿ�
//

#pragma once
#include "../GxmcCode/GxmcGerber.h"

class CAutoDeviceDoc : public CDocument
{
protected: // �������л�����
	CAutoDeviceDoc();
	DECLARE_DYNCREATE(CAutoDeviceDoc)

// ����
public:
	Gxmc_Gerber::CGxmcGerber m_gxmcGerber;
	vector<CRect> m_vecClusterRect;
	std::map<int,Gxmc_Gerber::PadCluster> m_mapPadCluster;
	RectD m_clustersMargin;
	vector<CGxmcDatabase::Mark> m_vecMark;

	bool m_bIsImported;
// ����
public:
	void GetPadCluster();


// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CAutoDeviceDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnImport();
};
