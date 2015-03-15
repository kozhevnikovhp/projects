#include "stdafx.h"
#define INITGUID
#include "Direct3D.h"
#include <math.h>

BOOL CDirect3D::fDebug=FALSE; 
BOOL CDirect3D::fDeviceFound=FALSE; 
DWORD CDirect3D::dwDeviceBitDepth=0;
char CDirect3D::szDeviceName[MAX_DEVICE_NAME]; 
char CDirect3D::szDeviceDesc[MAX_DEVICE_DESC];
D3DDEVICEDESC CDirect3D::d3dHWDeviceDesc; 
D3DDEVICEDESC CDirect3D::d3dSWDeviceDesc;
GUID CDirect3D::guidDevice;
DDPIXELFORMAT CDirect3D::RGB_PixelFormat;		// Pixel format for RGB textures
DDPIXELFORMAT CDirect3D::RGBAPixelFormat;		// Pixel format for RGBA textures*/
DDPIXELFORMAT CDirect3D::ddpfZBuffer; // Формат Z-буфера(глубина).
BOOL CDirect3D::bIsBlendTexture=TRUE;	// Наличие RGBAPixelFormat.
BOOL CDirect3D::bIsHardware=TRUE;	// 

CDirect3D::CDirect3D(DWORD hWnd) : C3DRenderer(hWnd)
{
	memset(m_Lights, 0, sizeof(m_Lights));
	m_LightCount=0;

	fActive=TRUE;
	fSuspended=FALSE; 
	m_hCurrentMaterial=0;
	m_pDD=NULL;
	m_pDDFrontBuffer=NULL; 
	m_pDDBackBuffer=NULL; 
	m_pDDZBuffer=NULL; 
	m_pDDPalette=NULL; 
	m_pD3D=NULL; 
	m_pD3DDevice=NULL;
	m_pD3DViewport=NULL;
	m_pD3DBackMaterial=NULL;
	m_pD3DCurrentMaterial=NULL;
	
	rSrcRect.top=rSrcRect.left=0;
	rSrcRect.bottom=rSrcRect.right=100;
	rDstRect.top=rDstRect.left=rDstRect.bottom=rDstRect.right=0;

	d3dWorldMatrix._11=D3DVAL(1.0);
	d3dWorldMatrix._12=D3DVAL(0.0);
	d3dWorldMatrix._13=D3DVAL(0.0);
	d3dWorldMatrix._14=D3DVAL(0.0);
	d3dWorldMatrix._21=D3DVAL(0.0);
	d3dWorldMatrix._22=D3DVAL(1.0);
	d3dWorldMatrix._23=D3DVAL(0.0);
	d3dWorldMatrix._24=D3DVAL(0.0);
	d3dWorldMatrix._31=D3DVAL(0.0);
	d3dWorldMatrix._32=D3DVAL(0.0);
	d3dWorldMatrix._33=D3DVAL(1.0);
	d3dWorldMatrix._34=D3DVAL(0.0);
	d3dWorldMatrix._41=D3DVAL(0.0);
	d3dWorldMatrix._42=D3DVAL(0.0);
	d3dWorldMatrix._43=D3DVAL(0.0);
	d3dWorldMatrix._44=D3DVAL(1.0);
 
	CalculateViewMatrix();

	SetPerspectiveProjection(45.f, 0.1f, 2000000.f);
  
	m_BackRed=m_BackGreen=m_BackBlue=1.0;

	HRESULT hRes = CreateDirect3D();
	if (FAILED(hRes))
	{
		ReleaseDirect3D();
		return;
	}
	hRes = CreateFrontBuffer();
	if (FAILED(hRes))
	{ 
		ReleaseFront();
		ReleaseDirect3D();
		return;
	}
 
	hRes = ChooseDevice(); 
	if (FAILED(hRes)) 
	{ 
		ReleaseFront(); 
		ReleaseDirect3D(); 
		return; 
	}
}

CDirect3D::~CDirect3D()
{
	FreeAllTextures();
	ReleaseScene();
    ReleaseDevice();
    ReleaseFront();
    ReleaseDirect3D();
}

HRESULT CDirect3D::ChooseDevice(void) 
{ 
    DDSURFACEDESC2 ddsd; 
    HRESULT       hRes; 
 
    ASSERT(NULL != m_pD3D); 
    ASSERT(NULL != m_pDDFrontBuffer); 
 
        // Since we are running in a window, we will not be changing the 
        // screen depth; therefore, the pixel format of the rendering 
        // target must match the pixel format of the current primary 
        // surface. This means that we need to determine the pixel 
        // format of the primary surface. 

 
    ZeroMemory(&ddsd, sizeof(ddsd)); 
    ddsd.dwSize = sizeof(ddsd); 
    hRes = m_pDDFrontBuffer->GetSurfaceDesc(&ddsd); 
    if (FAILED(hRes)) 
        return hRes; 
 
    dwDeviceBitDepth = 
        BitDepthToFlags(ddsd.ddpfPixelFormat.dwRGBBitCount); 
 
        // Enumerate the devices and pick one. 
 
    fDeviceFound = FALSE; 
    hRes = GetD3D()->EnumDevices(EnumDeviceCallback, &fDeviceFound);

    if (FAILED(hRes))
        return hRes;
 
    if (!fDeviceFound) 
    { 
                // No suitable device was found. We cannot allow 
                // device-creation to continue. 
 
        return DDERR_NOTFOUND; 
    } 
 
//	Программа определения и инициализации глобальных свойств текстур. Вызывается
//	первой при инициализации растеризатора.

	ZeroMemory(&RGB_PixelFormat,sizeof(RGB_PixelFormat));
	ZeroMemory(&RGBAPixelFormat,sizeof(RGBAPixelFormat));

    return DD_OK; 
} 

HRESULT CDirect3D::FindTextureCallback(DDPIXELFORMAT*ddpf, VOID*lParam)
{
//	Программа выбирает формат RGBA текстур. Формат RGB текстур определяется
//	при создании первичной поверхности по внутреннему формату.

	DDPIXELFORMAT *ddpfRGBA =(DDPIXELFORMAT*)lParam;

	if	(!(ddpf->dwFlags & DDPF_RGB)			// Отбрасываем не RGB текстуры.
			||	(  ddpf->dwFlags &(DDPF_PALETTEINDEXED8	// Отбрасываем палитровые текстуры.
					 | DDPF_PALETTEINDEXED4
					 | DDPF_PALETTEINDEXED2
					 | DDPF_PALETTEINDEXED1))) return DDENUMRET_OK;

	if	(ddpf->dwFlags & (DDPF_ALPHA | DDPF_ALPHAPIXELS)) // Проверка текстур с полупрозрачностью
	{
		if	(ddpfRGBA->dwRGBBitCount == 0)  // Если у нас еще нет формата, то запоминаем этот
		{
			CopyMemory(ddpfRGBA, ddpf, sizeof(DDPIXELFORMAT));
		}
		else if	((ddpfRGBA->dwRGBAlphaBitMask != 0xf000 )
				&&	( ddpf    ->dwRGBAlphaBitMask == 0xf000))
		{
			CopyMemory(ddpfRGBA,ddpf,sizeof(DDPIXELFORMAT));
		}
	}
	return DDENUMRET_OK;
}

HRESULT CDirect3D::ReleaseFront(void) 
{ 
    if (NULL != m_pDDPalette) 
    { 
        m_pDDPalette->Release(); 
        m_pDDPalette = NULL; 
    } 
    if (NULL != m_pDDFrontBuffer) 
    { 
        m_pDDFrontBuffer->Release(); 
        m_pDDFrontBuffer = NULL; 
    } 
 
    return DD_OK; 
} 

HRESULT CDirect3D::CreateFrontBuffer() 
{ 
    HRESULT             hRes; 
    DDSURFACEDESC2      ddsd; 
    LPDIRECTDRAWCLIPPER lpddClipper; 
    HDC                 hdc; 
    int                 i; 
    PALETTEENTRY        peColorTable[256]; 
 
    ASSERT(NULL != m_hWnd); 
    ASSERT(NULL != GetDD()); 
    ASSERT(NULL == m_pDDFrontBuffer); 
    ASSERT(NULL == m_pDDPalette); 
 
    // Create the primary surface. 
 
    ZeroMemory(&ddsd, sizeof(ddsd)); 
    ddsd.dwSize         = sizeof(ddsd); 

    ddsd.dwFlags        = DDSD_CAPS; 
    ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    hRes = GetDD()->CreateSurface(&ddsd, &m_pDDFrontBuffer, NULL); 
    if (FAILED(hRes)) 
        return hRes; 
 
    // Create the clipper. We bind the application's window to the 
    // clipper and attach it to the primary. This ensures that when we 
    // blit from the rendering surface to the primary we don't write 
    // outside the visible region of the window. 

 
    hRes = DirectDrawCreateClipper(0, &lpddClipper, NULL); 
    if (FAILED(hRes)) 
        return hRes; 
    hRes = lpddClipper->SetHWnd(0, m_hWnd); 
    if (FAILED(hRes)) 
    { 
        lpddClipper->Release(); 
        return hRes; 
    } 
    hRes = m_pDDFrontBuffer->SetClipper(lpddClipper); 
    if (FAILED(hRes)) 
    { 
        lpddClipper->Release(); 
        return hRes; 
    } 

 
    // We release the clipper interface after attaching it to the 
    // surface because we don't need to use it again. The surface 
    // holds a reference to the clipper when it has been attached. 
    // The clipper will therefore be released automatically when the 
    // surface is released. 
 
    lpddClipper->Release(); 
 
    // If the primary surface is palettized, the device will be, too. 
    // (The device surface must have the same pixel format as the 
    // current primary surface if we want to double buffer with 
    // DirectDraw.) Therefore, if the primary surface is palettized we 
    // need to create a palette and attach it to the primary surface 
    // (and to the device surface when we create it). 
 
    ZeroMemory(&ddsd, sizeof(ddsd)); 
    ddsd.dwSize = sizeof(ddsd); 
    hRes = m_pDDFrontBuffer->GetSurfaceDesc(&ddsd); 
    if (FAILED(hRes)) 
        return hRes; 
    if (ddsd.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) 

    { 
    // Initializing the palette correctly is essential. Since we are 
    // running in a window, we must not change the top ten and bottom 
    // ten static colors. Therefore, we copy them from the system 
    // palette and mark them as read only (D3DPAL_READONLY). The middle 
    // 236 entries are free for use by Direct3D so we mark them free 
    // (D3DPAL_FREE). 
 
    // NOTE: In order that the palette entries are correctly 
    // allocated it is essential that the free entries are 

    // also marked reserved to GDI (PC_RESERVED). 
 
    // NOTE: We don't need to specify the palette caps flag 
    // DDPCAPS_INITIALIZE. This flag is obsolete. CreatePalette 
    // must be given a valid palette-entry array and always 
    // initializes from it. 
 
        hdc = ::GetDC(m_hWnd); 
        GetSystemPaletteEntries(hdc, 0, 256, peColorTable); 
        ::ReleaseDC(m_hWnd,hdc); 
 
        for (i = 0; i < 10; i++) 
            peColorTable[i].peFlags = D3DPAL_READONLY; 

        for (i = 10; i < 246; i++) 
            peColorTable[i].peFlags = D3DPAL_FREE | PC_RESERVED; 
        for (i = 246; i < 256; i++) 
            peColorTable[i].peFlags = D3DPAL_READONLY; 
        hRes = GetDD()->CreatePalette(DDPCAPS_8BIT, peColorTable, &m_pDDPalette, NULL); 
 
        if (FAILED(hRes)) 
            return hRes; 
 
        hRes = m_pDDFrontBuffer->SetPalette(m_pDDPalette); 
            return hRes; 

    } 
 
    return DD_OK; 
} 

