; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CGranSim2DFrame
LastTemplate=CFormView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "gransim2d.h"
LastPage=0

ClassCount=19
Class1=CAbstractForm
Class2=CObjectCommonForm
Class3=CGeometryObjectCommonForm
Class4=CLineGeometryForm
Class5=CPointGeometryForm
Class6=CMaterialConstantsForm
Class7=CParticleSizeOpenGLWnd
Class8=CParticleSizeForm
Class9=CParticleShapeOpenGLWnd
Class10=CParticleShapeForm
Class11=CParticleMaterialForm
Class12=CParticleSourceIntensityForm
Class13=CGranSim2DApp
Class14=CAboutDlg
Class15=CGranSim2DDoc
Class16=CGranSim2DFrame
Class17=CGranSim2DProjectView
Class18=CGranSim2DView

ResourceCount=17
Resource1=IDD_POINT_GEOMETRY_FORM
Resource2=IDD_GEOMETRY_OBJECT_COMMON_FORM
Resource3=IDD_ABSTRACT_FORM
Resource4=IDD_MATERIAL_CONSTANTS_FORM
Resource5=IDD_ABOUTBOX (English (U.S.))
Resource6=IDR_EDITOR_TOOLBAR (English (U.S.))
Resource7=IDD_PARTICLE_SIZE_FORM
Resource8=IDD_REFLECTING_OBJECT_MATERIAL_FORM
Resource9=IDD_PARTICLE_SHAPE_FORM
Resource10=IDD_PARTICLE_SOURCE_TYPE_FORM
Resource11=IDD_PARTICLE_MATERIAL_FORM
Resource12=IDR_DESIGN_2D_VIEW_TOOLBAR (English (U.S.))
Resource13=IDD_OBJECT_COMMON_FORM
Resource14=IDD_RECTANGLE_GEOMETRY_FORM
Resource15=IDD_PARTICLE_SOURCE_INTENSITY_FORM
Resource16=IDD_LINE_GEOMETRY_FORM
Class19=CRectangleGeometryForm
Resource17=IDR_MAINFRAME (English (U.S.))

[CLS:CAbstractForm]
Type=0
BaseClass=CFormView
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CObjectCommonForm]
Type=0
BaseClass=CAbstractForm
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CGeometryObjectCommonForm]
Type=0
BaseClass=CAbstractForm
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CLineGeometryForm]
Type=0
BaseClass=CAbstractForm
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CPointGeometryForm]
Type=0
BaseClass=CAbstractForm
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CMaterialConstantsForm]
Type=0
BaseClass=CAbstractForm
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CParticleSizeOpenGLWnd]
Type=0
BaseClass=COpenGLStatic
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CParticleSizeForm]
Type=0
BaseClass=CAbstractForm
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CParticleShapeOpenGLWnd]
Type=0
BaseClass=COpenGLStatic
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CParticleShapeForm]
Type=0
BaseClass=CAbstractForm
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp

[CLS:CParticleMaterialForm]
Type=0
BaseClass=CAbstractForm
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp
LastObject=ID_SIMULATION_START

[CLS:CParticleSourceIntensityForm]
Type=0
BaseClass=CAbstractForm
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp
LastObject=IDC_APPLY_BUTTON

[CLS:CGranSim2DApp]
Type=0
BaseClass=CWinApp
HeaderFile=GranSim2D.h
ImplementationFile=GranSim2D.cpp
Filter=N
VirtualFilter=AC
LastObject=CGranSim2DApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=GranSim2D.cpp
ImplementationFile=GranSim2D.cpp
LastObject=CAboutDlg

[CLS:CGranSim2DDoc]
Type=0
BaseClass=CDocument
HeaderFile=GranSim2DDoc.h
ImplementationFile=GranSim2DDoc.cpp
LastObject=CGranSim2DDoc

