#if !defined(AFX_AUTOREPEATBITMAPBUTTON_H__3F2B6CC1_2415_11D3_ABE9_008048EEE0E8__INCLUDED_)
#define AFX_AUTOREPEATBITMAPBUTTON_H__3F2B6CC1_2415_11D3_ABE9_008048EEE0E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoRepeatBitmapButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAutoRepeatBitmapButton window

class CAutoRepeatBitmapButton : public CBitmapButton
{
// Construction
public:
	CAutoRepeatBitmapButton();

// Attributes
public:

// Operations
public:
	inline void SetRepeatDelay(UINT RepeatDelay) { m_RepeatDelay=RepeatDelay; }
	inline void EnableRepeating(BOOL bEnable) { m_bRepeatEnabled=bEnable; }
	inline void SetAutoRepeatMethod(void (*pfnAutoRepeatMethod)(void *))
	{
		m_pfnAutoRepeatMethod=pfnAutoRepeatMethod;
	}

// Overrides
	//{{AFX_VIRTUAL(CAutoRepeatBitmapButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAutoRepeatBitmapButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAutoRepeatBitmapButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	void (*m_pfnAutoRepeatMethod)(void *);
	UINT m_RepeatDelay;
	BOOL m_bRepeatEnabled;
	DWORD m_TimerID;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOREPEATBITMAPBUTTON_H__3F2B6CC1_2415_11D3_ABE9_008048EEE0E8__INCLUDED_)