HRESULT CDirect3D::ReleaseDirect3D(void) 
{ 
    if (NULL != m_pD3D) 
    { 
        m_pD3D->Release(); 
        m_pD3D = NULL; 
    } 
    if (NULL != m_pDD) 
    { 
        m_pDD->Release(); 
        m_pDD = NULL; 
    } 
    return DD_OK; 
} 

HRESULT CDirect3D::UpdateViewport(void) 
{ 
    HRESULT			hResult;
    D3DVIEWPORT2	d3dViewport;
	DWORD			dwSurfW, dwSurfH;

    // Check Parameters
	if ((!m_pD3DDevice) || (!m_pD3DViewport))
	{
		// Not properly initialized.
		return DDERR_GENERIC;
	}

	// Get Surface Width and Height
	dwSurfW = abs (rDstRect.right - rDstRect.left);
	dwSurfH = abs (rDstRect.bottom - rDstRect.top);

	float inv_aspect;
	
	if (dwSurfW)
		inv_aspect = (float)dwSurfH/(float)dwSurfW;
	else
		inv_aspect = 1.0f;

    // Update Viewport
    ZeroMemory (&d3dViewport, sizeof(d3dViewport));
    d3dViewport.dwSize		= sizeof(d3dViewport);     // Always set size of structure!!!
    d3dViewport.dwX			= 0UL;
    d3dViewport.dwY			= 0UL;
    d3dViewport.dwWidth		= dwSurfW;
    d3dViewport.dwHeight	= dwSurfH;
    d3dViewport.dvClipX		= -1.0f;
    d3dViewport.dvClipY		= inv_aspect;
    d3dViewport.dvClipWidth	= 2.0f;
    d3dViewport.dvClipHeight =2.0f * inv_aspect;
    d3dViewport.dvMinZ		= 0.0f;
    d3dViewport.dvMaxZ		= 1.0f;

	// Update Viewport
    hResult = m_pD3DViewport->SetViewport2(&d3dViewport);
    if (FAILED (hResult))
        return hResult;

	// Update D3D device to use this viewport
    hResult = GetDevice()->SetCurrentViewport(m_pD3DViewport);
    if (FAILED (hResult))
        return hResult;

    // Success
    return DD_OK;
}

LRESULT CDirect3D::RestoreSurfaces(void) 
{ 
    HRESULT hRes; 
 
    hRes = RestoreFront(); 
    if (FAILED(hRes)) 
        return hRes; 
 
    hRes = RestoreDevice(); 
    if (FAILED(hRes)) 
        return hRes; 
 
    return DD_OK; 
}

DWORD CDirect3D::BitDepthToFlags(DWORD dwBitDepth) 
{ 
    switch (dwBitDepth) 
    { 
        case  1: return DDBD_1; 
        case  2: return DDBD_2; 
        case  4: return DDBD_4; 
        case  8: return DDBD_8; 
        case 16: return DDBD_16; 
        case 24: return DDBD_24; 
        case 32: return DDBD_32; 
        default: return 0; 
    } 
} 

HRESULT CDirect3D::RenderScene(void) 
{ 
    HRESULT hRes; 
    D3DRECT d3dRect;
	
    // Clear both back and z-buffer. 
    d3dRect.lX1 = rSrcRect.left; 
    d3dRect.lX2 = rSrcRect.right; 
    d3dRect.lY1 = rSrcRect.top; 
    d3dRect.lY2 = rSrcRect.bottom; 
	if (GetViewport()==NULL) return DD_OK;
    hRes = GetViewport()->Clear(1, &d3dRect, 
        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER); 
    if (FAILED(hRes)) 
        return hRes; 

	if (GetDevice()==NULL) return DD_OK;
 
    // Start the scene. 
    hRes = GetDevice()->BeginScene(); 
    if (FAILED(hRes)) 
        return hRes; 
 
	hRes=GetDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD,&d3dWorldMatrix);
	if (FAILED(hRes))
        return hRes;
    
	hRes=GetDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW,&d3dViewMatrix);
	if (FAILED(hRes))
        return hRes;
    
	hRes=GetDevice()->SetTransform(D3DTRANSFORMSTATE_PROJECTION,&d3dProjMatrix);
	if (FAILED(hRes))
        return hRes;

	hRes=UpdateBackGround();
	if (FAILED(hRes))
        return hRes;
	
	hRes=OnRenderScene();
 
    hRes = GetDevice()->EndScene(); 
    if (FAILED(hRes)) 
        return hRes; 
 
    return DD_OK; 
} 

HRESULT WINAPI CDirect3D::EnumDeviceCallback(LPGUID	lpGUID,
                                      LPSTR				lpszDeviceDesc,
                                      LPSTR				lpszDeviceName,
                                      LPD3DDEVICEDESC	lpd3dHWDeviceDesc,
                                      LPD3DDEVICEDESC	lpd3dSWDeviceDesc,
                                      LPVOID			lpUserArg)
{ 
    LPD3DDEVICEDESC lpd3dDeviceDesc; 
 
        // If there is no hardware support the color model is zero. 
 
    bIsHardware     = (0 != lpd3dHWDeviceDesc->dcmColorModel); 
    lpd3dDeviceDesc = (bIsHardware ? lpd3dHWDeviceDesc : 
                                     lpd3dSWDeviceDesc); 
 
        // If we are in debug mode and this is a hardware device, 
        // skip it. 
 
    if (fDebug && bIsHardware) 
        return D3DENUMRET_OK; 

 
        // Does the device render at the depth we want? 
 
    if (0 == (lpd3dDeviceDesc->dwDeviceRenderBitDepth & 
              dwDeviceBitDepth)) 
    { 
                // If not, skip this device. 
 
        return D3DENUMRET_OK; 
    } 
 
        // The device must support Gouraud-shaded triangles. 
 
    if (D3DCOLOR_MONO == lpd3dDeviceDesc->dcmColorModel) 
    { 
        if (!(lpd3dDeviceDesc->dpcTriCaps.dwShadeCaps & 
              D3DPSHADECAPS_COLORGOURAUDMONO)) 

        { 
                        // No Gouraud shading. Skip this device. 
 
            return D3DENUMRET_OK; 
        } 
    } 
    else 
    { 
        if (!(lpd3dDeviceDesc->dpcTriCaps.dwShadeCaps & 
              D3DPSHADECAPS_COLORGOURAUDRGB)) 
        { 
                        // No Gouraud shading. Skip this device. 
 
            return D3DENUMRET_OK; 
        } 
    } 
 
    if (!bIsHardware && fDeviceFound && 
           (D3DCOLOR_RGB == lpd3dDeviceDesc->dcmColorModel)) 

    { 
                // If this is software RGB and we already have found 
                // a software monochromatic renderer, we are not 
                // interested. Skip this device. 
 
        return D3DENUMRET_OK; 
    } 
 
        // This is a device we are interested in. Save the details. 
 
    fDeviceFound = TRUE;
    CopyMemory(&guidDevice, lpGUID, sizeof(GUID)); 
    strcpy(szDeviceDesc, lpszDeviceDesc); 
    strcpy(szDeviceName, lpszDeviceName); 

    CopyMemory(&d3dHWDeviceDesc, lpd3dHWDeviceDesc, 
               sizeof(D3DDEVICEDESC)); 
    CopyMemory(&d3dSWDeviceDesc, lpd3dSWDeviceDesc, 
               sizeof(D3DDEVICEDESC)); 
 
        // If this is a hardware device, we have found 
        // what we are looking for. 
 
    if (bIsHardware) 
        return D3DENUMRET_CANCEL; 
 
        // Otherwise, keep looking. 
 
    return D3DENUMRET_OK; 
} 

HRESULT WINAPI CDirect3D::EnumZBufferFormatsCallback(DDPIXELFORMAT *pddpf, VOID *pddpfDesired)
{
//	Программа просматривает форматы Z-буферов и выбирает 32-х или 16-ти битовый.

	if	((pddpf		   == NULL )
	||	( pddpfDesired == NULL)) return D3DENUMRET_CANCEL;

	if	(pddpf->dwFlags ==((DDPIXELFORMAT*)pddpfDesired)->dwFlags)
	{
		if	(((DDPIXELFORMAT*)pddpfDesired)->dwZBufferBitDepth == pddpf->dwZBufferBitDepth)
		{
			CopyMemory(pddpfDesired,pddpf,sizeof(DDPIXELFORMAT));
			return D3DENUMRET_CANCEL;
		}
	}
	return D3DENUMRET_OK;
}

HRESULT CDirect3D::CreateDirect3D() 
{ 
	LPDIRECTDRAW  lpDD;

    HRESULT hRes; 
 
    ASSERT(NULL == GetDD()); 
    ASSERT(NULL == GetD3D()); 
 
    // Create the DirectDraw/3D driver object and get the DirectDraw 
    // interface to that object. 
 
    hRes = DirectDrawCreate(NULL, &lpDD, NULL); 
    if (FAILED(hRes)) 
        return hRes; 
 
    // Since we are running in a window, set the cooperative level to 
    // normal. Also, to ensure that the palette is realized correctly, 
    // we need to pass the window handle of the main window. 
 
    hRes = lpDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL); 
    if (FAILED(hRes)) 
        return hRes; 
 
        // Retrieve the Direct3D interface to the DirectDraw/3D driver 
        // object. 
 
	hRes = lpDD->QueryInterface(IID_IDirectDraw4, (LPVOID *)&m_pDD); 
    if (FAILED(hRes)) 
        return hRes; 

    hRes = lpDD->QueryInterface(IID_IDirect3D3, (LPVOID *)(&m_pD3D)); 
    if (FAILED(hRes)) 
        return hRes; 
 
    return DD_OK; 
} 

HRESULT CDirect3D::CreateDevice(DWORD dwWidth, DWORD dwHeight) 
{ 
    DDSURFACEDESC2  ddsd; 
    HRESULT         hRes; 
 
    ASSERT(NULL != GetDD()); 
    ASSERT(NULL != GetD3D());
    ASSERT(NULL != m_pDDFrontBuffer); 
    ASSERT(NULL == m_pDDBackBuffer); 
    ASSERT(NULL == GetDevice()); 
 

 
    // Create the device surface. The pixel format will be identical 
    // to that of the primary surface, so we don't have to explicitly 
    // specify it. We do need to explicitly specify the size, memory 
    // type and capabilities  of the surface. 
 
    ZeroMemory(&ddsd, sizeof(ddsd)); 
    ddsd.dwSize = sizeof(ddsd); 
	m_pDDFrontBuffer->GetSurfaceDesc(&ddsd);
    ddsd.dwSize         = sizeof(ddsd); 
    ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT; 
    ddsd.dwWidth        = dwWidth; 
    ddsd.dwHeight       = dwHeight; 

    ddsd.ddsCaps.dwCaps =  DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN;
	ddsd.ddsCaps.dwCaps|= DDSCAPS_VIDEOMEMORY;
    
	hRes = GetDD()->CreateSurface(&ddsd, &m_pDDBackBuffer, NULL); 
    if (FAILED(hRes)) 
        return hRes; 
 
    // If we have created a palette, we have already determined that 
    // the primary surface (and hence the device surface) is palettized. 
    // Therefore, we should attach the palette to the device surface. 
    // (The palette is already attached to the primary surface.) 

    if (NULL != m_pDDPalette) 
    { 
        hRes = m_pDDBackBuffer->SetPalette(m_pDDPalette); 
        if (FAILED(hRes)) 
            return hRes; 
    } 
 
    // We now determine whether or not we need a z-buffer and, if 
    // so, its bit depth. 
 
	CreateZBuffer();
 
    // Now all the elements are in place: the device surface is in the 
    // correct memory type; a z-buffer has been attached with the 
    // correct depth and memory type; and a palette has been attached, 
    // if necessary. Now we can query for the Direct3D device we chose 
    // earlier. 
 
	hRes=GetD3D()->CreateDevice(guidDevice, m_pDDBackBuffer, &m_pD3DDevice, NULL);
    if (FAILED(hRes)) 
        return hRes;

	 // Определение формата текстур.
	RGB_PixelFormat.dwSize = sizeof(RGB_PixelFormat);
	if	(m_pDDFrontBuffer->GetPixelFormat(&RGB_PixelFormat)!=DD_OK)
	{
		TRACE0("Device doesn't support requested RGB textures.\n");
	}

	GetDevice()->EnumTextureFormats(FindTextureCallback, (LPVOID)&RGBAPixelFormat);
	bIsBlendTexture=RGBAPixelFormat.dwRGBBitCount!=0;

	if	(!RGB_PixelFormat.dwRGBBitCount)
	{
		TRACE0("Disabled RGB PixelFormat. Textured disabled.\n");
	}

    return DD_OK; 
}

