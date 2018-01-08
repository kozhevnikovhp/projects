// ExTreeCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "ExTreeCtrl.h"
#include "shlobj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl
//
// By Nicola Delfino 1999 delfinon@altavista.net
//
// 
// 
////////////////////////////////////////////////////////////////////////////

CDirTreeCtrl::CDirTreeCtrl()
{
}

CDirTreeCtrl::~CDirTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CDirTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CDirTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirTreeCtrl message handlers

void CDirTreeCtrl::Initialize(bool clear)
{
	LPITEMIDLIST itemlist;
	char buf[300];
	int desktopIcon;
	int mycomputerIcon;
    int ret;

    if (clear)
	    DeleteAllItems();
	SetImageList (NULL,0);

	SHGetSpecialFolderLocation (this->m_hWnd, CSIDL_DESKTOP, &itemlist);
	SHGetPathFromIDList(itemlist, buf ); 
	m_desktop_path=buf;

	m_img.Create( GetSystemMetrics(SM_CXSMICON), 
		          GetSystemMetrics(SM_CYSMICON), 
				  ILC_COLOR24, 50, 50);

	m_img.SetBkColor( GetSysColor(COLOR_WINDOW) );

    CBitmap bmp;
    CBitmap *old;
    CBitmap *newbmp;
    CDC dc;

    dc.CreateCompatibleDC(NULL);
    bmp.CreateBitmap (32,32,1,24,NULL);
    old=dc.SelectObject (&bmp);
    dc.FillSolidRect(0,0,31,31,GetSysColor(COLOR_WINDOW));
    newbmp=dc.SelectObject (old);
    dc.DeleteDC ();
    ret=m_img.Add(newbmp,COLORREF(0x00));


	desktopIcon=    m_img.Add(ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 34));
	mycomputerIcon= m_img.Add(ExtractIcon( AfxGetApp()->m_hInstance, "shell32.dll", 15));

	m_drives_root=AddItem_Core("Drives", TVI_ROOT, true, mycomputerIcon, mycomputerIcon);

	m_desktop_root=AddItem(m_desktop_path, TVI_ROOT, true, desktopIcon, desktopIcon);
}

HTREEITEM CDirTreeCtrl::AddItem(const char * path, HTREEITEM parent, bool isDir, int iconEx, int iconEx2)
{
    SHFILEINFO shinfo, shinfo_sel;

	SHGetFileInfo( path, NULL, 
				   &shinfo, 
				   sizeof(shinfo), 
				   SHGFI_DISPLAYNAME | SHGFI_ICON | SHGFI_SMALLICON);

	SHGetFileInfo( path, NULL, 
				   &shinfo_sel, 
				   sizeof(shinfo_sel), 
				   SHGFI_DISPLAYNAME |  SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON);

	int icon    = iconEx!=-1 ? iconEx : m_img.Add(shinfo.hIcon);
    int iconsel = iconEx2!=-1 ? iconEx2 : m_img.Add(shinfo_sel.hIcon);

	SetImageList (&m_img,LVSIL_NORMAL);

	return AddItem_Core ( shinfo.szDisplayName, parent, isDir, icon, iconsel);
}

void CDirTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TV_ITEM itm= pNMTreeView->itemNew;

	if (itm.state & TVIS_EXPANDED)
		ExpandBranch(itm.hItem);
	else
	{
		DeleteAllChild(itm.hItem);
	}
	
	*pResult = 0;
}

void CDirTreeCtrl::DeleteAllChild(HTREEITEM itm, bool isEmpty)
{
	HTREEITEM child;

	child= GetChildItem(itm);

	while (child)
	{
		int img1, img2;
		
        if (GetChildItem(child))
            DeleteAllChild(GetChildItem(child), isEmpty);

        GetItemImage(child,img1,img2);

        if (img2!=0 && img2!=img1) 
		    {
            m_img.Remove(img2);
            
            RefreshTreeImages(m_drives_root, img2);
            RefreshTreeImages(m_desktop_root, img2);
            }

		if (img1!= 0)
            {
            m_img.Remove(img1);

            RefreshTreeImages(m_drives_root, img1);
            RefreshTreeImages(m_desktop_root, img1);
            }
		
        DeleteItem (child);

        SetImageList(&m_img,TVSIL_NORMAL);

		child= GetChildItem(itm);
	}

	if (isEmpty)
		InsertItem( "",itm);; 
}


