
#include "stdafx.h"

#include "ITBrowseForFolderDialog.h"
#include "Macros.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ITCBrowseForFolderDialog helpers

// @func Converts a file path into an item identifier.
//
// @parm The path of the file to convert.
//
// @rdesc If successful, an item identifier list that specifies the
// relative path, from the desktop folder, that corresponds to the
// specified file path. It contains only one <t SHITEMID> structure
// followed by a terminating zero; NULL otherwise.
//
// @comm The caller is responsible for freeing the returned item identifier
// list using the shell’s task allocator.
static LPITEMIDLIST PathToIDL(LPCTSTR lpszPath)
{
	LPITEMIDLIST pidl = NULL; // Assume error

	if (lpszPath)
	{
		LPSHELLFOLDER pShellFolder = NULL;
		HRESULT hr = ::SHGetDesktopFolder(&pShellFolder);
		ASSERT(SUCCEEDED(hr));

		if (SUCCEEDED(hr))
		{
			// ParseDisplayName wants UNICODE strings.
			OLECHAR szDisplayName[MAX_PATH];
			::ZeroMemory(szDisplayName, sizeof(szDisplayName));
			VERIFY(::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
				lpszPath, -1, szDisplayName, CountOf(szDisplayName)));

			ULONG chEaten = 0;
			ULONG dwAttributes = 0;
			
			hr = pShellFolder->ParseDisplayName(NULL, NULL, szDisplayName,
				&chEaten, &pidl, &dwAttributes);

			if (FAILED(hr))
			{
				pidl = NULL;
			}

			pShellFolder->Release();
		}
	}

	return pidl;
}

/////////////////////////////////////////////////////////////////////////////
// ITCBrowseForFolderDialog

BEGIN_MESSAGE_MAP(ITCBrowseForFolderDialog, CWnd)
	//{{AFX_MSG_MAP(ITCBrowseForFolderDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ITCBrowseForFolderDialog construction

// @mfunc Constructs a <c ITCBrowseForFolderDialog> object.
//
// @parm CWnd* | pParentWnd | The parent window of the dialog box.
// @parmopt LPCTSTR | lpszInstructions | NULL | Pointer to a null-terminated
// string that is displayed above the tree view control in the dialog box.
// This string can be used to specify instructions to the user.
// @parmopt LPCTSTR | lpszFolder | NULL | The initial folder to select,
// can be NULL.
// @parmopt LPCTSTR | lpszCaption | NULL | The caption for the dialog box.
// @parmopt LPCTSTR | lpszRootFolder | NULL | Specifies the path of the
// "root" folder to browse from. Only the specified folder and its subfolders
// appear in the dialog box. If NULL, the name space root (the desktop folder)
// is used.
// @parmopt UINT | ulFlags | BIF_RETURNFSANCESTORS BIF_RETURNONLYFSDIRS |
// Value specifying the types of folders to be listed in the dialog box as
// well as other options. See the Win32 SDK documentation on the <t BROWSEINFO>
// structure for more information.
//
// @xref <c ITCBrowseForFolderDialog>
ITCBrowseForFolderDialog::ITCBrowseForFolderDialog(CWnd* pParentWnd,
	LPCTSTR lpszInstructions, LPCTSTR lpszFolder, LPCTSTR lpszCaption,
	LPCTSTR lpszRootFolder, UINT ulFlags)
	: m_lpszCaption(lpszCaption), m_lpszInstructions(lpszInstructions),
	m_lpszRootFolder(lpszRootFolder), m_ulFlags(ulFlags),
	m_pParentWnd(pParentWnd)
{
	if (!pParentWnd)
		pParentWnd = AfxGetMainWnd();
	ASSERT_VALID(pParentWnd);

	m_pMalloc = NULL;
	m_pidlFolder = NULL;

	VERIFY(SUCCEEDED(::SHGetMalloc(&m_pMalloc)));

	// Don't create a PIDL we can't free.
	m_pidlRoot = (m_pMalloc) ? PathToIDL(m_lpszRootFolder) : NULL;

	SetFolder(lpszFolder);

	::ZeroMemory(&m_bi, sizeof(m_bi));

	m_bi.hwndOwner = m_pParentWnd->GetSafeHwnd();
	m_bi.pidlRoot = m_pidlRoot;
	m_bi.pszDisplayName = m_szFolder;
	m_bi.lpszTitle = m_lpszInstructions;
	m_bi.ulFlags = m_ulFlags;
	m_bi.lpfn = BrowseCallbackProc;
	m_bi.lParam = (LPARAM)this;
}