HRESULT CDirect3D::CreateZBuffer()
{
//	Программа создает поверхность Z-буфера и присоединяет ее ко вторичному
//	буферу.

	DDSURFACEDESC2	bbsd;
	ZeroMemory(&bbsd,sizeof(DDSURFACEDESC2));
	bbsd.dwSize	= sizeof(DDSURFACEDESC2);

	DDSURFACEDESC2	ddsd;
	ZeroMemory(&ddsd,sizeof(DDSURFACEDESC2));
	ddsd.dwSize	= sizeof(DDSURFACEDESC2);
	
	HRESULT hr;

	m_pDDBackBuffer->GetSurfaceDesc(&bbsd);
	m_pDDBackBuffer->GetSurfaceDesc(&ddsd);

	if	(ddpfZBuffer.dwSize != sizeof(DDPIXELFORMAT))
	{
		ddpfZBuffer.dwFlags			  = DDPF_ZBUFFER;
		ddpfZBuffer.dwZBufferBitDepth = 16;

		/*if	(gD3DDraw::bIsZMode32 && (gD3DDraw::gRGBBitCount == 32))
		{
		if		((gD3DDraw::d3HAL.dwDeviceZBufferBitDepth&DDBD_32)!=0) ddpfZBuffer.dwZBufferBitDepth = 32;
		else if	((gD3DDraw::d3HAL.dwDeviceZBufferBitDepth&DDBD_24)!=0) ddpfZBuffer.dwZBufferBitDepth = 24;
		}*/
	GetD3D()->EnumZBufferFormats(guidDevice,EnumZBufferFormatsCallback,(VOID*)&ddpfZBuffer);

	if	(ddpfZBuffer.dwSize != sizeof(DDPIXELFORMAT))
	{
		TRACE0("Device doesn't support requested zbuffer format\n");
		return false;
	}
	TRACE1("Select %d-ZBuffer depth\n",ddpfZBuffer.dwZBufferBitDepth);
	}
	ddsd.dwFlags		= DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps	= DDSCAPS_ZBUFFER;
	ddsd.dwWidth		= rDstRect.right-rDstRect.left;
	ddsd.dwHeight		= rDstRect.bottom-rDstRect.top;

	CopyMemory(&ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT));
	{
		if	(bbsd.ddsCaps.dwCaps &  DDSCAPS_VIDEOMEMORY )
			ddsd.ddsCaps.dwCaps|= DDSCAPS_VIDEOMEMORY ;
		else
			ddsd.ddsCaps.dwCaps|= DDSCAPS_SYSTEMMEMORY;
	}
	if	((hr = GetDD()->CreateSurface(&ddsd, &m_pDDZBuffer, NULL))!=DD_OK)
	{
		TRACE0("CreateSurface for Z-buffer failed: %s\n");
		return DDERR_EXCEPTION;
	}
	if	((hr = m_pDDBackBuffer->AddAttachedSurface(m_pDDZBuffer))!=DD_OK)
	{
		TRACE0("AddAttachedSurface for Z-buffer failed:\n");

		if	((bbsd.ddsCaps.dwCaps&DDSCAPS_VIDEOMEMORY) != 0)
			TRACE0("BackBuffer DDSCAPS_VIDEOMEMORY\n");
		else 
			TRACE0("BackBuffer DDSCAPS_SYSTEMMEMORY\n");

		if	((ddsd.ddsCaps.dwCaps&DDSCAPS_VIDEOMEMORY) != 0)
			TRACE0("Z-Buffer DDSCAPS_VIDEOMEMORY\n");
		else
			TRACE0("Z-Buffer DDSCAPS_SYSTEMMEMORY\n");

		return DDERR_GENERIC;
	}
	else
	{
		if	((bbsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0)
			TRACE0("BackBuffer DDSCAPS_VIDEOMEMORY\n");
		else
			TRACE0("BackBuffer DDSCAPS_SYSTEMMEMORY\n");
	}
	return DD_OK;
}

HRESULT CDirect3D::ReleaseDevice(void) 
{
    if (NULL != m_pD3DDevice) 
    { 
        m_pD3DDevice->Release(); 
        m_pD3DDevice = NULL; 
    } 
    if (NULL != m_pDDZBuffer) 
    { 
        m_pDDZBuffer->Release(); 
        m_pDDZBuffer = NULL; 
    } 
    if (NULL != m_pDDBackBuffer) 
    { 
        m_pDDBackBuffer->Release(); 
        m_pDDBackBuffer = NULL; 
    } 
 
    return DD_OK; 
}

HRESULT CDirect3D::RestoreDevice(void) 
{ 
    HRESULT hRes; 
 
    if (NULL != m_pDDZBuffer) 
    { 
        hRes = m_pDDZBuffer->Restore();
        if (FAILED(hRes)) 
            return hRes; 
    } 
 
    if (NULL != m_pDDBackBuffer) 
    { 
        hRes = m_pDDBackBuffer->Restore(); 
        if (FAILED(hRes)) 
            return hRes; 
    } 
 
    return DD_OK; 
} 

HRESULT CDirect3D::RestoreFront(void) 
{ 
    ASSERT(NULL != m_pDDFrontBuffer); 
    return m_pDDFrontBuffer->Restore(); 
}

DWORD CDirect3D::FlagsToBitDepth(DWORD dwFlags) 
{ 
    if (dwFlags & DDBD_1) 
        return 1; 
    else if (dwFlags & DDBD_2) 
        return 2; 
    else if (dwFlags & DDBD_4) 
        return 4; 
    else if (dwFlags & DDBD_8) 
        return 8; 
    else if (dwFlags & DDBD_16) 
        return 16; 
    else if (dwFlags & DDBD_24) 
        return 24; 
    else if (dwFlags & DDBD_32) 
        return 32; 
    else 
        return 0; 
}

HRESULT CDirect3D::PrepareScene()
{
	HRESULT hRes;
	hRes = GetD3D()->CreateViewport(&m_pD3DViewport, NULL); 
	if (FAILED(hRes))
		return hRes;
	hRes = GetDevice()->AddViewport(m_pD3DViewport); 
    if (FAILED(hRes))
        return hRes;
	hRes=OnInitScene();
    if (FAILED(hRes))
        return hRes;
	hRes=RebuildLights();
    if (FAILED(hRes))
        return hRes;

	return DD_OK;
}

int CDirect3D::AddLight(D3DLIGHT *pLight)
{
	if (m_LightCount>=sizeof(m_Lights)/sizeof(m_Lights[0]))
		return false;
	memcpy(&m_Lights[m_LightCount], pLight, sizeof(*pLight));
    m_Lights[m_LightCount].dwSize = sizeof(m_Lights[0]); 
	m_LightCount++;
	return m_LightCount-1;
}

bool CDirect3D::GetLightInfo(int LightToGet, D3DLIGHT *pLight)
{
	AfxIsValidAddress(pLight, sizeof(*pLight));
	if (LightToGet>=sizeof(m_Lights)/sizeof(m_Lights[0])) return false;
	if (LightToGet>=m_LightCount) return false;
	memcpy(pLight, &m_Lights[LightToGet], sizeof(*pLight));
	return true;
}

void CDirect3D::ModifyLight(int LightToModify, D3DLIGHT *pLight)
{
	if (LightToModify>=sizeof(m_Lights)/sizeof(m_Lights[0])) return;
	if (LightToModify>=m_LightCount) return;
	memcpy(&m_Lights[LightToModify], pLight, sizeof(*pLight));
    m_Lights[LightToModify].dwSize = sizeof(m_Lights[0]); 
}

int CDirect3D::AddLight(D3DLIGHTTYPE type,
							double red, double green, double blue, double alfa,
							double pX, double pY, double pZ, 
							double dX, double dY, double dZ, 
							double att0, double att1, double att2)
{
	if (m_LightCount>=sizeof(m_Lights)/sizeof(m_Lights[0]))
		return false;
	ZeroMemory(&m_Lights[m_LightCount], sizeof(m_Lights[0])); 
    m_Lights[m_LightCount].dwSize = sizeof(m_Lights[0]); 
    m_Lights[m_LightCount].dltType = type;

    m_Lights[m_LightCount].dvRange = 100000.f;
	
    m_Lights[m_LightCount].dcvColor.dvR    = D3DVAL(red);
    m_Lights[m_LightCount].dcvColor.dvG    = D3DVAL(green); 
    m_Lights[m_LightCount].dcvColor.dvB    = D3DVAL(blue); 
    m_Lights[m_LightCount].dcvColor.dvA    = D3DVAL(alfa); 
    m_Lights[m_LightCount].dvPosition.dvX  = D3DVAL(pX); 
    m_Lights[m_LightCount].dvPosition.dvY  = D3DVAL(pY); 
    m_Lights[m_LightCount].dvPosition.dvZ  = D3DVAL(pZ); 
    m_Lights[m_LightCount].dvDirection.dvX  = D3DVAL(dX); 
    m_Lights[m_LightCount].dvDirection.dvY  = D3DVAL(dY); 
    m_Lights[m_LightCount].dvDirection.dvZ  = D3DVAL(dZ); 
    m_Lights[m_LightCount].dvAttenuation0  = D3DVAL(att0); 
    m_Lights[m_LightCount].dvAttenuation1  = D3DVAL(att1); 
    m_Lights[m_LightCount].dvAttenuation2  = D3DVAL(att2);
	m_LightCount++;
	return m_LightCount-1;
}

void CDirect3D::SetBackGround(double Red, double Green, double Blue)
{
	m_BackRed=Red;
	m_BackGreen=Green;
	m_BackBlue=Blue;
	OnPaint();	OnPaint();
}

HRESULT CDirect3D::UpdateBackGround()
{
	D3DMATERIALHANDLE hd3dBackgroundMaterial;
	if (m_pD3DBackMaterial)
		m_pD3DBackMaterial->Release();
    HRESULT hRes = GetD3D()->CreateMaterial(&m_pD3DBackMaterial, NULL);
    if (FAILED(hRes)) 
        return hRes; 
	D3DMATERIAL Material;
	ZeroMemory(&Material, sizeof(Material)); 
	Material.dwSize = sizeof(Material); 
	Material.dcvDiffuse.r  = D3DVAL(m_BackRed); 
	Material.dcvDiffuse.g  = D3DVAL(m_BackGreen); 
	Material.dcvDiffuse.b  = D3DVAL(m_BackBlue); 
	Material.dvPower       = D3DVAL(0.); 
	Material.dwRampSize    = 1; 
    hRes = m_pD3DBackMaterial->SetMaterial(&Material); 
    if (FAILED(hRes)) 
        return hRes; 
    hRes = m_pD3DBackMaterial->GetHandle(GetDevice(), &hd3dBackgroundMaterial); 
    if (FAILED(hRes)) 
        return hRes; 
	hRes=GetViewport()->SetBackground(hd3dBackgroundMaterial);
	return hRes;
}

