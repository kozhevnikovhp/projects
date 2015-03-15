; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CParametersDialog
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "particlesinbox.h"
LastPage=0

ClassCount=6
Class1=CMainFrame
Class2=CParametersDialog
Class3=CAboutDlg
Class4=CParticlesInBoxApp
Class5=CParticlesInBoxDoc
Class6=CParticlesInBoxView

ResourceCount=3
Resource1=IDD_ABOUTBOX (English (U.S.))
Resource2=IDD_PARAMETERS_DIALOG
Resource3=IDR_MAINFRAME (English (U.S.))

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CParametersDialog]
Type=0
BaseClass=CDialog
HeaderFile=ParametersDialog.h
ImplementationFile=ParametersDialog.cpp
LastObject=IDC_LOSS_EDIT
Filter=D
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=ParticlesInBox.h
ImplementationFile=ParticlesInBox.cpp

[CLS:CParticlesInBoxApp]
Type=0
BaseClass=CWinApp
HeaderFile=ParticlesInBox.h
ImplementationFile=ParticlesInBox.cpp

[CLS:CParticlesInBoxDoc]
Type=0
BaseClass=CDocument
HeaderFile=ParticlesInBoxDoc.h
ImplementationFile=ParticlesInBoxDoc.cpp

[CLS:CParticlesInBoxView]
Type=0
BaseClass=COpenGLView
HeaderFile=ParticlesInBoxView.h
ImplementationFile=ParticlesInBoxView.cpp

[DLG:IDD_PARAMETERS_DIALOG]
Type=1
Class=CParametersDialog
ControlCount=54
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_N_PARTICLES_EDIT,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_G_EDIT,edit,1350631552
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_LOSS_EDIT,edit,1350631552
Control10=IDC_STATIC,static,1342308352
Control11=IDC_R_PARTICLE_EDIT,edit,1350631552
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,static,1342308352
Control17=IDC_X_LEFT_EDIT,edit,1350631552
Control18=IDC_STATIC,static,1342308352
Control19=IDC_X_RIGHT_EDIT,edit,1350631552
Control20=IDC_STATIC,static,1342308352
Control21=IDC_Y_TOP_EDIT,edit,1350631552
Control22=IDC_STATIC,static,1342308352
Control23=IDC_Y_BOTTOM_EDIT,edit,1350631552
Control24=IDC_STATIC,button,1342177287
Control25=IDC_STATIC,static,1342308352
Control26=IDC_WALL_X_POSITION_EDIT,edit,1350631552
Control27=IDC_STATIC,static,1342308352
Control28=IDC_HOLE_POSITION_EDIT,edit,1350631552
Control29=IDC_STATIC,static,1342308352
Control30=IDC_HOLE_SIZE_EDIT,edit,1350631552
Control31=IDC_STATIC,static,1342308352
Control32=IDC_V_INIT_EDIT,edit,1350631552
Control33=IDC_STATIC,button,1342177287
Control34=IDC_STATIC,static,1342308352
Control35=IDC_STATIC,static,1342308352
Control36=IDC_BOUNCE_BOTTOM_EDIT,edit,1350631552
Control37=IDC_STATIC,static,1342308352
Control38=IDC_STATIC,static,1342308352
Control39=IDC_BOUNCE_TOP_EDIT,edit,1350631552
Control40=IDC_STATIC,static,1342308352
Control41=IDC_STATIC,static,1342308352
Control42=IDC_BOUNCE_SIDE_EDIT,edit,1350631552
Control43=IDC_STATIC,static,1342308352
Control44=IDC_STATIC,static,1342308352
Control45=IDC_STATIC,static,1342308352
Control46=IDC_STATIC,button,1342177287
Control47=IDC_STATIC,button,1342177287
Control48=IDC_STATIC,static,1342308352
Control49=IDC_STATIC,static,1342308352
Control50=IDC_STATIC,button,1342177287
Control51=IDC_STATIC,button,1342177287
Control52=IDC_FILE_NAME_STATIC,static,1342308352
Control53=IDC_BROWSE_BUTTON,button,1342242816
Control54=ID_ABOUT_BUTTON,button,1342242816

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDOK,button,1342373889
Control3=IDC_EMAIL_HYPERLINK_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_PRINT
Command6=ID_FILE_PRINT_PREVIEW
Command7=ID_FILE_PRINT_SETUP
Command8=ID_FILE_MRU_FILE1
Command9=ID_APP_EXIT
Command10=ID_EDIT_UNDO
Command11=ID_EDIT_CUT
Command12=ID_EDIT_COPY
Command13=ID_EDIT_PASTE
Command14=ID_VIEW_TOOLBAR
Command15=ID_VIEW_STATUS_BAR
Command16=ID_APP_ABOUT
CommandCount=16

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