// @mfunc Destroys a <c ITCBrowseForFolderDialog> object.
//
// @xref <c ITCBrowseForFolderDialog>
ITCBrowseForFolderDialog::~ITCBrowseForFolderDialog()
{
	if (m_pidlRoot)
		m_pMalloc->Free(m_pidlRoot);
	m_pidlRoot = NULL;

	if (m_pidlFolder)
		m_pMalloc->Free(m_pidlFolder);
	m_pidlFolder = NULL;

	if (m_pMalloc)
		m_pMalloc->Release();
	m_pMalloc = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// ITCBrowseForFolderDialog operations

// @mfunc Call this method to display the browse dialog box and allow the
// user to browse folders.
//
// If you want to initialize the various browse options by setting members
// of the <md ITCBrowseForFolderDialog::m_bi> structure, you should do this
// before calling <mf ITCBrowseForFolderDialog::DoModal>, but after the
// dialog object is constructed.
//
// When the user clicks the dialog box's OK or Cancel buttons, or selects
// the Close option from the dialog box's control menu, control is returned
// to your application. You can then call other member functions to retrieve
// the settings or information the user inputs into the dialog box.
//
// @rdesc IDOK or IDCANCEL if the function is successful; otherwise -1.
// IDOK and IDCANCEL are constants that indicate whether the user selected
// the OK or Cancel button.
//
// @xref <c ITCBrowseForFolderDialog>
int ITCBrowseForFolderDialog::DoModal()
{
	ASSERT_VALID(this);

	int nModalResult = -1; // Assume error

	if (!m_pMalloc)
	{
		TRACE("DoModal failed because ::SHGetMalloc failed.\n");
		return nModalResult;
	}

	if (m_pidlFolder)
		m_pMalloc->Free(m_pidlFolder);

	m_pidlFolder = ::SHBrowseForFolder(&m_bi);
	if (m_pidlFolder)
	{
		char szFolder[MAX_PATH];
		VERIFY(::SHGetPathFromIDList(m_pidlFolder, szFolder));
		SetFolder(szFolder);

		nModalResult = IDOK;
	}
	else
	{
		nModalResult = IDCANCEL;
	}

	return nModalResult;
}

// @mfunc Enables the OK button.
//
// You can only call this method after
// <mf ITCBrowseForFolderDialog::OnInitialized> has been called.
//
// @parm Nonzero to enable the button; otherwise 0.
//
// @xref <c ITCBrowseForFolderDialog>
void ITCBrowseForFolderDialog::EnableOK(BOOL bEnable)
{
	SendMessage(BFFM_ENABLEOK, bEnable);
}

// @mfunc Selects the specified folder.
//
// @syntax void SetSelection(LPCTSTR lpsz);
// @syntax void SetSelection(LPITEMIDLIST pidl);
//
// @parm LPCTSTR | lpsz | The path of the folder.
// @parm LPITEMIDLIST | pidl | The item identifier list of the folder.
//
// @xref <c ITCBrowseForFolderDialog>
void ITCBrowseForFolderDialog::SetSelection(LPCTSTR lpsz)
{
	SendMessage(BFFM_SETSELECTION, TRUE, (LPARAM)lpsz);
}

void ITCBrowseForFolderDialog::SetSelection(LPITEMIDLIST pidl)
{
	SendMessage(BFFM_SETSELECTION, FALSE, (LPARAM)pidl);
}

// @mfunc Sets the status text.
//
// @parm The status text.
//
// @xref <c ITCBrowseForFolderDialog>
void ITCBrowseForFolderDialog::SetStatusText(LPCTSTR lpsz)
{
	SendMessage(BFFM_SETSTATUSTEXT, 0, (LPARAM)lpsz);
}

// @mfunc Call this method to set the initially selected folder.
// The path can have the format C:\Folder\Folder or \\\\UNC\Folder\Folder.
//
// @parm The display name of the folder to select or NULL if no folder
// should be selected.
//
// @xref <c ITCBrowseForFolderDialog> <mf ITCBrowseForFolderDialog::GetFolder>
void ITCBrowseForFolderDialog::SetFolder(LPCSTR lpszFolder)
{
	::ZeroMemory(m_szFolder, CountOf(m_szFolder));

	if (lpszFolder)
	{
		_tfullpath(m_szFolder, lpszFolder, CountOf(m_szFolder));
	}

	// Make all paths consistent.
	const int nLength = lstrlen(m_szFolder)-1;
	if ((nLength > 0) && (m_szFolder[nLength] == '\\'))
		m_szFolder[nLength] = '\0';
}

// @mfunc Call this method to get a pointer to the display name of
// the folder selected by the user. The returned folder will not
// have the format C:\Folder\Folder or \\\\UNC\Folder\Folder.
//
// @rdesc The display name of the folder selected by the user or
// NULL if no folder was selected.
//
// @xref <c ITCBrowseForFolderDialog> <mf ITCBrowseForFolderDialog::SetFolder>
LPCSTR ITCBrowseForFolderDialog::GetFolder() const
{
	return (m_szFolder[0]) ? m_szFolder : NULL;
}

// @mfunc Call this method to get the item identifier list of the folder
// selected by the user.
//
// @rdesc Returns a pointer to the item identifier list that specifies the
// location of the selected folder relative to the root of the name space.
//
// @comm Do not free the returned pointer using the shell’s task allocator.
//
// @xref <c ITCBrowseForFolderDialog>
LPITEMIDLIST ITCBrowseForFolderDialog::GetFolderIdentifier() const
{
	return m_pidlFolder;
}

/////////////////////////////////////////////////////////////////////////////
// ITCBrowseForFolderDialog overridables

// @mfunc Called to handle the <m BFFM_INITIALIZED> browse event.
// The default implementation selects the default folder and changes
// the dialog box window caption.
//
// @xref <c ITCBrowseForFolderDialog>
void ITCBrowseForFolderDialog::OnInitialized()
{
	LPCSTR lpszFolder = GetFolder();
	if (lpszFolder)
	{
		SetSelection(lpszFolder);
	}

	if (m_lpszCaption)
	{
		SetWindowText(m_lpszCaption);
	}
}

// @mfunc Called to handle the <m BFFM_SELCHANGED> browse event.
// The default implementation does nothing.
//
// @parm The item identifier list of the new selection.
//
// @xref <c ITCBrowseForFolderDialog>
void ITCBrowseForFolderDialog::OnSelChanged(LPITEMIDLIST pidl)
{
	UNUSED_ALWAYS(pidl);
}

// @mfunc Called to handle browse events.
//
// @parm Value identifying the event.
// @parm A message specific value.
//
// @comm See the Win32 SDK documentation on the <t BROWSEINFO>
// structure for more information on the parameters to this method.
//
// @xref <c ITCBrowseForFolderDialog>
void ITCBrowseForFolderDialog::OnBrowseEvent(UINT uMsg, LPARAM lParam)
{
	switch (uMsg)
	{
	case BFFM_INITIALIZED:
		OnInitialized();
		break;
	case BFFM_SELCHANGED :
		OnSelChanged((LPITEMIDLIST)lParam);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// ITCBrowseForFolderDialog implementation

int CALLBACK ITCBrowseForFolderDialog::BrowseCallbackProc(
	HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	ASSERT(hWnd);
	ASSERT(lpData != NULL);

	ITCBrowseForFolderDialog* pThis =
		DYNAMIC_DOWNCAST(ITCBrowseForFolderDialog, (CObject*)lpData);
	ASSERT(pThis != NULL);

	if (pThis->GetSafeHwnd() == NULL)
	{
		VERIFY(pThis->SubclassWindow(hWnd));
	}

	ASSERT(pThis->GetSafeHwnd() == hWnd);

	// Forward to dialog object.
	if (pThis)
	{
		pThis->OnBrowseEvent(uMsg, lParam);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(ITCBrowseForFolderDialog, CWnd)