HRESULT CDirect3D::ReleaseScene()
{
    if (NULL != m_pD3DViewport) 
    { 
        m_pD3DViewport->Release(); 
        m_pD3DViewport = NULL; 
    }
	m_LightCount=0;
	return DD_OK;
}

void CDirect3D::OnPaint()
{
	if (GetDevice()==NULL) return;
    HRESULT hRes; 
 
    // We keeping trying until we succeed or we fail for a reason 
    // other than DDERR_SURFACELOST. 
 	RecalcDestRect();
    while (TRUE) 
    {
        hRes = RenderScene(); 
        if (SUCCEEDED(hRes)) 
        {
			if (OnDraw())
                return;

        }
        while (DDERR_SURFACELOST == hRes)  // Restore lost surfaces 
		{
            hRes = RestoreSurfaces();
		}
    }
}

void CDirect3D::OnSize(int cx, int cy)
{
	C3DRenderer::OnSize(cx, cy);
	ReleaseTextures();

    HRESULT       hRes; 
    DDSURFACEDESC2 ddsd; 
 
        // Nothing to do if we are suspended. 
 
    if (!fSuspended && cx!=0 && cy!=0) 
    { 
        // Update the source and destination rectangles (used by the 
        // blit that shows the rendering in the client area). 
        rDstRect.right  = rDstRect.left + cx; 
        rDstRect.bottom = rDstRect.top  + cy; 
        rSrcRect.right  = cx; 
        rSrcRect.bottom = cy; 
 
        if (NULL != GetDevice()) 

        { 
            // Although we already have a device, we need to be sure it 
            // is big enough for the new window client size. 
 
            // Because the window in this sample has a fixed size, it 
            // should never be necessary to handle this case. This code 
            // will be useful when we make the application resizable. 
 
            ZeroMemory(&ddsd, sizeof(ddsd)); 
            ddsd.dwSize = sizeof(ddsd); 
            hRes = m_pDDBackBuffer->GetSurfaceDesc(&ddsd); 
            if (FAILED(hRes)) 
                return; 
 
            if ((cx > (int)ddsd.dwWidth) || (cy > (int)ddsd.dwHeight)) 
            { 
                // The device is too small. We need to shut it down 
                // and rebuild it. 
                // Execute buffers are bound to devices, so when 
                // we release the device we must release the execute 
                // buffer. 
 
                ReleaseScene();
                ReleaseDevice(); 
            } 
        } 
 
        if (NULL == GetDevice()) 
        { 
            // No Direct3D device yet. This is either because this is 
            // the first time through the loop or because we discarded 
            // the existing device because it was not big enough for the 
            // new window client size. 
 
            hRes = CreateDevice((DWORD)cx, (DWORD)cy); 

            if (FAILED(hRes)) 
            { 
                return; 
            } 
            hRes = PrepareScene(); 
            if (FAILED(hRes)) 
            { 
                return; 
            }
        } 
 
        hRes = UpdateViewport(); 
        if (FAILED(hRes)) 
            return; 
 
        // Render at the new size and show the results in the window's 
        // client area.
		RebuildTextures();
		OnPaint(); 
		OnPaint(); 
    }
}

// Product projection, view and world matrices
void CDirect3D::GetProjViewWorldMatrix(D3DMATRIX &ResultMatrix)
{
	D3DMATRIX ProjMatrix, ViewMatrix, WorldMatrix;
	GetDevice()->GetTransform(D3DTRANSFORMSTATE_PROJECTION, &ProjMatrix);
	GetDevice()->GetTransform(D3DTRANSFORMSTATE_VIEW, &ViewMatrix);
	GetDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &WorldMatrix);
	ResultMatrix=MatrixMult(ProjMatrix, ViewMatrix);
	ResultMatrix=MatrixMult(ResultMatrix, WorldMatrix);
}

// Product projection and view matrices
void CDirect3D::GetProjViewMatrix(D3DMATRIX &ResultMatrix)
{
	D3DMATRIX ProjMatrix, ViewMatrix;
	GetDevice()->GetTransform(D3DTRANSFORMSTATE_PROJECTION, &ProjMatrix);
	GetDevice()->GetTransform(D3DTRANSFORMSTATE_VIEW, &ViewMatrix);
	ResultMatrix=MatrixMult(ProjMatrix, ViewMatrix);
}

void CDirect3D::Point3DToPoint2D(float x3D, float y3D, float z3D, float &x2D, float &y2D, float &w)
{
	D3DMATRIX ProjViewMatrix;
	GetProjViewMatrix(ProjViewMatrix);
	Point3DToPoint2D(x3D, y3D, z3D, x2D, y2D, w, &ProjViewMatrix);
}

void CDirect3D::Point3DToPoint2D(float x3D, float y3D, float z3D, float &x2D, float &y2D, float &w, D3DMATRIX *pProjViewWorldMatrix)
{
	w=1.f;
	TransformVector(pProjViewWorldMatrix, x3D, y3D, z3D, w);
	x2D=x3D/w;
	y2D=y3D/w;

	// Get Surface Width and Height to obtain aspect ratio
	DWORD dwSurfW = abs (rDstRect.right - rDstRect.left);
	DWORD dwSurfH = abs (rDstRect.bottom - rDstRect.top);
	float aspect=1.f;
	if (dwSurfH)
		aspect = (float)dwSurfW/(float)dwSurfH;

	y2D*=aspect;
}

HRESULT CDirect3D::Material(double diffuseR, double diffuseG, double diffuseB, double diffuseA,
		double ambientR, double ambientG, double ambientB, double ambientA,
		double specularR, double specularG, double specularB, double specularA,
		double power, DWORD RampSize)
{
	if (m_pD3DCurrentMaterial)
		m_pD3DCurrentMaterial->Release();
	m_pD3DCurrentMaterial=NULL;
	LPDIRECT3DMATERIAL3 lpD3DMaterial;
	D3DMATERIALHANDLE hMaterial;
	HRESULT hRes = GetD3D()->CreateMaterial(&lpD3DMaterial,NULL);
	if (FAILED(hRes)) 
		return hRes;
	D3DMATERIAL Material;
    ZeroMemory(&Material, sizeof(Material)); 
    Material.dwSize = sizeof(Material); 
    Material.dcvDiffuse.r  = D3DVAL(diffuseR); 
    Material.dcvDiffuse.g  = D3DVAL(diffuseG); 
    Material.dcvDiffuse.b  = D3DVAL(diffuseB); 
    Material.dcvDiffuse.a  = D3DVAL(diffuseA); 
    Material.dcvAmbient.r  = D3DVAL(ambientR); 
    Material.dcvAmbient.g  = D3DVAL(ambientG); 
    Material.dcvAmbient.b  = D3DVAL(ambientB); 
    Material.dcvAmbient.a  = D3DVAL(ambientA); 
    Material.dcvSpecular.r = D3DVAL(specularR); 
    Material.dcvSpecular.g = D3DVAL(specularG); 
    Material.dcvSpecular.b = D3DVAL(specularB); 
    Material.dcvSpecular.a = D3DVAL(specularA); 
    Material.dvPower       = D3DVAL(power);
    Material.dwRampSize    = RampSize;
	hRes = lpD3DMaterial->SetMaterial(&Material); 
	if (FAILED(hRes))
		return hRes;
	hRes=lpD3DMaterial->GetHandle(GetDevice(),&hMaterial);
	if (FAILED(hRes)) 
		return hRes; 
	hRes=GetDevice()->SetLightState(D3DLIGHTSTATE_MATERIAL,hMaterial);
	if (FAILED(hRes))
        return hRes;
	m_pD3DCurrentMaterial=lpD3DMaterial;
	return DD_OK;
}

void CDirect3D::SetPerspectiveProjection(float CameraAngle, float NearPlane, float FarPlane) 
{
    float w = (float)(1./tan(CameraAngle*grad2rad*0.5f));
    float Q = FarPlane/(FarPlane - NearPlane);
    ZeroMemory(&d3dProjMatrix, sizeof(d3dProjMatrix));
    d3dProjMatrix._11 = w;
    d3dProjMatrix._22 = w;
    d3dProjMatrix._33 = Q;
    d3dProjMatrix._43 = -Q*NearPlane;
    d3dProjMatrix._34 = 1;
	Update();
} 

void CDirect3D::Redraw()
{
	OnPaint();
}

HRESULT CDirect3D::RebuildLights()
{
	ReleaseLights();
	HRESULT hRes;
	LPDIRECT3DLIGHT lpD3DLight=NULL;
	for (int i=0; i<m_LightCount; i++)
	{
		GetD3D()->CreateLight(&lpD3DLight, NULL);
		hRes = lpD3DLight->SetLight(&m_Lights[i]); 
		if (FAILED(hRes)) 
			return hRes;
		hRes=GetViewport()->AddLight(lpD3DLight);
		if (FAILED(hRes)) 
			return hRes;
	}
	
	return DD_OK;
}

HRESULT CDirect3D::ReleaseLights()
{
	LPDIRECT3DLIGHT lpLight=NULL, lpNextLight;
	HRESULT hRes=GetViewport()->NextLight(lpLight, &lpNextLight, D3DNEXT_HEAD);
	if (FAILED(hRes)) 
		return hRes;
	while (lpNextLight!=NULL)
	{
		lpLight=lpNextLight;
		hRes=GetViewport()->NextLight(lpLight, &lpNextLight, D3DNEXT_NEXT);
		if (FAILED(hRes)) 
			return hRes;
		hRes=GetViewport()->DeleteLight(lpLight);
		if (FAILED(hRes)) 
			return hRes;
		lpLight->Release();
	}
	return DD_OK;
}

void CDirect3D::CalculateNormals(D3DVERTEX *pVertex)
{
	double nx, ny, nz;
	double ax,ay,az,bx,by,bz;
	ax=pVertex[1].x-pVertex[0].x;
	bx=pVertex[2].x-pVertex[0].x;
	ay=pVertex[1].y-pVertex[0].y;
	by=pVertex[2].y-pVertex[0].y;
	az=pVertex[1].z-pVertex[0].z;
	bz=pVertex[2].z-pVertex[0].z;
	nx=ay*bz-az*by;
	ny=az*bx-ax*bz;
	nz=ax*by-ay*bx;
	double normer=sqrt(nx*nx+ny*ny+nz*nz);
	if (fabs(normer)<1.e-6) return;
	nx/=normer;
	ny/=normer;
	nz/=normer;
	pVertex[0].nx=pVertex[1].nx=pVertex[2].nx=(D3DVALUE)nx;
	pVertex[0].ny=pVertex[1].ny=pVertex[2].ny=(D3DVALUE)ny;
	pVertex[0].nz=pVertex[1].nz=pVertex[2].nz=(D3DVALUE)nz;
}

void CDirect3D::MoveCamera(float dx, float dy, float dz, BOOL bUpdate)
{
	d3dViewMatrix._41+=D3DVAL(dx);
	d3dViewMatrix._42+=D3DVAL(dy);
	d3dViewMatrix._43+=D3DVAL(dz);
	if (bUpdate)
		OnPaint();
}

void CDirect3D::TransformWorld(const D3DMATRIX &TransformMatrix)
{
	MultWorldMatrix(TransformMatrix);
	GetDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &d3dWorldMatrix);
}

void CDirect3D::SetWorldMatrix(const D3DMATRIX &WorldMatrix)
{
	CopyMemory(&d3dWorldMatrix, &WorldMatrix, sizeof(d3dWorldMatrix));
	if (GetDevice())
		GetDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &d3dWorldMatrix);
}

void CDirect3D::MultWorldMatrix(const D3DMATRIX &Multiplier)
{
    D3DMATRIX ret;
	ZeroMemory(&ret, sizeof(ret));

    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            for (int k=0; k<4; k++) {
                ret(i, j) += d3dWorldMatrix(k, j) * Multiplier(i, k);
            }
        }
    }
	CopyMemory(&d3dWorldMatrix, &ret, sizeof(ret));
}

