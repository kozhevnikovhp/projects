#ifndef __DIRECTX__
#define __DIRECTX__

#include "3DRenderer.h"
#include "objlists.h"

#define D3D_OVERLOADS
#include <d3d.h> 
#define MAX_DEVICE_NAME       256 
#define MAX_DEVICE_DESC       256

#pragma pack(push,1)

// Константы для определения формата хранения пикселов
typedef enum cPixelFormat_e {
	cpfUnknown,				// Формат не определен
	cpfRGB555,				// 16 bit  RGB, R:0x7c00, G:0x03e0, B:0x001f
	cpfRGB565,				// 16 bit  RGB, R:0xf800, G:0x07e0, B:0x001f
	cpfARGB1555,			// 16 bit ARGB, R:0x7c00, G:0x03e0, B:0x001f, A:0x8000
	cpfARGB4444,			// 16 bit ARGB, R:0x0f00, G:0x00f0, B:0x000f, A:0xf000

	cpfRGB_24,				// 24 bit RGB, R:0xff0000, G:0x00ff00, B:0x0000ff
	
	cpfRGB_32,				// 32 bit RGB, R:0x00ff0000, G:0x0000ff00, B:0x000000ff
	cpfARGB8888,			// 32 bit ARGB, R:0x00ff0000, G:0x0000ff00, B:0x000000ff, A:0xff000000
	cpfRGBA8888,			// 32 bit ARGB, R:0xff000000, G:0x00ff0000, B:0x0000ff00, A:0x000000ff

	cpfIndex				// 8 bit, палитра
};

// Константы для определения порядка хранения строк изображения
typedef enum cImageRowOrder_e {
	cImageRowOrder_UpDown,		// Строки в буфере изображения располагаются сверху вниз
	cImageRowOrder_DownUp		// Строки в буфере изображения располагаются снизу вверх
};



struct cImageHeader {
	cPixelFormat_e		format;				// Формат хранения пикселов
	cImageRowOrder_e	order;				// Порядок строк в изображении
	unsigned short		components;			// Количество компонент: 3 - RGB, 4 - RGBA
	unsigned short		width;				// Ширина
	unsigned short		height;				// Высота

} ;

struct TARGAHEADER 
{
	unsigned char IDlength;		// ID field length
	unsigned char ColorMapType;	// 1 - contain, 0 - not
	unsigned char ImgType;		//	0 - no image present
							//  1 - color-mapped, uncompressed
							//  2 - true color, uncompressed
							//  3 - b/w, uncompressed 
							//  9 - color-mapped, RLE compressed
							// 10 - true color, compressed
							// 11 - b/w, compressed 
	short FirstColorMapEntry;	//
	short ColorMapLength;		//
	unsigned char ColorMapEntrySize;	// 15,16,24,32
	short Xorg;				// position of lower left corner image
	short Yorg;				// (0,0) - lower left corner of the screen
	short Width;
	short Height;
	unsigned char bpp;				// 8,16,24,32
	unsigned char bits;				// image descriptor bits
								// bits 0-3 number of attribute bits per pixel
								// bits 4,5 :
								//			- 00 -left to right, bottom to top
								//			- 10 -right to left, bottom to top
								//			- 20 -left to right, top to bottom
								//			- 30 -right to left, top to bottom
								// bits 6,7 :
								//			- 00 -noninterleaved
								//			- 40 -even-odd two way interleaving
								//			- 80 -four way interleaving
} ;

#pragma pack(pop)

class CTextureD3D;
class CDirect3D : public C3DRenderer
{
	//public overridable methods
public:
	virtual void OnSize(int cx, int cy);
	virtual void OnPaint();
	virtual void Redraw();

	virtual void OnCameraPosChanged();
	virtual void OnCameraDirChanged();
	virtual void OnWorldPosChanged();
	virtual void OnWorldDirChanged();

	//protected overridable methods
protected:
	virtual BOOL OnInitScene() {return TRUE;}
	virtual BOOL OnRenderScene() {return TRUE;}