void CDirTreeCtrl::ExpandBranch(HTREEITEM parent)
{
    bool bOne=false;

	if(parent==m_drives_root)
	{
		char *ptr;

    	DeleteAllChild(parent,false);
		GetLogicalDriveStrings(500,m_drives_name);

		ptr=m_drives_name;

		while (*ptr)
		{
            bOne=true;
			AddItem(ptr, m_drives_root, true);
			ptr+=strlen(ptr)+1;
		}
	}
	else
	{
        CString fullpath,pathWildcard;

        fullpath=GetPathFromHere(fullpath, parent);
        DeleteAllChild(parent,false);
        pathWildcard=fullpath+"*.*";

        CFileFind finder;
        BOOL bWorking = finder.FindFile(pathWildcard);
        while (bWorking)
            {
            bWorking = finder.FindNextFile();
            if ( finder.IsDirectory() && !finder.IsDots() )
                {
                bOne=true;
                HTREEITEM itm=AddItem (fullpath+finder.GetFileName(), parent );
                InsertItem("",itm);
                }
            }
        
        bWorking = finder.FindFile(pathWildcard);
        while (bWorking)
            {
            bWorking = finder.FindNextFile();
            if ( !finder.IsDirectory() && MatchExtension(finder.GetFileName()) )
                {
                bOne=true;
                AddItem (fullpath+finder.GetFileName(), parent );
                }
            }

	}
    
    if (!bOne) InsertItem( "",parent);
}

HTREEITEM CDirTreeCtrl::AddItem_Core(const char * name, HTREEITEM parent, bool isDir, int icon,int iconOpen)
{
	HTREEITEM hitm;

	hitm=InsertItem( name, icon, iconOpen, parent);
	if (isDir) InsertItem( "",hitm);

	return hitm;
}

CString CDirTreeCtrl::GetPathFromHere(CString s, HTREEITEM itm)
{
    HTREEITEM parent=GetParentItem(itm);
    HTREEITEM itmChild=GetChildItem(itm);

    if (itm==m_drives_root) return "";
        
	if (parent==m_drives_root)
	{
		HTREEITEM child;

		char *ptr=m_drives_name;
		child=GetChildItem(parent);
		
		while (child)
		{
			if (child==itm)
			{
				s=CString(ptr)+s;
				return s;
			}
			ptr+=strlen(ptr)+1;
			child=GetNextItem(child, TVGN_NEXT);
		}
	}
	else if (itm==m_desktop_root)
	{
		s=m_desktop_path+'\\'+s;
		return s;
	}
    else
    {
        if (GetItemText(itm)!= CString(""))
            s=GetItemText(itm)+'\\'+s;

        s=GetPathFromHere(s,parent);
    }


    // remove the last '\' if it isn't a directory
    if (!itmChild && s.GetLength()>0)
        s=s.Left (s.GetLength()-1);

	return s;
}

/* if image number is more than img number -- */
void CDirTreeCtrl::RefreshTreeImages(HTREEITEM item,int img)
{
    int iStandard, iOpen;
    HTREEITEM itmCurrent;
       
    itmCurrent=GetChildItem(item);
    
    while (itmCurrent)
        {
        if (GetItemImage(itmCurrent,iStandard,iOpen))
            {
            if (iStandard>img) iStandard--;
            if (iOpen>img) iOpen--;

            SetItemImage(itmCurrent,iStandard, iOpen);
            }

        if (ItemHasChildren( itmCurrent)!=0)
            {
            RefreshTreeImages(itmCurrent, img);
            }

        itmCurrent=GetNextSiblingItem(itmCurrent);
        }
}

bool CDirTreeCtrl::MatchExtension(CString file)
{
    return file.Right(4)==CString(".exe");
}

CString CDirTreeCtrl::GetFullPath(HTREEITEM item)
{
    if (GetItemText(item)==CString("")) return "";
    else return GetPathFromHere("",item);
}