void CDirect3D::CalculateViewMatrix()
{
	float Rx=m_CameraAngle*grad2rad;
	float Ry=m_CameraAzimuth*grad2rad;
	float Rz=m_CameraTilt*grad2rad;

	float cosx=(float)cos(Rx);
	float sinx=(float)sin(Rx);
	float cosy=(float)cos(Ry);
	float siny=(float)sin(Ry);
	float cosz=(float)cos(Rz);
	float sinz=(float)sin(Rz);
	float Tx=-m_CameraX;
	float Ty=-m_CameraY;
	float Tz=-m_CameraZ;

	d3dViewMatrix._11=cosy*cosz;
	d3dViewMatrix._21=-cosy*sinz;
	d3dViewMatrix._31=siny;
	d3dViewMatrix._41=Tx*cosy*cosz
					-Ty*cosy*sinz
					+Tz*siny;

	d3dViewMatrix._12=cosx*sinz+sinx*siny*cosz;
	d3dViewMatrix._22=(cosx*cosz-sinx*siny*sinz);
	d3dViewMatrix._32=-sinx*cosy;
	d3dViewMatrix._42=Tx*(cosx*sinz+sinx*siny*cosz)
					+Ty*(cosx*cosz-sinx*siny*sinz)
					-Tz*sinx*cosy;

	d3dViewMatrix._13=sinx*sinz-cosx*siny*cosz;
	d3dViewMatrix._23=cosx*siny*sinz+sinx*cosz;
	d3dViewMatrix._33=cosx*cosy;
	d3dViewMatrix._43=Tx*(sinx*siny-cosx*siny*cosz)
					+Ty*(cosx*siny*sinz+sinx*cosz)
					+Tz*cosx*cosy;

	d3dViewMatrix._14=(D3DVALUE)0.;
	d3dViewMatrix._24=(D3DVALUE)0.;
	d3dViewMatrix._34=(D3DVALUE)0.;
	d3dViewMatrix._44=(D3DVALUE)1.;
	
	if (GetDevice())
		GetDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &d3dViewMatrix);
}

void CDirect3D::CalculateWorldMatrix()
{
    D3DVALUE cosx=(D3DVALUE)cos(m_WorldRx*grad2rad);
	D3DVALUE sinx=(D3DVALUE)sin(m_WorldRx*grad2rad);
    D3DVALUE cosy=(D3DVALUE)cos(m_WorldRy*grad2rad);
	D3DVALUE siny=(D3DVALUE)sin(m_WorldRy*grad2rad);
    D3DVALUE cosz=(D3DVALUE)cos(m_WorldRz*grad2rad);
	D3DVALUE sinz=(D3DVALUE)sin(m_WorldRz*grad2rad);

	d3dWorldMatrix._11=cosz*cosy;
	d3dWorldMatrix._21=cosy*sinz;
	d3dWorldMatrix._31=-siny;
	d3dWorldMatrix._14=D3DVAL(0.);

	d3dWorldMatrix._12=sinx*cosz*siny-cosx*sinz;
	d3dWorldMatrix._22=cosx*cosz+sinx*siny*sinz;
	d3dWorldMatrix._32=sinx*cosy;
	d3dWorldMatrix._24=D3DVAL(0.);

	d3dWorldMatrix._13=cosx*cosz*siny+sinx*sinz;
	d3dWorldMatrix._23=cosx*siny*sinz-sinx*cosz;
	d3dWorldMatrix._33=cosx*cosy;
	d3dWorldMatrix._34=D3DVAL(0.);

	d3dWorldMatrix._41=D3DVAL(m_WorldX);
	d3dWorldMatrix._42=D3DVAL(m_WorldY);
	d3dWorldMatrix._43=D3DVAL(m_WorldZ);
	d3dWorldMatrix._44=D3DVAL(1.);

	if (IsCoordinateSystemRight())
	{ // если принята правая система, переворачиваем ось Z.
		d3dWorldMatrix._13=-d3dWorldMatrix._13;
		d3dWorldMatrix._23=-d3dWorldMatrix._23;
		d3dWorldMatrix._33=-d3dWorldMatrix._33;
		d3dWorldMatrix._43=-d3dWorldMatrix._43;
	}

	if (GetDevice())
		GetDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD,&d3dWorldMatrix);
}

BOOL CDirect3D::OnDraw()
{
	RecalcDestRect();
	ASSERT(m_pDDFrontBuffer);
	ASSERT(m_pDDBackBuffer);
	HRESULT hRes=m_pDDFrontBuffer->Blt(&rDstRect, m_pDDBackBuffer, &rSrcRect, DDBLT_WAIT, NULL);
	return SUCCEEDED(hRes); 
}

void CDirect3D::RecalcDestRect()
{
	GetClientRect(m_hWnd, &rSrcRect);
	GetClientRect(m_hWnd, &rDstRect);
	POINT pt;
	pt.x=rDstRect.left;
	pt.y=rDstRect.top;
	ClientToScreen(m_hWnd, &pt);
	rDstRect.left=pt.x;
	rDstRect.top=pt.y;
	pt.x=rDstRect.right;
	pt.y=rDstRect.bottom;
	ClientToScreen(m_hWnd, &pt);
	rDstRect.right=pt.x;
	rDstRect.bottom=pt.y;
}

bool CDirect3D::AddTexture(CTextureD3D * pTexture)
{
	bool found=false;
	CTextureD3D *pExisting=NULL;
	for (POSITION pos=m_TextureList.GetHeadPosition(); pos; m_TextureList.GetNext(pos))
	{
		pExisting=(CTextureD3D *)m_TextureList.GetAt(pos);
		if (pExisting->IsEqual(pTexture))
		{
			found=TRUE;
			break;
		}
	}

	if (found)
	{
		pTexture->Equal(pExisting);
		pTexture->SetTheSame(pExisting);
	}
	else
	{
		int rgba=ddpf2cpf(&RGBAPixelFormat);
		int rgb=ddpf2cpf(&RGB_PixelFormat);
		pTexture->Build(rgb, rgba);
	}
	if (pTexture->IsValid())
		m_TextureList.AddTail((CObject *)pTexture);

	return !found;
}

int CDirect3D::ddpf2cpf(DDPIXELFORMAT* ddpf)
{
//	Служебная программа определения идентификатора формата пиксела.

if	(  !(ddpf->dwFlags & DDPF_RGB          ))	return cpfUnknown;	// Не RGB
if	(    ddpf->dwFlags &(DDPF_PALETTEINDEXED4
					   | DDPF_PALETTEINDEXED2
					   | DDPF_PALETTEINDEXED1))	return cpfUnknown;	// Палитровые с короткой палитрой
else if (ddpf->dwFlags & DDPF_PALETTEINDEXED8)	return cpfIndex;	// Палитра, 8 бит

else if (ddpf->dwFlags &(DDPF_ALPHA|DDPF_ALPHAPIXELS))				// RGB с полупрозрачностью
	{
	if	(	 ddpf->dwRGBBitCount == 16)	// 16 bit
		{
		if		(ddpf->dwRBitMask		== 0x00007c00
		&&		 ddpf->dwGBitMask		== 0x000003e0
		&&		 ddpf->dwBBitMask		== 0x0000001f
		&&		 ddpf->dwRGBAlphaBitMask== 0x00008000)	return cpfARGB1555;

		else if (ddpf->dwRBitMask		== 0x00000f00
		&&		 ddpf->dwGBitMask		== 0x000000f0
		&&		 ddpf->dwBBitMask		== 0x0000000f
		&&		 ddpf->dwRGBAlphaBitMask== 0x0000f000)	return cpfARGB4444;
		else											return cpfUnknown;
		}
	else if (ddpf->dwRGBBitCount == 32)	// 32 bit
		{
		if		(ddpf->dwRBitMask		== 0x00ff0000
		&&		 ddpf->dwGBitMask		== 0x0000ff00
		&&		 ddpf->dwBBitMask		== 0x000000ff
		&&		 ddpf->dwRGBAlphaBitMask== 0xff000000)	return cpfARGB8888;

		else if (ddpf->dwRBitMask		== 0xff000000
		&&		 ddpf->dwGBitMask		== 0x00ff0000
		&&		 ddpf->dwBBitMask		== 0x0000ff00
		&&		 ddpf->dwRGBAlphaBitMask== 0x000000ff)	return cpfRGBA8888;
		else									        return cpfUnknown;
		}
	}
else{	// RGB без полупрозрачности

	if	(	 ddpf->dwRGBBitCount == 16)	// 16 bit
		{
		if		(ddpf->dwRBitMask		== 0x00007c00
		&&		 ddpf->dwGBitMask		== 0x000003e0
		&&		 ddpf->dwBBitMask		== 0x0000001f)	return cpfRGB555;

		else if (ddpf->dwRBitMask		== 0x0000f800
		&&		 ddpf->dwGBitMask		== 0x000007e0
		&&		 ddpf->dwBBitMask		== 0x0000001f)	return cpfRGB565;
		else											return cpfUnknown;
		}
	else if (ddpf->dwRGBBitCount == 32)	// 32 bit
		{
		if		(ddpf->dwRBitMask		== 0x00ff0000
		&&		 ddpf->dwGBitMask		== 0x0000ff00
		&&		 ddpf->dwBBitMask		== 0x000000ff)	return cpfRGB_32;
		else									        return cpfUnknown;
		}
	else if (ddpf->dwRGBBitCount == 24)	// 24 bit - ни разу не встречал
		{
		if		(ddpf->dwRBitMask		== 0x00ff0000
		&&		 ddpf->dwGBitMask		== 0x0000ff00
		&&		 ddpf->dwBBitMask		== 0x000000ff)	return cpfRGB_24;
		else									        return cpfUnknown;
		}
	}
	return cpfUnknown;
}


void CDirect3D::BeforeTexture(double diffuseR, double diffuseG, double diffuseB, double diffuseA,
		double ambientR, double ambientG, double ambientB, double ambientA,
		double specularR, double specularG, double specularB, double specularA,
		double power, DWORD RampSize)
{
	HRESULT hRes=GetDevice()->GetLightState(D3DLIGHTSTATE_MATERIAL,&m_hCurrentMaterial);
	if (FAILED(hRes))
        return;
	Material(diffuseR, diffuseG, diffuseB, diffuseA,
			ambientR, ambientG, ambientB, ambientA,
			specularR, specularG, specularB, specularA,
			power, RampSize);
}

void CDirect3D::AfterTexture()
{
	HRESULT hRes=GetDevice()->SetLightState(D3DLIGHTSTATE_MATERIAL,m_hCurrentMaterial);
	if (FAILED(hRes))
        return;
	m_hCurrentMaterial=0;
}

void CDirect3D::ReleaseTextures()
{
	for (POSITION pos=m_TextureList.GetHeadPosition(); pos; m_TextureList.GetNext(pos))
	{
		CTextureD3D *pTexture=(CTextureD3D *)m_TextureList.GetAt(pos);
		pTexture->Release();
	}
}

void CDirect3D::RebuildTextures()
{
	for (POSITION pos=m_TextureList.GetHeadPosition(); pos; m_TextureList.GetNext(pos))
	{
		CTextureD3D *pTexture=(CTextureD3D *)m_TextureList.GetAt(pos);
		CTextureD3D *pTheSame=pTexture->GetTheSame();
		if (pTheSame)
		{
			pTexture->Equal(pTheSame);
		//	*pTexture=*pTheSame;
		}
		else
		{
			int rgba=ddpf2cpf(&RGBAPixelFormat);
			int rgb=ddpf2cpf(&RGB_PixelFormat);
			pTexture->Build(rgb, rgba);
		}
	}
}

void CDirect3D::FreeAllTextures()
{
	GetD3D()->EvictManagedTextures();
	for (POSITION pos=m_TextureList.GetHeadPosition(); pos; m_TextureList.GetNext(pos))
	{
		CTextureD3D *pTexture=(CTextureD3D *)m_TextureList.GetAt(pos);
		delete pTexture;
	}
	m_TextureList.RemoveAll();
}

