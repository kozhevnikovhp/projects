; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CLithoFrame
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "litho.h"
LastPage=0

ClassCount=6
Class1=CLithoApp
Class2=CAboutDlg
Class3=CLithoDoc
Class4=CLithoFrame
Class5=CLithoView
Class6=CSettingsDialog

ResourceCount=4
Resource1=IDR_MAINFRAME (English (U.S.))
Resource2=IDD_ABOUTBOX (English (U.S.))
Resource3=CG_IDD_PROGRESS
Resource4=IDD_SETTINGS_DIALOG

[CLS:CLithoApp]
Type=0
BaseClass=CWinApp
HeaderFile=Litho.h
ImplementationFile=Litho.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=Litho.cpp
ImplementationFile=Litho.cpp
LastObject=CAboutDlg

[CLS:CLithoDoc]
Type=0
BaseClass=CDocument
HeaderFile=LithoDoc.h
ImplementationFile=LithoDoc.cpp

[CLS:CLithoFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=LithoFrame.h
ImplementationFile=LithoFrame.cpp
Filter=T
VirtualFilter=fWC
LastObject=ID_VIEW_MASK

[CLS:CLithoView]
Type=0
BaseClass=COpenGLView
HeaderFile=LithoView.h
ImplementationFile=LithoView.cpp

[CLS:CSettingsDialog]
Type=0
BaseClass=CDialog
HeaderFile=SettingsDialog.h
ImplementationFile=SettingsDialog.cpp
LastObject=ID_VIEW_MASK_GRID
Filter=D
VirtualFilter=dWC

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_SETTINGS_DIALOG]
Type=1
Class=CSettingsDialog
ControlCount=16
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_WAVELENGTH_EDIT,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_DISTANCE_EDIT,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_MASK_SIZE_EDIT,edit,1350631552
Control9=IDC_POSITIVE_MASK_CHECK,button,1342242819
Control10=IDC_STATIC,static,1342308352
Control11=IDC_IMAGE_ACCURACY_EDIT,edit,1350631552
Control12=IDC_STATIC,button,1342177287
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,static,1342308352
Control16=IDC_IMAGE_SIZE_EDIT,edit,1350631552

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_EDIT_COPY
Command2=ID_EDIT_CUT
Command3=ID_EDIT_PASTE
Command4=ID_BUTTON32792
Command5=ID_BUTTON32793
Command6=ID_BUTTON32795
Command7=ID_BUTTON32771
Command8=ID_BUTTON32772
Command9=ID_BUTTON32773
Command10=ID_BUTTON32774
Command11=ID_BUTTON32775
Command12=ID_BUTTON32776
Command13=ID_FILE_NEW
Command14=ID_FILE_OPEN
Command15=ID_FILE_SAVE
Command16=ID_FILE_PRINT
Command17=ID_APP_ABOUT
CommandCount=17

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CLithoFrame
Command1=ID_APP_EXIT
Command2=ID_EDIT_UNDO
Command3=ID_EDIT_CUT
Command4=ID_EDIT_COPY
Command5=ID_EDIT_PASTE
Command6=ID_EDIT_EDITMODE
Command7=ID_EDIT_PHASESHIFT0
Command8=ID_EDIT_PHASESHIFT90
Command9=ID_EDIT_PHASESHIFT180
Command10=ID_EDIT_PHASESHIFT270
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_VIEW_ZOOM_IN
Command14=ID_VIEW_ZOOM_OUT
Command15=ID_VIEW_INITIALVIEW
Command16=ID_VIEW_SELECT
Command17=ID_VIEW_ZOOM_RECTANGLE
Command18=ID_VIEW_DRAG_SCROLL
Command19=ID_VIEW_MASK
Command20=ID_VIEW_MASK_GRID
Command21=ID_VIEW_IMAGE_GRID
Command22=ID_VIEW_MOUSE_GRID
Command23=ID_SETTINGS
Command24=ID_SIMULATE
Command25=ID_APP_ABOUT
CommandCount=25

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:CG_IDD_PROGRESS]
Type=1
Class=?
ControlCount=3
Control1=IDCANCEL,button,1342242817
Control2=CG_IDC_PROGDLG_PROGRESS,msctls_progress32,1350565888
Control3=CG_IDC_PROGDLG_STATUS,static,1342308353

