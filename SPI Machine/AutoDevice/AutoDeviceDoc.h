
// AutoDeviceDoc.h : CAutoDeviceDoc 类的接口
//

#pragma once
#include "../GxmcCode/GxmcGerber.h"

class CAutoDeviceDoc : public CDocument
{
protected: // 仅从序列化创建
	CAutoDeviceDoc();
	DECLARE_DYNCREATE(CAutoDeviceDoc)

// 特性
public:
	Gxmc_Gerber::CGxmcGerber m_gxmcGerber;
	vector<CRect> m_vecClusterRect;
	std::map<int,Gxmc_Gerber::PadCluster> m_mapPadCluster;
	RectD m_clustersMargin;
	vector<CGxmcDatabase::Mark> m_vecMark;

	bool m_bIsImported;
// 操作
public:
	void GetPadCluster();


// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CAutoDeviceDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnImport();
};