void CDirect3D::OnCameraPosChanged()
{
	C3DRenderer::OnCameraPosChanged();
	CalculateViewMatrix();	
}

void CDirect3D::OnCameraDirChanged()
{
	C3DRenderer::OnCameraDirChanged();
	CalculateViewMatrix();	
}

void CDirect3D::OnWorldPosChanged()
{
	C3DRenderer::OnWorldPosChanged();
	d3dWorldMatrix._41=D3DVAL(m_WorldX);
	d3dWorldMatrix._42=D3DVAL(m_WorldY);
	d3dWorldMatrix._43=D3DVAL(m_WorldZ);
}

void CDirect3D::OnWorldDirChanged()
{
	C3DRenderer::OnWorldDirChanged();
	CalculateWorldMatrix();	
}

// Функция чтения графического файла с преобразованием формата
// fileName	- имя файла изображения
// header	- указатель на заголовок
// rgb		- требуемый формат для изображения без полупрозрачности
// rgbа		- требуемый формат для изображения с полупрозрачностью
// order	- требуемый порядок строк в изображении

// В случае успеха функция возвращает указатель на буфер с изображением,
// В случае неудачи - возвращает 0 и заполняет нулями все элементы header.
unsigned char* cReadImage( char* fileName, cImageHeader* header,
						  int rgb, int rgba, int order ) 
{
	unsigned char* bits = 0 ;
	FILE* f = fopen( fileName,"rb" ) ;
 	if( !f ) return 0 ;

	int bitsOffset ;
	int sizeX, sizeY ;
	int components ;
	long size ;
	
	char* p = strrchr( fileName,'.') ;
	// TARGA ?
	if( !strcmp( p + 1,"tga") || !strcmp( p + 1,"TGA" ) )
	{
		TARGAHEADER core;
		fread( &core, 1, sizeof( core ), f ) ;

		bitsOffset = sizeof( core ) ;
		sizeX = core.Width   ;
		sizeY = core.Height  ;
		components = core.bpp / 8 ;
	
		size = components * sizeX * sizeY ;
		bits = new unsigned char[ size ] ; 
	}
	else // BMP ?
	if( !strcmp( p + 1,"bmp" ) || !strcmp( p + 1,"BMP" ) )
	{
		/*FUI_*/ BITMAPFILEHEADER bmfh ;
		/*FUI_*/ BITMAPINFOHEADER bmih ;

		fread( &bmfh, sizeof( bmfh ), 1, f ) ;
		fread( &bmih, sizeof( bmih ), 1, f ) ;

		components = bmih.biBitCount / 8 ;
		sizeX = bmih.biWidth  ;
		sizeY = bmih.biHeight ;
		bitsOffset = bmfh.bfOffBits ;

		size = components * sizeX * sizeY ;
		bits = new unsigned char[ size ] ; 
	}
	else
		return 0 ;

	fseek( f, bitsOffset, SEEK_SET ) ;
	fread( bits, size, 1, f ) ;
	fclose(f) ;

	if( order != cImageRowOrder_DownUp )
	{
		// Swap rows in buffer :
		unsigned length = components * sizeX ;
		unsigned char* temp = new unsigned char[ length ] ;
		for( unsigned row = 0 ; row < (unsigned)(sizeY/2); row++ )
		{
			unsigned char* b1 = bits + row * length ;
			unsigned char* b2 = bits + ( sizeY - 1 - row ) * length ;
			memcpy( temp, b1, length ) ;
			memcpy( b1,   b2, length ) ;
			memcpy( b2, temp, length ) ;
		}
		delete[] temp ;

		header->order = cImageRowOrder_DownUp ;
	}
	// If order == cImageRowOrder_UpDown - do nothing.
	else
		header->order = cImageRowOrder_UpDown ;

	header->format     = cpfUnknown ;
	header->components = components ;
	header->width      = sizeX      ;
	header->height     = sizeY      ;

	unsigned char* ret=NULL;
	size = sizeX * sizeY ;
	int i ;
	if( components == 3 )
	{
		switch( rgb )
		{
			case cpfRGB555 :// 16 bit  RGB, R:0x7c00, G:0x03e0, B:0x001f

				header->format = cpfRGB555 ;
				ret = new unsigned char[ 2 * size ] ;
				for( i = 0 ; i < size ; i++ )
				{
					int r = bits[ components * i + 2 ] ;
					int g = bits[ components * i + 1 ] ;
					int b = bits[ components * i + 0 ] ;

					short c = 
						( short( r >> 3 ) << 10 ) & 0x7c00 |
						( short( g >> 3 ) << 5  ) & 0x03e0 |
						( short( b >> 3 )       ) & 0x001f ;
					
					*( short* )( ret + 2 * i ) = c ;
				}
				break ;

			case cpfRGB565 :// 16 bit  RGB, R:0xf800, G:0x07e0, B:0x001f

				header->format = cpfRGB565 ;
				ret = new unsigned char[ 2 * size ] ;
				for( i = 0 ; i < size ; i++ )
				{
					int r = bits[ components * i + 2 ] ;
					int g = bits[ components * i + 1 ] ;
					int b = bits[ components * i + 0 ] ;

					short c = 
						( short( r >> 3 ) << 11 ) & 0xf800 |
						( short( g >> 2 ) << 5  ) & 0x07e0 |
						( short( b >> 3 )       ) & 0x001f ;
					
					*( short* )( ret + 2 * i ) = c ;
				}
				break ;

			case cpfRGB_24 :// 24 bit RGB, R:0xff0000, G:0x00ff00, B:0x0000ff

				header->format = cpfRGB_24 ;
				ret = bits ;

				break ;

			case cpfRGB_32 :// 32 bit RGB, R:0x00ff0000, G:0x0000ff00, B:0x000000ff

				header->format = cpfRGB_32 ;
				ret = new unsigned char[ 4 * size ] ;
				for( i = 0 ; i < size ; i++ )
				{
					ret[ 4 * i + 0 ] = bits[ components * i + 0 ] ; // blue
					ret[ 4 * i + 1 ] = bits[ components * i + 1 ] ; // green
					ret[ 4 * i + 2 ] = bits[ components * i + 2 ] ; // red
					ret[ 4 * i + 3 ] = 255 ;
				}
				break ;
		}
	}
	else
	if( components == 4 )
	{
		switch( rgba )
		{
			case cpfARGB1555 : // 16 bit ARGB, R:0x7c00, G:0x03e0, B:0x001f, A:0x8000

				header->format = cpfARGB1555 ;
				ret = new unsigned char[ 2 * size ] ;
				for( i = 0 ; i < size ; i++ )
				{
					int r = bits[ components * i + 2 ] ;
					int g = bits[ components * i + 1 ] ;
					int b = bits[ components * i + 0 ] ;
					int a = bits[ components * i + 3 ] ;

					short c = 
						( short( r >> 3 ) << 11 ) & 0xfc00 |
						( short( g >> 3 ) << 5  ) & 0x03e0 |
						( short( b >> 3 )       ) & 0x001f |
						( short( a > 125 ? 1 : 0  ) << 15 ) & 0x8000 ;
					
					*( short* )( ret + 2 * i ) = c ;
				}
				break ;

			case cpfARGB4444 : // 16 bit ARGB, R:0x0f00, G:0x00f0, B:0x000f, A:0xf000

				header->format = cpfARGB4444 ;
				ret = new unsigned char[ 2 * size ] ;
				for( i = 0 ; i < size ; i++ )
				{
					int r = bits[ components * i + 2 ] ;
					int g = bits[ components * i + 1 ] ;
					int b = bits[ components * i + 0 ] ;
					int a = bits[ components * i + 3 ] ;

					short c = 
						( short( r >> 4 ) << 8  ) & 0x0f00 |
						( short( g >> 4 ) << 4  ) & 0x00f0 |
						( short( b >> 4 )       ) & 0x000f |
						( short( a >> 4 ) << 12 ) & 0xf000 ;
					
					*( short* )( ret + 2 * i ) = c ;
				}
				break ;

			case cpfARGB8888 : // 32 bit ARGB, R:0x00ff0000, G:0x0000ff00, B:0x000000ff, A:0xff000000

				header->format = cpfARGB8888 ;
				ret = bits ;
				break ;

			case cpfRGBA8888 : // 32 bit RGBA, R:0xff000000, G:0x00ff0000, B:0x0000ff00, A:0x000000ff

				header->format = cpfRGBA8888 ;
				for( i = 0 ; i < size ; i++ )
				{
					unsigned char temp = bits[ components * i + 3 ] ;
//					bits[ components * i + 3 ] = bits[ components * i + 2 ] ;
//					bits[ components * i + 2 ] = bits[ components * i + 1 ] ;
//					bits[ components * i + 1 ] = bits[ components * i + 0 ] ;
//					bits[ components * i + 0 ] = temp ;

					temp = bits[ components * i + 0 ] ;
					bits[ components * i + 0 ] = bits[ components * i + 2 ] ;
					bits[ components * i + 2 ] = temp ;

				}
				ret = bits ;

				break ;
		}
	}
	delete[] bits ;
	return ret ;
}