[CLS:CGranSim2DFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=GranSim2DDoc.h
ImplementationFile=GranSim2DDoc.cpp
Filter=T
VirtualFilter=fWC
LastObject=ID_EDIT_DELETE

[CLS:CGranSim2DProjectView]
Type=0
BaseClass=CTreeView
HeaderFile=GranSim2DProjectView.h
ImplementationFile=GranSim2DProjectView.cpp

[CLS:CGranSim2DView]
Type=0
BaseClass=COpenGLView
HeaderFile=gransim2dview.h
ImplementationFile=GranSim2DView.cpp

[DLG:IDD_OBJECT_COMMON_FORM]
Type=1
Class=CObjectCommonForm
ControlCount=4
Control1=65535,static,1342308352
Control2=IDC_GIVEN_NAME_EDIT,edit,1350631552
Control3=IDC_APPLY_BUTTON,button,1342242816
Control4=IDC_CANCEL_BUTTON,button,1342242816

[DLG:IDD_GEOMETRY_OBJECT_COMMON_FORM]
Type=1
Class=CGeometryObjectCommonForm
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_GIVEN_NAME_EDIT,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_COLOR_BUTTON,button,1342242827
Control5=IDC_APPLY_BUTTON,button,1342242816
Control6=IDC_CANCEL_BUTTON,button,1342242816

[DLG:IDD_LINE_GEOMETRY_FORM]
Type=1
Class=CLineGeometryForm
ControlCount=22
Control1=IDC_STATIC,static,1342308352
Control2=IDC_X1_EDIT,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_Y1_EDIT,edit,1350631552
Control5=IDC_APPLY_BUTTON,button,1342242816
Control6=IDC_CANCEL_BUTTON,button,1342242816
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,static,1342308352
Control9=IDC_X2_EDIT,edit,1350631552
Control10=IDC_STATIC,static,1342308352
Control11=IDC_Y2_EDIT,edit,1350631552
Control12=IDC_STATIC,button,1342177287
Control13=IDC_STATIC,static,1342308352
Control14=IDC_XCENTER_EDIT,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_YCENTER_EDIT,edit,1350631552
Control17=IDC_STATIC,button,1342177287
Control18=IDC_STATIC,static,1342308352
Control19=IDC_LENGTH_EDIT,edit,1350631552
Control20=IDC_STATIC,static,1342308352
Control21=IDC_ANGLE_EDIT,edit,1350631552
Control22=IDC_STATIC,button,1342177287

[DLG:IDD_POINT_GEOMETRY_FORM]
Type=1
Class=CPointGeometryForm
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_X_EDIT,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_Y_EDIT,edit,1350631552
Control5=IDC_APPLY_BUTTON,button,1342242816
Control6=IDC_CANCEL_BUTTON,button,1342242816

[DLG:IDD_MATERIAL_CONSTANTS_FORM]
Type=1
Class=CMaterialConstantsForm
ControlCount=11
Control1=IDC_STATIC,static,1342308352
Control2=IDC_NU_EDIT,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_E_EDIT,edit,1350631552
Control5=IDC_STATIC,button,1342177287
Control6=IDC_STATIC,static,1342308352
Control7=IDC_RO_EDIT,edit,1350631552
Control8=IDC_APPLY_BUTTON,button,1342242816
Control9=IDC_CANCEL_BUTTON,button,1342242816
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352

[DLG:IDD_PARTICLE_SIZE_FORM]
Type=1
Class=CParticleSizeForm
ControlCount=19
Control1=IDC_SIZE_STATIC,static,1342308352
Control2=IDC_STATIC,button,1342177287
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,button,1342177287
Control5=IDC_FIXED_SIZE_RADIO,button,1342177289
Control6=IDC_UNIFORM_SIZE_RADIO,button,1342177289
Control7=IDC_NORMAL_SIZE_RADIO,button,1342177289
Control8=IDC_APPLY_BUTTON,button,1342242816
Control9=IDC_CANCEL_BUTTON,button,1342242816
Control10=IDC_FIXED_SIZE_EDIT,edit,1350631552
Control11=IDC_STATIC,static,1342308352
Control12=IDC_UNIFORM_FROM_EDIT,edit,1350631552
Control13=IDC_STATIC,static,1342308352
Control14=IDC_UNIFORM_TO_EDIT,edit,1350631552
Control15=IDC_STATIC,static,1342308352
Control16=IDC_NORMAL_MEDIAN_EDIT,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_NORMAL_SIGMA_EDIT,edit,1350631552
Control19=IDC_STATIC,static,1342308352

[DLG:IDD_PARTICLE_SHAPE_FORM]
Type=1
Class=CParticleShapeForm
ControlCount=8
Control1=IDC_SHAPE_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_SQUARENESS_EDIT,edit,1350631552
Control4=IDC_SQUARENESS_SPIN,msctls_updown32,1342177446
Control5=IDC_STATIC,static,1342308352
Control6=IDC_ASPECT_RATIO_EDIT,edit,1350631552
Control7=IDC_APPLY_BUTTON,button,1342242816
Control8=IDC_CANCEL_BUTTON,button,1342242816

[DLG:IDD_PARTICLE_MATERIAL_FORM]
Type=1
Class=CParticleMaterialForm
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_MATERIAL_COMBO,combobox,1344340227
Control3=IDC_APPLY_BUTTON,button,1342242816
Control4=IDC_CANCEL_BUTTON,button,1342242816

[DLG:IDD_PARTICLE_SOURCE_INTENSITY_FORM]
Type=1
Class=CParticleSourceIntensityForm
ControlCount=8
Control1=IDC_APPLY_BUTTON,button,1342242816
Control2=IDC_CANCEL_BUTTON,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_INTENSITY_EDIT,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_UNIFORM_FLOW_RADIO,button,1342177289
Control8=IDC_POISSON_FLOW_RADIO,button,1342177289

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_ABSTRACT_FORM]
Type=1
Class=?
ControlCount=0

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
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_BUTTON32790
Command8=ID_FILE_PRINT
Command9=ID_APP_ABOUT
CommandCount=9