	//public methods
public:
	CDirect3D(DWORD hWnd);
	virtual ~CDirect3D();
	inline int GetRGBAcpf() { return ddpf2cpf(&RGBAPixelFormat); }
	inline int GetRGB_cpf() { return ddpf2cpf(&RGB_PixelFormat); }
	void Point3DToPoint2D(float x3D, float y3D, float z3D, float &x2D, float &y2D, float &w);
	void Point3DToPoint2D(float x3D, float y3D, float z3D, float &x2D, float &y2D, float &w, D3DMATRIX *pProjViewMatrix);
	void GetProjViewWorldMatrix(D3DMATRIX &ResultMatrix); // Product projection, view and world matrices
	void GetProjViewMatrix(D3DMATRIX &ResultMatrix); // Product projection and view matrices
	void TransformWorld(const D3DMATRIX &TransformMatrix);
	void SetWorldMatrix(const D3DMATRIX &WorldMatrix);
	D3DMATRIX GetWorldMatrix() {return d3dWorldMatrix;}
	void ModifyLight(int LightToModify, D3DLIGHT *pLight);
	bool GetLightInfo(int LightToGet, D3DLIGHT *pLight);
	inline LPDIRECTDRAW4 GetDD() {return m_pDD;}
	inline LPDIRECT3D3 GetD3D() {return m_pD3D;}
	inline LPDIRECT3DDEVICE3 GetDevice() {return m_pD3DDevice;}
	inline LPDIRECT3DVIEWPORT3 GetViewport() {return m_pD3DViewport;}
	BOOL OnDraw();
	void MoveCamera(float dX, float dY, float dZ, BOOL bRedraw=TRUE);
	void BeforeTexture(double diffuseR, double diffuseG, double diffuseB, double diffuseA,
		double ambientR, double ambientG, double ambientB, double ambientA,
		double specularR, double specularG, double specularB, double specularA,
		double power, DWORD RampSize);
	void AfterTexture();
	void SetBackGround(double Red, double Green, double Blue);
	void SetPerspectiveProjection(float CameraAngle, float NearPlane, float FarPlane);
	HRESULT RebuildLights();

	//Protected methods
protected:
	HRESULT PrepareScene(void);
	HRESULT ReleaseScene(void);
	HRESULT UpdateViewport(void);
	void CalculateNormals(D3DVERTEX *pVertex);
	HRESULT Material(double diffuseR, double diffuseG, double diffuseB, double diffuseA,
		double ambientR, double ambientG, double ambientB, double ambientA,
		double specularR, double specularG, double specularB, double specularA,
		double power, DWORD RampSize);
	int AddLight(D3DLIGHTTYPE type,
					double red, double green, double blue, double alfa,
					double pX, double pY, double pZ, //position
					double dX, double dY, double dZ, //direction
					double att0, double att1, double att2);
	int AddLight(D3DLIGHT *pLight);
	HRESULT ReleaseLights();
	HRESULT UpdateBackGround();

	//Private methods
private:
	int ddpf2cpf(DDPIXELFORMAT* ddpf);
	void RecalcDestRect();
	void CalculateViewMatrix();
	void CalculateWorldMatrix();
	void MultWorldMatrix(const D3DMATRIX &Multiplier);
	DWORD FlagsToBitDepth(DWORD dwFlags);
	HRESULT CreateDevice(DWORD dwWidth, DWORD dwHeight);
	HRESULT RestoreDevice(void);
	HRESULT ReleaseDevice(void); 
	HRESULT CreateDirect3D(void);
	HRESULT ReleaseDirect3D(void);
	HRESULT CreateFrontBuffer(void);
	HRESULT CreateZBuffer(void);
	HRESULT RestoreFront(void);
	HRESULT ChooseDevice(void);
	HRESULT ReleaseFront(void);
	LRESULT RestoreSurfaces(void);
	DWORD BitDepthToFlags(DWORD dwBitDepth);
	HRESULT RenderScene(void);
	static HRESULT WINAPI EnumDeviceCallback(LPGUID			lpGUID,
                                      LPSTR				lpszDeviceDesc,
                                      LPSTR				lpszDeviceName,
                                      LPD3DDEVICEDESC	lpd3dHWDeviceDesc,
                                      LPD3DDEVICEDESC	lpd3dSWDeviceDesc,
                                      LPVOID			lpUserArg);
	static HRESULT WINAPI EnumZBufferFormatsCallback(DDPIXELFORMAT *, VOID *);
	static HRESULT WINAPI FindTextureCallback (DDPIXELFORMAT *ddpf, VOID *lParam);
	//Public members //wil be moved to private
public:
	void FreeAllTextures();
	bool AddTexture(CTextureD3D *pTexture);
	static DDPIXELFORMAT RGB_PixelFormat;// Pixel format for RGB texture.
	static DDPIXELFORMAT RGBAPixelFormat;// Pixel format for RGBA texture.
	static DDPIXELFORMAT  ddpfZBuffer; // Формат Z-буфера(глубина).

	//Protected members
protected:
	void ReleaseTextures();
	void RebuildTextures();
	D3DMATRIX d3dWorldMatrix;		  