bool CTextureD3D::PackTexture(LPDIRECTDRAWSURFACE4 lpds1, LPDIRECTDRAWSURFACE4 lpds2)
{
	HRESULT			  hr;
	DDSURFACEDESC2	dds1;
	ZeroMemory(&dds1,sizeof(DDSURFACEDESC2));
	dds1.dwSize = sizeof(DDSURFACEDESC2);
	
	DDSURFACEDESC2	dds2;
	ZeroMemory(&dds2,sizeof(DDSURFACEDESC2));
	dds2.dwSize = sizeof(DDSURFACEDESC2);

	if	((hr=lpds1->Lock(NULL, &dds1,DDLOCK_SURFACEMEMORYPTR|DDLOCK_READONLY,NULL))!=DD_OK)
	{
		TRACE1("Can't lock mipmap surface1 for <%s>\n", m_pszFileName);
		return false;
	}
	if	((hr=lpds2->Lock(NULL, &dds2,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WRITEONLY,NULL))!=DD_OK)
	{
		TRACE1("Can't lock mipmap surface2 for <%s>\n",  m_pszFileName);
		lpds1->Unlock(0);
		return false;
	}
	int	col,row;
	int	width  = dds2.dwWidth ;
	int	height = dds2.dwHeight;
	int	pitch2 = dds2.lPitch  ;
	int	pitch1 = dds1.lPitch  ;

	if	(IsAlpha())
	{
		switch(GetDirect3DObject()->GetRGBAcpf())
		{
			case cpfARGB1555 : // 16 bit ARGB, R:0x7c00, G:0x03e0, B:0x001f, A:0x8000
			{
				unsigned short *srow1 =(unsigned short*)dds1.lpSurface; pitch1>>=1;
				unsigned short *srow2 =(unsigned short*)dds2.lpSurface; pitch2>>=1;

				for	(row=0; row < height ;row++,srow1+=(pitch1+pitch1),srow2+=pitch2)
				{
					unsigned short *src = srow1;
					unsigned short *dst = srow2;

					for	(col=0; col < width ;col++,src+=2,dst++)
					{
						int r	=((int)((*(src         )>>10)&0x001F)
								+ (int)((*(src+       1)>>10)&0x001F)
								+ (int)((*(src+pitch1  )>>10)&0x001F)
								+ (int)((*(src+pitch1+1)>>10)&0x001F))>>2;
						int g	=((int)((*(src         )>> 5)&0x001F)
								+ (int)((*(src+       1)>> 5)&0x001F)
								+ (int)((*(src+pitch1  )>> 5)&0x001F)
								+ (int)((*(src+pitch1+1)>> 5)&0x001F))>>2;
						int b	=((int)( *(src         )     &0x001F)
								+ (int)( *(src+       1)     &0x001F)
								+ (int)( *(src+pitch1  )     &0x001F)
								+ (int)( *(src+pitch1+1)     &0x001F))>>2;
						int a;

						//if	(flag == 0)
						{
							a	=((int)( *(src         )     &0x8000)
								| (int)( *(src+       1)     &0x8000)
								| (int)( *(src+pitch1  )     &0x8000)
								| (int)( *(src+pitch1+1)     &0x8000))>>2;
						}
						/*else{
							a	=((int)( *(src         )     &0x8000)
								& (int)( *(src+       1)     &0x8000)
								& (int)( *(src+pitch1  )     &0x8000)
								& (int)( *(src+pitch1+1)     &0x8000))>>2;
							}*/
						*dst =(unsigned short)(a|(r<<10)|(g<<5)|b);
					}
				}
			}
			break;
			case cpfARGB4444 : // 16 bit ARGB, R:0x0f00, G:0x00f0, B:0x000f, A:0xf000
			{
				unsigned short *srow1 =(unsigned short*)dds1.lpSurface; pitch1>>=1;
				unsigned short *srow2 =(unsigned short*)dds2.lpSurface; pitch2>>=1;

				for	(row=0; row < height ;row++,srow1+=(pitch1+pitch1),srow2+=pitch2)
				{
					unsigned short *src = srow1;
					unsigned short *dst = srow2;

					for	(col=0; col < width ;col++,src+=2,dst++)
					{
						int r	=((int)((*(src         )>> 8)&0x000F)
								+ (int)((*(src+       1)>> 8)&0x000F)
								+ (int)((*(src+pitch1  )>> 8)&0x000F)
								+ (int)((*(src+pitch1+1)>> 8)&0x000F))>>2;
						int g	=((int)((*(src         )>> 4)&0x000F)
								+ (int)((*(src+       1)>> 4)&0x000F)
								+ (int)((*(src+pitch1  )>> 4)&0x000F)
								+ (int)((*(src+pitch1+1)>> 4)&0x000F))>>2;
						int b	=((int)( *(src         )     &0x000F)
								+ (int)( *(src+       1)     &0x000F)
								+ (int)( *(src+pitch1  )     &0x000F)
								+ (int)( *(src+pitch1+1)     &0x000F))>>2;
						int a	;

						//if	(flag == 0)
						{
							a	=((int)((*(src         )>>12)&0x000F)
								+ (int)((*(src+       1)>>12)&0x000F)
								+ (int)((*(src+pitch1  )>>12)&0x000F)
								+ (int)((*(src+pitch1+1)>>12)&0x000F))>>2;
						}
						/*else
						{	a	=(int)((*(src         )>>12)&0x000F);
							int a2	=(int)((*(src+       1)>>12)&0x000F);
							int a3	=(int)((*(src+pitch1  )>>12)&0x000F);
							int a4	=(int)((*(src+pitch1+1)>>12)&0x000F);
							if	(a > a2) a = a2;
							if	(a > a3) a = a3;
							if	(a > a4) a = a4;
						}*/
						*dst =(unsigned short)((a<<12)|(r<<8)|(g<<4)|b);
					}
				}
			}
			break;
			case cpfARGB8888 : // 32 bit ARGB, R:0x00ff0000, G:0x0000ff00, B:0x000000ff, A:0xff000000
			{
				unsigned int *srow1 =(unsigned int*)dds1.lpSurface; pitch1>>=2;
				unsigned int *srow2 =(unsigned int*)dds2.lpSurface; pitch2>>=2;

				for	(row=0; row < height ;row++,srow1+=(pitch1+pitch1),srow2+=pitch2)
				{
					unsigned int *src = srow1;
					unsigned int *dst = srow2;

					for	(col=0; col < width ;col++,src+=2,dst++)
					{
						unsigned int r	=(((*(src         )>>16)&0x00FF)
										+ ((*(src+       1)>>16)&0x00FF)
										+ ((*(src+pitch1  )>>16)&0x00FF)
										+ ((*(src+pitch1+1)>>16)&0x00FF))>>2;
						unsigned int g	=(((*(src         )>> 8)&0x00FF)
										+ ((*(src+       1)>> 8)&0x00FF)
										+ ((*(src+pitch1  )>> 8)&0x00FF)
										+ ((*(src+pitch1+1)>> 8)&0x00FF))>>2;
						unsigned int b	=(( *(src         )     &0x00FF)
										+ ( *(src+       1)     &0x00FF)
										+ ( *(src+pitch1  )     &0x00FF)
										+ ( *(src+pitch1+1)     &0x00FF))>>2;
						unsigned int a	;

						//if	(flag == 0)
						{
							a	=(((*(src         )>>24)&0x00FF)
								+ ((*(src+       1)>>24)&0x00FF)
								+ ((*(src+pitch1  )>>24)&0x00FF)
								+ ((*(src+pitch1+1)>>24)&0x00FF))>>2;
						}
						/*else
						{
							a	=((*(src         )>>24)&0x00FF);
							unsigned int a2 =((*(src+       1)>>24)&0x00FF);
							unsigned int a3	=((*(src+pitch1  )>>24)&0x00FF);
							unsigned int a4	=((*(src+pitch1+1)>>24)&0x00FF);
							if	(a > a2) a = a2;
							if	(a > a3) a = a3;
							if	(a > a4) a = a4;
						}*/
						*dst = (a<<24)|(r<<16)|(g<<8)|b;
					}
				}
			}
			break;
			case cpfRGBA8888 : // 32 bit RGBA, R:0xff000000, G:0x00ff0000, B:0x0000ff00, A:0x000000ff
			{
				unsigned int *srow1 =(unsigned int*)dds1.lpSurface; pitch1>>=2;
				unsigned int *srow2 =(unsigned int*)dds2.lpSurface; pitch2>>=2;
				
				for	(row=0; row < height ;row++,srow1+=(pitch1+pitch1),srow2+=pitch2)
				{
					unsigned int *src = srow1;
					unsigned int *dst = srow2;

					for	(col=0; col < width ;col++,src+=2,dst++)
					{
						unsigned int r	=(((*(src         )>>24)&0x00FF)
										+ ((*(src+       1)>>24)&0x00FF)
										+ ((*(src+pitch1  )>>24)&0x00FF)
										+ ((*(src+pitch1+1)>>24)&0x00FF))>>2;
						unsigned int g	=(((*(src         )>>16)&0x00FF)
										+ ((*(src+       1)>>16)&0x00FF)
										+ ((*(src+pitch1  )>>16)&0x00FF)
										+ ((*(src+pitch1+1)>>16)&0x00FF))>>2;
						unsigned int b	=(((*(src         )>> 8)&0x00FF)
										+ ((*(src+       1)>> 8)&0x00FF)
										+ ((*(src+pitch1  )>> 8)&0x00FF)
										+ ((*(src+pitch1+1)>> 8)&0x00FF))>>2;
						unsigned int a	;

						//if	(flag == 0)
						{
							a	=(( *(src         )     &0x00FF)
								+ ( *(src+       1)     &0x00FF)
								+ ( *(src+pitch1  )     &0x00FF)
								+ ( *(src+pitch1+1)     &0x00FF))>>2;
							}
						/*else
						{
							a	=(*(src         )&0x00FF);
							unsigned int a2 =(*(src+       1)&0x00FF);
							unsigned int a3	=(*(src+pitch1  )&0x00FF);
							unsigned int a4	=(*(src+pitch1+1)&0x00FF);
							if	(a > a2) a = a2;
							if	(a > a3) a = a3;
							if	(a > a4) a = a4;
						}*/
						*dst = (r<<24)|(g<<16)|(b<<8)|a;
					}
				}
			}
			break;
			default:
				TRACE0("Unknown RGBA texture format\n");
				lpds1->Unlock(0);
				lpds2->Unlock(0);
				return false;
		} // end switch
	}
	else
	{
		switch(GetDirect3DObject()->GetRGB_cpf())
		{
			case cpfRGB555 :// 16 bit  RGB, R:0x7c00, G:0x03e0, B:0x001f
			{
				unsigned short *srow1 =(unsigned short*)dds1.lpSurface; pitch1>>=1;
				unsigned short *srow2 =(unsigned short*)dds2.lpSurface; pitch2>>=1;

				for	(row=0; row < height ;row++,srow1+=(pitch1+pitch1),srow2+=pitch2)
				{
					unsigned short *src = srow1;
					unsigned short *dst = srow2;

					for	(col=0; col < width ;col++,src+=2,dst++)
					{
						int r	=((int)((*(src         )>>10)&0x001F)
								+ (int)((*(src+       1)>>10)&0x001F)
								+ (int)((*(src+pitch1  )>>10)&0x001F)
								+ (int)((*(src+pitch1+1)>>10)&0x001F))>>2;
						int g	=((int)((*(src         )>> 5)&0x001F)
								+ (int)((*(src+       1)>> 5)&0x001F)
								+ (int)((*(src+pitch1  )>> 5)&0x001F)
								+ (int)((*(src+pitch1+1)>> 5)&0x001F))>>2;
						int b	=((int)( *(src         )     &0x001F)
								+ (int)( *(src+       1)     &0x001F)
								+ (int)( *(src+pitch1  )     &0x001F)
								+ (int)( *(src+pitch1+1)     &0x001F))>>2;

						*dst =(unsigned short)((r<<10)|(g<<5)|b);
					}
				}
			}
			break;
			case cpfRGB565 :// 16 bit  RGB, R:0xf800, G:0x07e0, B:0x001f
			{
				unsigned short *srow1 =(unsigned short*)dds1.lpSurface; pitch1>>=1;
				unsigned short *srow2 =(unsigned short*)dds2.lpSurface; pitch2>>=1;

				for	(row=0; row < height ;row++,srow1+=(pitch1+pitch1),srow2+=pitch2)
				{
					unsigned short *src = srow1;
					unsigned short *dst = srow2;

					for	(col=0; col < width ;col++,src+=2,dst++)
					{
						int r	=((int)((*(src         )>>11)&0x001F)
								+ (int)((*(src+       1)>>11)&0x001F)
								+ (int)((*(src+pitch1  )>>11)&0x001F)
								+ (int)((*(src+pitch1+1)>>11)&0x001F))>>2;
						int g	=((int)((*(src         )>> 5)&0x003F)
								+ (int)((*(src+       1)>> 5)&0x003F)
								+ (int)((*(src+pitch1  )>> 5)&0x003F)
								+ (int)((*(src+pitch1+1)>> 5)&0x003F))>>2;
						int b	=((int)( *(src         )     &0x001F)
								+ (int)( *(src+       1)     &0x001F)
								+ (int)( *(src+pitch1  )     &0x001F)
								+ (int)( *(src+pitch1+1)     &0x001F))>>2;

						*dst =(unsigned short)((r<<11)|(g<<5)|b);
					}
				}
			}
			break;
			case cpfRGB_24 :// 24 bit RGB, R:0xff0000, G:0x00ff00, B:0x0000ff
			{
				unsigned char *srow1 =(unsigned char*)dds1.lpSurface;
				unsigned char *srow2 =(unsigned char*)dds2.lpSurface;

				for	(row=0; row < height ;row++,srow1+=(pitch1+pitch1),srow2+=pitch2)
				{
					unsigned char *src = srow1;
					unsigned char *dst = srow2;

					for	(col=0; col < width ;col++,src+=6,dst+=3)
					{
						int r	=((int)*(src         ) + (int)*(src+       3)
								+ (int)*(src+pitch1  ) + (int)*(src+pitch1+3))>>2;
						int g	=((int)*(src+       1) + (int)*(src+       4)
								+ (int)*(src+pitch1+1) + (int)*(src+pitch1+4))>>2;
						int b	=((int)*(src+       2) + (int)*(src+       5)
								+ (int)*(src+pitch1+2) + (int)*(src+pitch1+5))>>2;

						*(dst  ) =(unsigned char)r;
						*(dst+1) =(unsigned char)g;
						*(dst+2) =(unsigned char)b;
					}
				}
			}
			break;
			case cpfRGB_32 :// 32 bit RGB, R:0x00ff0000, G:0x0000ff00, B:0x000000ff
			{
				unsigned int *srow1 =(unsigned int*)dds1.lpSurface; pitch1>>=2;
				unsigned int *srow2 =(unsigned int*)dds2.lpSurface; pitch2>>=2;

				for	(row=0; row < height ;row++,srow1+=(pitch1+pitch1),srow2+=pitch2)
				{
					unsigned int *src = srow1;
					unsigned int *dst = srow2;

					for	(col=0; col < width ;col++,src+=2,dst++)
					{
						unsigned int r	=(((*(src         )>>16)&0x00FF)
										+ ((*(src+       1)>>16)&0x00FF)
										+ ((*(src+pitch1  )>>16)&0x00FF)
										+ ((*(src+pitch1+1)>>16)&0x00FF))>>2;
						unsigned int g	=(((*(src         )>> 8)&0x00FF)
										+ ((*(src+       1)>> 8)&0x00FF)
										+ ((*(src+pitch1  )>> 8)&0x00FF)
										+ ((*(src+pitch1+1)>> 8)&0x00FF))>>2;
						unsigned int b	=(( *(src         )     &0x00FF)
										+ ( *(src+       1)     &0x00FF)
										+ ( *(src+pitch1  )     &0x00FF)
										+ ( *(src+pitch1+1)     &0x00FF))>>2;

						*dst = 0xFF000000|(r<<16)|(g<<8)|b;
					}
				}
			}
			break;
			default: 
				TRACE0("Unknown RGB texture format\n");
				lpds1->Unlock(0);
				lpds2->Unlock(0);
				return false;
		} // end switch
	} // end if
	
	lpds2->Unlock(0);
	lpds1->Unlock(0);

	return true;
}