bool CDirTreeCtrl::SetPath(CString path_in)
{
    char path[MAX_PATH];
    char *delimiters="\\";
    char *token;

    char *pathStart=path;

    HTREEITEM current=m_drives_root;

    strcpy(path,path_in);

    Expand(current,TVE_EXPAND );
    token=strtok (path,delimiters);
    while (token!=NULL)
        {
        current=Dir2Item(current, token);
        if (!current)
            {
            return false;
            }
        
        Expand(current,TVE_EXPAND );
        token=strtok(NULL,delimiters);
        }

    SelectItem(current);
    return true;
}

HTREEITEM CDirTreeCtrl::Dir2Item(HTREEITEM current, char *dir)
{
    if (current==m_drives_root)
        {
        HTREEITEM child;
        CString szDir(dir);

        szDir+="\\";
        szDir.MakeUpper();

		char *ptr=m_drives_name;
		child=GetChildItem(current);
		
		while (child)
		    {
			if (szDir==CString(ptr))
		    	{
				return child;
			    }
			ptr+=strlen(ptr)+1;
			child=GetNextItem(child, TVGN_NEXT);
		    }
        }
    else
        {
        HTREEITEM child;
        CString szChildText;
        CString szDir(dir);

        szDir.MakeUpper();

		child=GetChildItem(current);
		while (child)
		    {
            szChildText=GetItemText(child);
            szChildText.MakeUpper();

			if ( szChildText==szDir)
		    	{
				return child;
			    }
			child=GetNextItem(child, TVGN_NEXT);
		    }
        }

    return NULL;
}

int CDirTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	Initialize();
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CExTreeCtrl

CExTreeCtrl::CExTreeCtrl()
{
}

