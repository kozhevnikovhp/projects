#ifndef ITBROWSEFORFOLDERDIALOG_INCLUDED
#define	ITBROWSEFORFOLDERDIALOG_INCLUDED

#ifndef _SHLOBJ_H_
#include <shlobj.h>
#endif
/////////////////////////////////////////////////////////////////////////////
// ITCBrowseForFolderDialog dialog
//
// @doc EXTERNAL DIALOG
//
class /*ITC_CLASS_DECL*/ ITCBrowseForFolderDialog : public CWnd
{
	DECLARE_DYNAMIC(ITCBrowseForFolderDialog)

// @access Data members
public:
	// @cmember The Windows <t BROWSEINFO> structure. Provides
	// access to basic browse dialog parameters.
	BROWSEINFO m_bi;

// @access Construction
public:
	// @cmember Constructs a <c ITCBrowseForFolderDialog> object.
	ITCBrowseForFolderDialog(CWnd* pParentWnd,
		LPCTSTR lpszInstructions = NULL,
		LPCTSTR lpszFolder = NULL,
		LPCTSTR lpszCaption = NULL,
		LPCTSTR lpszRootFolder = NULL,
		UINT ulFlags = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS);

// @access Operations
public:
	// @cmember Displays the dialog box and returns when done.
	virtual int DoModal();

	// @cmember Enables the OK button.
	void EnableOK(BOOL bEnable);

	// @cmember Returns the display name of the folder selected by the user.
	LPCSTR GetFolder() const;

	// @cmember Returns the item identifier list of the folder
	// selected by the user.
	LPITEMIDLIST GetFolderIdentifier() const;

	// @cmember Sets the initially selected folder.
	void SetFolder(LPCSTR lpszFolder);

	// @cmember Selects the specified folder.
	void SetSelection(LPCTSTR lpsz);
	void SetSelection(LPITEMIDLIST pidl);

	// @cmember Sets the status text.
	void SetStatusText(LPCTSTR lpsz);

// @access Overridables
public:
	// @cmember Called to handle browse dialog events.
	virtual void OnBrowseEvent(UINT uMsg, LPARAM lParam);

	// @cmember Called to handle the <m BFFM_INITIALIZED> browse event.
	virtual void OnInitialized();

	// @cmember Called to handle the <m BFFM_SELCHANGED> browse event.
	virtual void OnSelChanged(LPITEMIDLIST pidl);

// Overrides
public:
	//{{AFX_VIRTUAL(ITCBrowseForFolderDialog)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~ITCBrowseForFolderDialog();

protected:
	UINT m_ulFlags;
	CWnd* m_pParentWnd;
	char m_szFolder[MAX_PATH];
	LPCTSTR m_lpszCaption;
	LPCTSTR m_lpszRootFolder;
	LPCTSTR m_lpszInstructions;
	LPMALLOC m_pMalloc;
	LPITEMIDLIST m_pidlRoot;
	LPITEMIDLIST m_pidlFolder;

	static int CALLBACK BrowseCallbackProc(HWND hWnd,
		UINT uMsg, LPARAM lParam, LPARAM lpData);

// Generated message map functions
protected:
	//{{AFX_MSG(ITCBrowseForFolderDialog)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // ITBROWSEFORFOLDERDIALOG_INCLUDED