CTextureD3D::CTextureD3D(CDirect3D *pDirect3D, char *pszFileName, int MipmapLevels)
{
	m_pDirect3D=pDirect3D;
	m_MipmapLevels=MipmapLevels;
	m_bValid=false;
	m_pTheSame=NULL;
	if (pszFileName==NULL) return;
	if (pszFileName[0]==0) return;
	strcpy(m_pszFileName, pszFileName);
	for (size_t i=0; i<strlen(m_pszFileName); i++)
		tolower(m_pszFileName[i]);
	m_pTextureInterface=NULL;
	width=0;
	height=0;
	m_bAlpha=false;
	m_pSurface=NULL;
	m_bValid=true;
	pDirect3D->AddTexture(this);
}

bool CTextureD3D::Read(int rgb, int rgba)
{
	unsigned char  *image = 0;
	cImageHeader	header;

	if	((image = cReadImage(m_pszFileName, &header, rgb, rgba, cImageRowOrder_DownUp)) != NULL)
	{
		if	(((header.components == 3)&&(header.format == rgb ) )
			||	( (header.components == 4)&&(header.format == rgba)))
		{
			int	BytePerPixel;

			if	(header.components == 3)
				BytePerPixel = GetDirect3DObject()->RGB_PixelFormat.dwRGBBitCount>>3;
			else
				BytePerPixel = GetDirect3DObject()->RGBAPixelFormat.dwRGBBitCount>>3;

			width  = header.width ;
			height = header.height;

			m_bAlpha =(header.components == 4)? true:false;

		//	Создание текстурной поверхности ------------------------------------

			HRESULT	hr;
			DDSURFACEDESC2	ddsd;
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
			ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
			ddsd.ddsCaps.dwCaps2= DDSCAPS2_TEXTUREMANAGE;
			ddsd.dwWidth        =  width;
			ddsd.dwHeight       = height;
			if  (IsAlpha())
				ddsd.ddpfPixelFormat = GetDirect3DObject()->RGBAPixelFormat;
			else 
				ddsd.ddpfPixelFormat = GetDirect3DObject()->RGB_PixelFormat;

			if  ((hr = GetDD()->CreateSurface(&ddsd, &m_pSurface, NULL)) == DD_OK)
			{
				hr = m_pSurface->QueryInterface(IID_IDirect3DTexture2,(void**)&m_pTextureInterface);

				ZeroMemory(&ddsd,sizeof(ddsd));
				ddsd.dwSize = sizeof(ddsd);
				if	((hr = m_pSurface->Lock(NULL,&ddsd,DDLOCK_NOSYSLOCK
					| DDLOCK_SURFACEMEMORYPTR
					| DDLOCK_WRITEONLY,NULL)) == DD_OK)
				{
					unsigned char	*ip = image;
					unsigned char	*sp =(unsigned char *)ddsd.lpSurface;
					long		  pitch =				  ddsd.lPitch;

					for	(int i=0; i < height ;i++)
					{
						memcpy(sp,ip,width*BytePerPixel);

						ip+= width*BytePerPixel;
						sp+= pitch;
					}
					if	((hr = m_pSurface->Unlock(NULL)) == DD_OK)
					{
						delete [] image;
						return	   true;
					}
					TRACE1("Can't unlock memory surface for %s\n", m_pszFileName);
				}
				else
				{
					TRACE1("Can't lock memory surface for %s\n", m_pszFileName);
				}
			}
			else
			{
				TRACE1("Can't create surface for %ss\n", m_pszFileName);
			}
		}
		else
		{
			TRACE1("Format mismatch during %s loading\n", m_pszFileName);
		}
	}
	else
	{
		TRACE1("Can't read file %s\n", m_pszFileName);
	}
	if (image)
		delete[] image;

	Destroy();
	return false;
}

void CTextureD3D::Release()
{
	Destroy();
}

void CTextureD3D::Destroy()
{
	if (m_pTheSame==NULL && m_pSurface!=NULL)
		m_pSurface->Release();
	m_pSurface = NULL;
	if (m_pTheSame==NULL && m_pTextureInterface!=NULL)
		m_pTextureInterface->Release();
	m_pTextureInterface = NULL;

	width=0;
	height=0;
	m_bValid=false;
}

CTextureD3D::~CTextureD3D()
{
	Destroy();
}

bool CTextureD3D::IsEqual(CTextureD3D *pTexture)
{
	if (strcmp(m_pszFileName, pTexture->m_pszFileName))
		return false;
	else
		return true;
}

void CTextureD3D::Equal(CTextureD3D *pTexture)
{
	m_bValid=pTexture->m_bValid;
	strcpy(m_pszFileName, pTexture->m_pszFileName);
	width=pTexture->width;
	height=pTexture->height;
	m_pSurface=pTexture->m_pSurface;
	m_pTextureInterface=pTexture->m_pTextureInterface;
	m_bAlpha=pTexture->m_bAlpha;
	m_pTheSame=pTexture;
}

void CTextureD3D::Build(int rgb, int rgba)
{
	Destroy();
	m_bValid=Read(rgb, rgba);
	CreateMipmapping();
}

bool CTextureD3D::CreateMipmapping()
{
//	Программа создает mipmap-овскую текстуру по ранее созданной. Прототип
//	уничтожается.
	if (!IsValid()) return false;
	if (m_MipmapLevels<=1) return true; // Мипмаппинг не требуется

	HRESULT					  hr;
	LPDIRECTDRAWSURFACE4	lpds;
	LPDIRECTDRAWSURFACE4	lpm1;
	LPDIRECTDRAWSURFACE4	lpm2;
	LPDIRECT3DTEXTURE2		lpdt;
	DDSURFACEDESC2			ddsd; 
	ZeroMemory(&ddsd,sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);

	int i;
	// Ищем меньший размер (ведь вдруг текстура слишком мала, чтобы обеспечить столько уровней мипмаппинга)
	unsigned short MinSize=width;
	if (height<MinSize) MinSize=height;
	for (i=0; i<=m_MipmapLevels; i++)
	{
		MinSize>>=1;
		if (MinSize==1)
		{
			m_MipmapLevels=i;
			break;
		}
	}

	ddsd.dwFlags         = DDSD_CAPS|DDSD_MIPMAPCOUNT|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE|DDSCAPS_MIPMAP|DDSCAPS_COMPLEX;
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	ddsd.dwMipMapCount   = m_MipmapLevels;
	ddsd.dwWidth         = width;
	ddsd.dwHeight        = height;
	if  (IsAlpha())
		ddsd.ddpfPixelFormat = GetDirect3DObject()->RGBAPixelFormat;
	else 
		ddsd.ddpfPixelFormat = GetDirect3DObject()->RGB_PixelFormat;

	if	((hr=GetDD()->CreateSurface(&ddsd,&lpds,NULL))!=DD_OK)
	{
		TRACE1("Create mipmap texture <%s> surface failed\n", m_pszFileName);
		return false;
	}
	if	((hr=lpds->QueryInterface(IID_IDirect3DTexture2, (void**)&lpdt))!=DD_OK)
	{
		TRACE1("QueryInterface mipmap texture <%s> failed\n", m_pszFileName);
		lpds->Release();
		return false;
	}
	if	((hr=lpds->BltFast(0,0, m_pSurface, NULL,DDBLTFAST_WAIT))!=DD_OK)
	{
		TRACE1("Can't BltFast mipmap texture surface for %s\n", m_pszFileName);
		lpdt->Release();
		lpds->Release();
		return false;
	}

	for	(i=1, lpm1=lpds; i < m_MipmapLevels; i++, lpm1=lpm2)
	{
		DDSCAPS2 ddsc; 
		ddsc.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;

		if	((hr=lpm1->GetAttachedSurface(&ddsc,&lpm2))!=DD_OK)
		{
			TRACE1("Create mipmap image <%s> failed\n", m_pszFileName);
			lpdt->Release();
			lpds->Release();
			return false;
		}
		if	(!PackTexture(lpm1,lpm2))
		{
			lpdt->Release();
			lpds->Release();
			return false;
		}
	}
	Destroy();
	m_bValid=true;

	m_pSurface	= lpds;
	m_pTextureInterface= lpdt;

	return true;
}

// Helper functions
void TransformVector(D3DMATRIX *pMatrix, float &x, float &y, float &z)
{
	float x1=x*pMatrix->_11+y*pMatrix->_21+z*pMatrix->_31+/*1.f**/pMatrix->_41;
	float y1=x*pMatrix->_12+y*pMatrix->_22+z*pMatrix->_32+/*1.f**/pMatrix->_42;
	float z1=x*pMatrix->_13+y*pMatrix->_23+z*pMatrix->_33+/*1.f**/pMatrix->_43;
	x=x1; y=y1; z=z1;
}

void TransformVector(D3DMATRIX *pMatrix, float &x, float &y, float &z, float &w)
{
	float x1=x*pMatrix->_11+y*pMatrix->_21+z*pMatrix->_31+/*1.f**/pMatrix->_41;
	float y1=x*pMatrix->_12+y*pMatrix->_22+z*pMatrix->_32+/*1.f**/pMatrix->_42;
	float z1=x*pMatrix->_13+y*pMatrix->_23+z*pMatrix->_33+/*1.f**/pMatrix->_43;
	float w1=x*pMatrix->_14+y*pMatrix->_24+z*pMatrix->_34+/*1.f**/pMatrix->_44;
	x=x1; y=y1; z=z1; w=w1;
}

D3DMATRIX MatrixMult(D3DMATRIX &Multiplied, D3DMATRIX &Multiplier)
{
	D3DMATRIX ret;
	ZeroMemory(&ret, sizeof(ret));
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++) {
			for (int k=0; k<4; k++) {
				ret(i, j) += Multiplied(k, j) * Multiplier(i, k);
			}
		}
	}
	return ret;
}