CExTreeCtrl::~CExTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CExTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CExTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExTreeCtrl::SetNewStyle(long lStyleMask, BOOL bSetBits)
{
	long		lStyleOld;

	lStyleOld = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyleOld &= ~lStyleMask;
	if (bSetBits)
		lStyleOld |= lStyleMask;

	SetWindowLong(m_hWnd, GWL_STYLE, lStyleOld);
	SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

BOOL CExTreeCtrl::IsChildNodeOf(HTREEITEM hItemChild, HTREEITEM hItemSuspectedParent)
{
	do
	{
		if (hItemChild == hItemSuspectedParent)
			break;
	}
	while ((hItemChild = GetParentItem(hItemChild)) != NULL);

	return (hItemChild != NULL);
}

HTREEITEM CExTreeCtrl::GetTopParentItem(HTREEITEM Item)
{
	HTREEITEM ParentItem=Item;
	while (Item)
	{
		ParentItem=GetParentItem(ParentItem);
		if (!ParentItem) break;
		Item=ParentItem;
	}
	return Item;
}

int CExTreeCtrl::GetChildCount(HTREEITEM Item)
{
	if (Item==NULL) return 0;
	int count=0;
	HTREEITEM ChildItem=GetChildItem(Item);
	while (ChildItem)
	{
		count++;
		ChildItem=GetNextItem(ChildItem, TVGN_NEXT);
	}
	return count;
}

HTREEITEM CExTreeCtrl::GetNextSearchItem(HTREEITEM Item)
{
	HTREEITEM NextItem=GetChildItem(Item);
	if (!NextItem)
		NextItem=GetNextSiblingItem(Item);
	if (NextItem)
		return NextItem;
	HTREEITEM ParentItem=Item;
	while (1)
	{
		ParentItem=GetParentItem(ParentItem);
		if (!ParentItem)
			return NULL;
		NextItem=GetNextSiblingItem(ParentItem);
		if (!NextItem) continue;
		if (!GetParentItem(NextItem)) return NULL;
		if (NextItem) return NextItem;
	}
}

HTREEITEM CExTreeCtrl::GetPrevSearchItem(HTREEITEM Item)
{
	HTREEITEM PrevItem=GetPrevSiblingItem(Item);
	if (PrevItem) return PrevItem;
	return GetParentItem(Item);
}

// Определяем, не встречается ли ItemToFind в дереве, если подниматься в нем от ItemFrom
BOOL CExTreeCtrl::FindItemUp(HTREEITEM ItemFrom, HTREEITEM ItemToFind)
{
	HTREEITEM FoundItem=ItemFrom;
	while (FoundItem)
	{
		if (FoundItem==ItemToFind)
			return TRUE;
		FoundItem=GetPrevSearchItem(FoundItem);
	}
	return (FoundItem!=NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CExTreeCtrl message handlers

void CExTreeCtrl::OnEndLabelEdit(LPNMHDR pnmhdr, LRESULT *pLResult)
{
	TV_DISPINFO *pTVinfo = (TV_DISPINFO *)pnmhdr;
	if (pTVinfo->item.pszText != NULL)
	{
		pTVinfo->item.mask = TVIF_TEXT;
		SetItem(&pTVinfo->item);
	}
	*pLResult = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CDragTreeCtrl

CDragTreeCtrl::CDragTreeCtrl()
{
	m_DraggingType=NoDragging;
	m_LastItem=NULL;
	m_pImageList = NULL;
	EnableDragging();
}

CDragTreeCtrl::~CDragTreeCtrl()
{}
 
BEGIN_MESSAGE_MAP(CDragTreeCtrl, CMSelTreeCtrl)
	//{{AFX_MSG_MAP(CDragTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBeginDrag)
	ON_NOTIFY_REFLECT(TVN_BEGINRDRAG, OnBeginRDrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDragTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	HTREEITEM			hitem;
	UINT				flags;

	if (IsDraggingEnabled() && IsDraggingNow() )
	{
		if (m_pImageList)
			m_pImageList->DragMove(point);
		hitem=HitTest(point, &flags);
		HTREEITEM NewItem;
		if (flags & TVHT_ABOVE)
		{
			NewItem=GetPrevSiblingItem(m_LastItem);
			if (!NewItem)
				NewItem=GetParentItem(m_LastItem);
			EnsureVisible(NewItem);
			m_LastItem=NewItem;;
		}
		if (flags & TVHT_BELOW)
		{
			NewItem=GetNextSiblingItem(m_LastItem);
			if (!NewItem)
			{
				NewItem=GetParentItem(m_LastItem);
				NewItem=GetNextSiblingItem(NewItem);
			}
			EnsureVisible(NewItem);
			m_LastItem=NewItem;;
		}
		if (hitem != NULL)
		{
			m_pImageList->DragLeave(this);
			SelectDropTarget(hitem);
			m_hItemDrop = hitem;
			m_pImageList->DragEnter(this, point);
			m_LastItem=hitem;;
		}
	}

	CMSelTreeCtrl::OnMouseMove(nFlags, point);
}

HTREEITEM CDragTreeCtrl::MoveItem(HTREEITEM hItemDrag, HTREEITEM hItemDrop, TypeOfDragging DraggingType)
{
	TypeOfDragging StoredDraddingType=m_DraggingType;
	m_DraggingType=DraggingType;

	SetRedraw(FALSE);
	BeforeMoving(hItemDrag);
	HTREEITEM TransferredItem=TransferItem(hItemDrag, hItemDrop, DraggingType);
	OnItemMoved(hItemDrag, hItemDrop, DraggingType);
	DeleteItem(hItemDrag);
	AfterMoving(TransferredItem);
	SetRedraw(TRUE);
	
	m_DraggingType=StoredDraddingType;
	return TransferredItem;
}

HTREEITEM CDragTreeCtrl::TransferItem(HTREEITEM hItemDrag, HTREEITEM hItemDrop, TypeOfDragging DraggingType)
{
	TV_INSERTSTRUCT		tvstruct;
	TCHAR				sztBuffer[50];
	HTREEITEM			hNewItem, hFirstChild;

	tvstruct.item.hItem = hItemDrag;
	tvstruct.item.cchTextMax = 49;
	tvstruct.item.pszText = sztBuffer;
	tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvstruct.item.stateMask=TVIS_OVERLAYMASK | TVIS_STATEIMAGEMASK;
	GetItem(&tvstruct.item);  // get information of the dragged element
	if (DraggingType==RightDragging)
	{
		tvstruct.hParent = hItemDrop;
		tvstruct.hInsertAfter = TVI_LAST;
	}
	else
	{
		tvstruct.hParent = GetParentItem(hItemDrop);
		tvstruct.hInsertAfter = hItemDrop;//TVI_SORT;
	}
	tvstruct.item.mask = TVIF_IMAGE |TVIF_SELECTEDIMAGE |  TVIF_STATE | TVIF_TEXT| TVIF_PARAM;
	hNewItem = InsertItem(&tvstruct);
	Expand(tvstruct.hParent, TVE_EXPAND);

	while ((hFirstChild = GetChildItem(hItemDrag)) != NULL)
	{
		TransferItem(hFirstChild, hNewItem, RightDragging);  // recursively transfer all the items
		DeleteItem(hFirstChild);		// delete the first child and all its children
	}
	Expand(hNewItem, TVE_COLLAPSE);
	return hNewItem;
}

void CDragTreeCtrl::OnButtonUp()
{
	if (IsDraggingEnabled() && IsDraggingNow())
	{
		BeforeMouseReleased();
		if (m_pImageList)
		{
			m_pImageList->DragLeave(this);
			m_pImageList->EndDrag();
			delete m_pImageList;
		}
		m_pImageList = NULL;

		POSITION pos, pos1;
		m_ItemsToMoveList.RemoveAll(); //На всякий случай
		for (pos=m_SelectionList.GetHeadPosition(); pos; m_SelectionList.GetNext(pos))
		{
			HTREEITEM Item=m_SelectionList.GetAt(pos);
			BOOL bMovingNeed=TRUE;
			for (pos1=m_ItemsToMoveList.GetHeadPosition(); pos1; m_ItemsToMoveList.GetNext(pos1))
			{
				HTREEITEM MayBeParent=m_ItemsToMoveList.GetAt(pos1);
				if (IsChildNodeOf(Item, MayBeParent))
					bMovingNeed=FALSE;
			}
			if (bMovingNeed)
				m_ItemsToMoveList.AddTail(Item);
		}
		if (GetTypeOfDragging()==RightDragging)
		{ // Переворачиваем список пунктов, которые надо переносить, т.к. по правой кнопке добавляются в конец
			pos=m_ItemsToMoveList.GetTailPosition(); 
			while (pos)
			{
				HTREEITEM Item=m_ItemsToMoveList.GetAt(pos);
				pos1=pos;
				m_ItemsToMoveList.GetPrev(pos);
				m_ItemsToMoveList.RemoveAt(pos1);
				m_ItemsToMoveList.AddTail(Item);
			}
		}

		SetRedraw(FALSE);
		for (pos=m_ItemsToMoveList.GetTailPosition(); pos; m_ItemsToMoveList.GetPrev(pos))
		{
			m_hItemDrag=m_ItemsToMoveList.GetAt(pos);
			if (m_hItemDrag == m_hItemDrop
				|| IsChildNodeOf(m_hItemDrop, m_hItemDrag)
				|| (GetParentItem(m_hItemDrag) == m_hItemDrop) && (GetTypeOfDragging()==RightDragging))
			{
				MessageBeep(0);
			}
			else
			{
				if (CanDropItem(m_hItemDrag, m_hItemDrop, GetTypeOfDragging()))
				{
					HTREEITEM hNewItem=MoveItem(m_hItemDrag, m_hItemDrop, GetTypeOfDragging());
					SelectItem(hNewItem);
				}
				else
					MessageBeep(0);
			}
		}
		SetRedraw(TRUE);
		m_SelectionList.RemoveAll();
		m_ItemsToMoveList.RemoveAll();

		ReleaseCapture();
		m_DraggingType=NoDragging;
		SelectDropTarget(NULL);
		AfterMouseReleased();
	}
}

void CDragTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	OnButtonUp();
	CMSelTreeCtrl::OnLButtonUp(nFlags, point);
}

void CDragTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	OnButtonUp();
	CMSelTreeCtrl::OnRButtonUp(nFlags, point);
}

void CDragTreeCtrl::OnBeginDrag(LPNMHDR pnmhdr, LRESULT *pLResult)
{
	if (!IsDraggingEnabled()) return;
	m_DraggingType=LeftDragging;
	BeginDrag();
}

void CDragTreeCtrl::OnBeginRDrag(LPNMHDR pnmhdr, LRESULT *pLResult)
{
	if (!IsDraggingEnabled()) return;
	m_DraggingType=RightDragging;
	BeginDrag();
}

void CDragTreeCtrl::BeginDrag()
{
	if (!IsDraggingEnabled()) return;
	CPoint		ptAction;
	UINT		nFlags;

	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);
//	((CTreeCtrlPage *)GetParent())->ShowNotification(pnmhdr, pLResult);
	//ASSERT(!m_Dragging);
	m_hItemDrag = HitTest(ptAction, &nFlags);
	m_hItemDrop = NULL;
	if (!CanDragItem(m_hItemDrag, GetTypeOfDragging()))
	{
		m_DraggingType=NoDragging;
		return;
	}

	ASSERT(m_pImageList == NULL);
	m_pImageList = CreateDragImage(m_hItemDrag);  // get the image list for dragging
	if (m_pImageList)
	{
		m_pImageList->DragShowNolock(TRUE);
		m_pImageList->SetDragCursorImage(0, CPoint(0, 0));
		m_pImageList->BeginDrag(0, CPoint(0,0));
		m_pImageList->DragMove(ptAction);
		m_pImageList->DragEnter(this, ptAction);
	}
	SetCapture();
}

BOOL CDragTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE
		&& IsDraggingNow())
	{
		m_DraggingType=NoDragging;
		if (m_pImageList)
		{
			m_pImageList->DragLeave(NULL);
			m_pImageList->EndDrag();
			ReleaseCapture();
			SelectDropTarget(NULL);
			delete m_pImageList;
		}
		m_pImageList=NULL;
		SetRedraw(TRUE);
		return TRUE;		// DO NOT process further
	}
	return CMSelTreeCtrl::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CMSelTreeCtrl