[TB:IDR_DESIGN_2D_VIEW_TOOLBAR (English (U.S.))]
Type=1
Class=?
Command1=ID_VIEW_ZOOM_IN
Command2=ID_VIEW_ZOOM_OUT
Command3=ID_VIEW_INITIALVIEW
Command4=ID_VIEW_SELECT
Command5=ID_VIEW_ZOOM_RECTANGLE
Command6=ID_VIEW_DRAG_SCROLL
Command7=ID_VIEW_EDITMODE
CommandCount=7

[TB:IDR_EDITOR_TOOLBAR (English (U.S.))]
Type=1
Class=?
Command1=ID_VIEW_ZOOM_IN
Command2=ID_BUTTON32787
Command3=ID_VIEW_ZOOM_OUT
Command4=ID_VIEW_INITIALVIEW
Command5=ID_VIEW_SELECT
Command6=ID_VIEW_ZOOM_RECTANGLE
Command7=ID_VIEW_DRAG_SCROLL
Command8=ID_VIEW_EDITMODE
CommandCount=8

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=CGranSim2DFrame
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
Command14=ID_EDIT_CREATENEWMATERIAL
Command15=ID_EDIT_CREATENEWPARTICLETYPE
Command16=ID_EDIT_CREATEREFLECTINGWALL
Command17=ID_EDIT_CREATEREFLECTINGRECTANGLE
Command18=ID_EDIT_CREATEPARTICLESOURCE_LINEAR
Command19=ID_EDIT_CREATEPARTICLESOURCE_POINT
Command20=ID_EDIT_DELETE
Command21=ID_VIEW_TOOLBAR
Command22=ID_VIEW_STATUS_BAR
Command23=ID_VIEW_ZOOM_IN
Command24=ID_VIEW_ZOOM_OUT
Command25=ID_VIEW_INITIALVIEW
Command26=ID_VIEW_SELECT
Command27=ID_VIEW_ZOOM_RECTANGLE
Command28=ID_VIEW_DRAG_SCROLL
Command29=ID_VIEW_EDITMODE
Command30=ID_SIMULATION_START
Command31=ID_SIMULATION_STOP
Command32=ID_SIMULATION_PAUSE
Command33=ID_APP_ABOUT
CommandCount=33

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

[DLG:IDD_PARTICLE_SOURCE_TYPE_FORM]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_PARTICLE_TYPE_COMBO,combobox,1344340227
Control3=IDC_APPLY_BUTTON,button,1342242816
Control4=IDC_CANCEL_BUTTON,button,1342242816

[DLG:IDD_REFLECTING_OBJECT_MATERIAL_FORM]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342308352
Control2=IDC_MATERIAL_COMBO,combobox,1344340227
Control3=IDC_APPLY_BUTTON,button,1342242816
Control4=IDC_CANCEL_BUTTON,button,1342242816

[DLG:IDD_RECTANGLE_GEOMETRY_FORM]
Type=1
Class=CRectangleGeometryForm
ControlCount=12
Control1=IDC_APPLY_BUTTON,button,1342242816
Control2=IDC_CANCEL_BUTTON,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_XCENTER_EDIT,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_YCENTER_EDIT,edit,1350631552
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,button,1342177287
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_WIDTH_EDIT,edit,1350631552
Control12=IDC_HEIGHT_EDIT,edit,1350631552

[CLS:CRectangleGeometryForm]
Type=0
HeaderFile=controlforms.h
ImplementationFile=controlforms.cpp
BaseClass=CAbstractForm
Filter=D
LastObject=IDC_CANCEL_BUTTON