	//private members
private:
	LPDIRECT3DMATERIAL3 m_pD3DCurrentMaterial;
	LPDIRECT3DMATERIAL3 m_pD3DBackMaterial;
	CPtrList m_TextureList;
	D3DMATERIALHANDLE m_hCurrentMaterial;
	D3DMATRIX d3dViewMatrix;
	D3DMATRIX d3dProjMatrix;
	static BOOL fDebug;
	static BOOL bIsHardware;
	static BOOL bIsBlendTexture;// Наличие RGBAPixelFormat.
	BOOL fActive;
	BOOL fSuspended;
	static BOOL fDeviceFound;
	static DWORD dwDeviceBitDepth;
	static GUID guidDevice;
	LPDIRECTDRAW4 m_pDD;
	LPDIRECTDRAWSURFACE4 m_pDDFrontBuffer;
	LPDIRECTDRAWSURFACE4 m_pDDBackBuffer;
	LPDIRECTDRAWSURFACE4 m_pDDZBuffer;
	LPDIRECTDRAWPALETTE m_pDDPalette;
 
// Direct3D interfaces 
	LPDIRECT3D3 m_pD3D;
	LPDIRECT3DDEVICE3 m_pD3DDevice;
	LPDIRECT3DVIEWPORT3 m_pD3DViewport;
 
// Globals used for selecting the Direct3D device. They are 
// globals because this makes it easy for the enumeration callback 
// function to read and write from them. 
	static char szDeviceName[MAX_DEVICE_NAME]; 
	static char szDeviceDesc[MAX_DEVICE_DESC]; 
	static D3DDEVICEDESC d3dHWDeviceDesc; 
	static D3DDEVICEDESC d3dSWDeviceDesc;
 
// The screen coordinates of the client area of the window. This 
// rectangle defines the destination into which we blit to update 
// the client area of the window with the results of the 3-D rendering. 
	RECT rDstRect; 

// This rectangle defines the portion of the rendering target surface 
// into which we render. The top-left coordinates of this rectangle 
// are always zero; the right and bottom coordinates give the size of 
// the viewport. 
	RECT rSrcRect; 
 
	//BackGround color
	double m_BackRed, m_BackGreen, m_BackBlue;
	
	int m_LightCount;
	D3DLIGHT m_Lights[10];
};

class CTextureD3D
{
	CDirect3D *m_pDirect3D;
	LPDIRECTDRAWSURFACE4 m_pSurface;		// Texture surface for managed texture
	LPDIRECT3DTEXTURE2 m_pTextureInterface;
	bool m_bValid;
	char m_pszFileName[256];
	CTextureD3D *m_pTheSame;
	bool m_bAlpha;	// Необходима прозрачность
	unsigned short width;				// Width
	unsigned short height;				// Height
	int m_MipmapLevels;

protected :
	inline CDirect3D *GetDirect3DObject() { return m_pDirect3D; }
	inline LPDIRECTDRAW4 GetDD() {return GetDirect3DObject()->GetDD();}
	inline LPDIRECT3D3 GetD3D() {return GetDirect3DObject()->GetD3D();}
	inline LPDIRECT3DDEVICE3 GetDevice() {return GetDirect3DObject()->GetDevice();}
	inline LPDIRECT3DVIEWPORT3 GetViewport() {return GetDirect3DObject()->GetViewport();}
	int ddpf2cpf(DDPIXELFORMAT* ddpf);
	void Destroy();				// Освобождение ресурсов текстуры.
	bool Read(int rgb, int rgba);
	bool PackTexture(LPDIRECTDRAWSURFACE4 lpds1, LPDIRECTDRAWSURFACE4 lpds2);
public :
	CTextureD3D(CDirect3D *pDirect3D, char *pszFileName, int MipmapLevels);
	void Build(int rgb, int rgba);
	inline bool IsAlpha() {return m_bAlpha;}
	inline LPDIRECT3DTEXTURE2 GetTextureInterface() { return m_pTextureInterface;}
	inline char *GetFileName() {return m_pszFileName;}
	virtual ~CTextureD3D();
	inline bool IsValid() { return m_bValid; }
	bool IsEqual(CTextureD3D *pTexture);
	void Equal(CTextureD3D *pTexture);
	bool CreateMipmapping();
	inline CTextureD3D *GetTheSame() {return m_pTheSame;}
	inline void SetTheSame(CTextureD3D *pTexture) {m_pTheSame=pTexture;}
	void Release();
};

// Helper functions
	void TransformVector(D3DMATRIX *pMatrix, float &x, float &y, float &z);
	void TransformVector(D3DMATRIX *pMatrix, float &x, float &y, float &z, float &w);
	D3DMATRIX MatrixMult(D3DMATRIX &Multiplied, D3DMATRIX &Multiplier);
#endif //__DIRECTX__