CMSelTreeCtrl::CMSelTreeCtrl()
{
	EnableMSelection();
	m_hItemFirstSel=NULL;
}

CMSelTreeCtrl::~CMSelTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CMSelTreeCtrl, CExTreeCtrl)
	//{{AFX_MSG_MAP(CMSelTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSelTreeCtrl message handlers

void CMSelTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Set focus to control if key strokes are needed.
	// Focus is not automatically given to control on lbuttondown
	UINT flag;
	HTREEITEM hItem=HitTest(point, &flag);
	if (!IsMSelectionEnabled())
	{
		m_SelectionList.RemoveAll();
		m_SelectionList.AddTail(hItem);
		CExTreeCtrl::OnLButtonDown(nFlags, point);
		return;
	}

	if(nFlags & MK_CONTROL && (flag & TVHT_ONITEM) ) 
	{
		// Control key is down
		if (hItem)
		{
			// Toggle selection state
			if (IsSelected(hItem))
				RemoveItemFromSelectionList(hItem);
			else
				AddNewSelection(hItem);
			UINT uNewSelState = 
				GetItemState(hItem, TVIS_SELECTED) & TVIS_SELECTED ? 
							0 : TVIS_SELECTED;
            
			// Get old selected (focus) item and state
			HTREEITEM hItemOld = GetSelectedItem();
			UINT uOldSelState  = hItemOld ? 
					GetItemState(hItemOld, TVIS_SELECTED) : 0;
            
			// Select new item
			if (GetSelectedItem()==hItem)
				SelectItem(NULL);		// to prevent edit
			CExTreeCtrl::OnLButtonDown(nFlags, point);

			// Set proper selection (highlight) state for new item
			SetItemState(hItem, uNewSelState,  TVIS_SELECTED);

			// Restore state of old selected item
			if (hItemOld && hItemOld != hItem)
				SetItemState(hItemOld, uOldSelState, TVIS_SELECTED);

			m_hItemFirstSel = NULL;

			return;
		}
	} 
	if (nFlags & MK_SHIFT && (flag & TVHT_ONITEM))
	{
		// Shift key is down
		// Initialize the reference item if this is the first shift selection
		if (!m_hItemFirstSel)
			m_hItemFirstSel=GetSelectedItem();

		// Select new item
		if (GetSelectedItem()==hItem)
			SelectItem(NULL);			// to prevent edit
		CExTreeCtrl::OnLButtonDown(nFlags, point);

		if (m_hItemFirstSel)
		{
			SelectItems(m_hItemFirstSel, hItem);
			return;
		}
	}
	if (flag & TVHT_ONITEM)
	{
		if (!(m_SelectionList.GetCount()>1 && IsSelected(hItem)))
		{
			ClearSelection();
			m_SelectionList.AddTail(hItem);
			RefreshSelection();
		}
		m_hItemFirstSel=NULL;
	}

	CExTreeCtrl::OnLButtonDown(nFlags, point);
}

void CMSelTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (IsMSelectionEnabled())
	{
		if ((nChar==VK_UP || nChar==VK_DOWN) && GetKeyState( VK_SHIFT )&0x8000)
		{
			// Initialize the reference item if this is the first shift selection
			if (!m_hItemFirstSel)
			{
				m_hItemFirstSel=GetSelectedItem();
				ClearSelection();
			}

			// Find which item is currently selected
			HTREEITEM hItemPrevSel=GetSelectedItem();

			HTREEITEM hItemNext;
			if (nChar==VK_UP)
				hItemNext = GetPrevVisibleItem(hItemPrevSel);
			else
				hItemNext = GetNextVisibleItem(hItemPrevSel);

			if (hItemNext)
			{
				// Determine if we need to reselect previously selected item
				BOOL bReselect = 
					!( GetItemState( hItemNext, TVIS_SELECTED ) & TVIS_SELECTED );

				// Select the next item - this will also deselect the previous item
				SelectItem(hItemNext);

				// Reselect the previously selected item
				if (bReselect)
					SetItemState( hItemPrevSel, TVIS_SELECTED, TVIS_SELECTED );
			}
			return;
		}
		else if (nChar >= VK_SPACE)
		{
			m_hItemFirstSel = NULL;
			ClearSelection();
		}
	}

	CExTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMSelTreeCtrl::RemoveItemFromSelectionList(HTREEITEM hItem)
{
	POSITION pos=m_SelectionList.Find(hItem);
	if (pos)
		m_SelectionList.RemoveAt(pos);
}

int CMSelTreeCtrl::CompareItems(HTREEITEM Item1, HTREEITEM Item2)
{ // Item1<Item2 return -1
	// Item1==Item2 return 0
	// Item1>Item2 return +1
	if (Item1==Item2) return 0;
	HTREEITEM Parent1=Item1;
	HTREEITEM Parent2=Item2;
	while (Parent1 || Parent2)
	{
		if (FindItemUp(Parent1, Parent2))
			return -1; 
		if (FindItemUp(Parent2, Parent1))
			return 1;
		if (Parent1)
			Parent1=GetParentItem(Parent1);
		if (FindItemUp(Parent1, Parent2))
			return -1; 
		if (FindItemUp(Parent2, Parent1))
			return 1;
		if (Parent2)
			Parent2=GetParentItem(Parent2);
		if (FindItemUp(Parent1, Parent2))
			return -1; 
		if (FindItemUp(Parent2, Parent1))
			return 1;
	}
	return 0;
}

BOOL CMSelTreeCtrl::AddNewSelection(HTREEITEM Item)
{
	POSITION pos=m_SelectionList.Find(Item);
	if (pos) return TRUE; // Уже в списке
	for (pos=m_SelectionList.GetHeadPosition(); pos; m_SelectionList.GetNext(pos))
	{
		HTREEITEM ItemInList=m_SelectionList.GetAt(pos);
		if (CompareItems(Item, ItemInList)==1)
		{
			m_SelectionList.InsertBefore(pos, Item);
			break;
		}
	}
	if (!pos)
		m_SelectionList.AddTail(Item);
	return TRUE;
}

BOOL CMSelTreeCtrl::DeleteItem(HTREEITEM hItem) // overloaded function
{
	POSITION pos=m_SelectionList.Find(hItem);
	if (pos)
		m_SelectionList.RemoveAt(pos);
	return CExTreeCtrl::DeleteItem(hItem);
}

BOOL CMSelTreeCtrl::DeleteAllItems() // overloaded function
{
	m_SelectionList.RemoveAll();
	return CExTreeCtrl::DeleteAllItems();
}

BOOL CMSelTreeCtrl::SelectItem(HTREEITEM hItem)
{
	if (hItem)
	{
		POSITION pos=m_SelectionList.Find(hItem);
		if (!pos)
			m_SelectionList.AddTail(hItem);
	}

	return CExTreeCtrl::SelectItem(hItem);
}

void CMSelTreeCtrl::ClearSelection()
{
	for (POSITION pos=m_SelectionList.GetHeadPosition(); pos; m_SelectionList.GetNext(pos))
	{
		HTREEITEM Item=m_SelectionList.GetAt(pos);
		SetItemState(Item, 0, TVIS_SELECTED);
	}
	m_SelectionList.RemoveAll();
}

BOOL CMSelTreeCtrl::SelectItems(HTREEITEM hItemFrom, HTREEITEM hItemTo)
{
	HTREEITEM hItem = GetRootItem();

	// Clear selection upto the first item
	while ( hItem && hItem!=hItemFrom && hItem!=hItemTo )
	{
		hItem = GetNextVisibleItem( hItem );
		SetItemState( hItem, 0, TVIS_SELECTED );
	}

	if ( !hItem )
		return FALSE;	// Item is not visible

	SelectItem( hItemTo );

	// Rearrange hItemFrom and hItemTo so that hItemFirst is at top
	if( hItem == hItemTo )
	{
		hItemTo = hItemFrom;
		hItemFrom = hItem;
	}


	// Go through remaining visible items
	BOOL bSelect = TRUE;
	while ( hItem )
	{
		// Select or remove selection depending on whether item
		// is still within the range.
		SetItemState( hItem, bSelect ? TVIS_SELECTED : 0, TVIS_SELECTED );
		if (bSelect)
			AddNewSelection(hItem);

		// Do we need to start removing items from selection
		if( hItem == hItemTo ) 
			bSelect = FALSE;

		hItem = GetNextVisibleItem( hItem );
	}

	return TRUE;
}

HTREEITEM CMSelTreeCtrl::GetFirstSelectedItem()
{
	POSITION pos=m_SelectionList.GetHeadPosition();
	if (!pos) return NULL;
	return m_SelectionList.GetAt(pos);
}

HTREEITEM CMSelTreeCtrl::GetLastSelectedItem()
{
	POSITION pos=m_SelectionList.GetTailPosition();
	if (!pos) return NULL;
	return m_SelectionList.GetAt(pos);
}

HTREEITEM CMSelTreeCtrl::GetNextSelectedItem(HTREEITEM hItem)
{
	POSITION pos=m_SelectionList.Find(hItem);
	if (!pos) return NULL;
	m_SelectionList.GetNext(pos);
	if (!pos) return NULL;
	return m_SelectionList.GetAt(pos);
}

HTREEITEM CMSelTreeCtrl::GetPrevSelectedItem( HTREEITEM hItem )
{
	POSITION pos=m_SelectionList.Find(hItem);
	if (!pos) return NULL;
	m_SelectionList.GetPrev(pos);
	if (!pos) return NULL;
	return m_SelectionList.GetAt(pos);
}

void CMSelTreeCtrl::SetSelection(HTREEITEM Item, BOOL bSelect/*=TRUE*/)
{
	if (bSelect)
		SetItemState(Item, TVIS_SELECTED, TVIS_SELECTED);
	else
		SetItemState(Item, 0, TVIS_SELECTED);
}

BOOL CMSelTreeCtrl::IsSelected(HTREEITEM Item)
{
	return GetItemState(Item, TVIS_SELECTED) & TVIS_SELECTED;
}

void CMSelTreeCtrl::RefreshSelection()
{
	for (POSITION pos=m_SelectionList.GetHeadPosition(); pos; m_SelectionList.GetNext(pos))
	{
		HTREEITEM Item=m_SelectionList.GetAt(pos);
		SetItemState(Item, TVIS_SELECTED, TVIS_SELECTED);
	}
}

void CMSelTreeCtrl::SelectAll()
{
	m_SelectionList.RemoveAll();
	HTREEITEM ChildItem=GetNextItem(NULL, TVGN_CHILD);
	while (ChildItem)
	{
		SetSelection(ChildItem);
		m_SelectionList.AddTail(ChildItem);
		ChildItem=GetNextSiblingItem(ChildItem);
	}
}
