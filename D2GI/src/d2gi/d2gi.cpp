
#include "../common.h"

#include "d2gi.h"
#include "d2gi_ddraw.h"
#include "d2gi_prim_flip_surf.h"
#include "d2gi_backbuf_surf.h"
#include "d2gi_palette_blitter.h"
#include "d2gi_prim_single_surf.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi_texture.h"


D2GI::D2GI()
	: m_hD3D9Lib(NULL), m_pD3D9(NULL), m_pDev(NULL), m_eRenderState(RS_UNKNOWN)
{
	m_pDirectDrawProxy = new D2GIDirectDraw(this);
	m_pPaletteBlitter = new D2GIPaletteBlitter(this);

	LoadD3D9Library();
}


D2GI::~D2GI()
{
	DEL(m_pPaletteBlitter);
	RELEASE(m_pDev);
	RELEASE(m_pD3D9);
	FreeLibrary(m_hD3D9Lib);
}


VOID D2GI::OnDirectDrawReleased()
{
	delete this;
}


VOID D2GI::LoadD3D9Library()
{
	typedef D3D9::IDirect3D9* (WINAPI* DIRECT3DCREATE9)(UINT);

	TCHAR           szPath[MAX_PATH];
	DIRECT3DCREATE9 pfnDirect3DCreate9;

	GetSystemDirectory(szPath, MAX_PATH);
	_tcscat(szPath, TEXT("\\d3d9.dll"));

	m_hD3D9Lib = LoadLibrary(szPath);

	pfnDirect3DCreate9 = (DIRECT3DCREATE9)GetProcAddress(m_hD3D9Lib, "Direct3DCreate9");

	m_pD3D9 = pfnDirect3DCreate9(D3D_SDK_VERSION);
}


VOID D2GI::OnCooperativeLevelSet(HWND hWnd, DWORD dwFlags)
{
	m_hWnd = hWnd;
}


VOID D2GI::OnDisplayModeSet(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwFlags)
{
	m_dwOriginalWidth = dwWidth;
	m_dwOriginalHeight = dwHeight;
	m_dwOriginalBPP = dwBPP;

	ResetD3D9Device();
}


VOID D2GI::ReleaseResources()
{
	m_pDirectDrawProxy->ReleaseResources();
	m_pPaletteBlitter->ReleaseResource();
}


VOID D2GI::LoadResources()
{
	m_pDirectDrawProxy->LoadResources();
	m_pPaletteBlitter->LoadResource();
}


VOID D2GI::ResetD3D9Device()
{
	D3D9::D3DPRESENT_PARAMETERS sParams;

	ReleaseResources();
	RELEASE(m_pDev);

	SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, 0);
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, m_dwOriginalWidth, m_dwOriginalHeight, 0);

	ZeroMemory(&sParams, sizeof(sParams));
	sParams.AutoDepthStencilFormat = D3D9::D3DFMT_D24X8;
	sParams.EnableAutoDepthStencil = TRUE;
	sParams.BackBufferCount = 1;
	sParams.BackBufferWidth = m_dwOriginalWidth;
	sParams.BackBufferHeight = m_dwOriginalHeight;
	sParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	sParams.SwapEffect = D3D9::D3DSWAPEFFECT_DISCARD;
	sParams.Windowed = TRUE;

	m_pD3D9->CreateDevice(0, D3D9::D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &sParams, &m_pDev);
	LoadResources();
}


VOID D2GI::OnViewportSet(D3D7::LPD3DVIEWPORT7 pVP)
{
	D3D9::D3DVIEWPORT9 sD3D9Viewport;

	sD3D9Viewport.X = pVP->dwX;
	sD3D9Viewport.Y = pVP->dwY;
	sD3D9Viewport.Width = pVP->dwWidth;
	sD3D9Viewport.Height = pVP->dwHeight;
	sD3D9Viewport.MinZ = pVP->dvMinZ;
	sD3D9Viewport.MaxZ = pVP->dvMaxZ;

	m_pDev->SetViewport(&sD3D9Viewport);
}


VOID D2GI::OnBackBufferLock()
{
	m_eRenderState = RS_BACKBUFFER_STREAMING;
}


VOID D2GI::OnFlip()
{
	if (m_eRenderState == RS_BACKBUFFER_STREAMING)
	{
		D2GIPrimaryFlippableSurface* pPrimSurf = m_pDirectDrawProxy->GetPrimaryFlippableSurface();
		D3D9::IDirect3DSurface9* pSurf = pPrimSurf->GetBackBufferSurface()->GetD3D9Surface();
		D3D9::IDirect3DSurface9* pRT;

		m_pDev->GetRenderTarget(0, &pRT);
		m_pDev->StretchRect(pSurf, NULL, pRT, NULL, D3D9::D3DTEXF_POINT);
		m_pDev->Present(NULL, NULL, NULL, NULL);

		pRT->Release();
	}
	else if (m_eRenderState == RS_BACKBUFFER_BLITTING)
	{
		m_pDev->Present(NULL, NULL, NULL, NULL);
	}
}


VOID D2GI::OnSysMemSurfaceBltOnPrimarySingle(D2GISystemMemorySurface* pSrc, RECT* pSrcRT, D2GIPrimarySingleSurface* pDst, RECT* pDstRT)
{
	D3D9::IDirect3DSurface9* pRT;

	m_eRenderState = RS_PRIMARY_SURFACE_BLITTING;

	if (m_dwOriginalBPP == 8)
	{
		pSrc->UpdateWithPalette(pDst->GetPalette());
		m_pDev->GetRenderTarget(0, &pRT);
		m_pDev->StretchRect(pSrc->GetD3D9Surface(), pSrcRT, pRT, pDstRT, D3D9::D3DTEXF_POINT);
		m_pDev->Present(NULL, NULL, NULL, NULL);

		pRT->Release();
	}
}


VOID D2GI::OnClear(DWORD dwCount, D3D7::LPD3DRECT lpRects, DWORD dwFlags, D3D7::D3DCOLOR col, D3D7::D3DVALUE z, DWORD dwStencil)
{
	m_pDev->Clear(dwCount, (D3D9::D3DRECT*)lpRects, dwFlags, col, z, dwStencil);
}


VOID D2GI::OnLightEnable(DWORD i, BOOL bEnable)
{
	m_pDev->LightEnable(i, bEnable);
}


VOID D2GI::OnSysMemSurfaceBltOnBackBuffer(D2GISystemMemorySurface* pSrc, RECT* pSrcRT, D2GIBackBufferSurface* pDst, RECT* pDstRT)
{
	D3D9::IDirect3DSurface9* pRT;

	m_eRenderState = RS_BACKBUFFER_BLITTING;

	m_pDev->GetRenderTarget(0, &pRT);
	m_pDev->StretchRect(pSrc->GetD3D9Surface(), pSrcRT, pRT, pDstRT, D3D9::D3DTEXF_POINT);

	pRT->Release();
}


VOID D2GI::OnSysMemSurfaceBltOnTexture(D2GISystemMemorySurface* pSrc, RECT* pSrcRT, D2GITexture* pDst, RECT* pDstRT)
{
	m_pDev->StretchRect(pSrc->GetD3D9Surface(), pSrcRT, pDst->GetD3D9Surface(), pDstRT, D3D9::D3DTEXF_POINT);
}


VOID D2GI::OnSceneBegin()
{
	m_pDev->BeginScene();
}


VOID D2GI::OnSceneEnd()
{
	m_pDev->EndScene();
}


VOID D2GI::OnRenderStateSet(D3D7::D3DRENDERSTATETYPE eState, DWORD dwValue)
{
	switch (eState)
	{
		case D3D7::D3DRENDERSTATE_CULLMODE:
			m_pDev->SetRenderState(D3D9::D3DRS_CULLMODE, dwValue);
			break;
		default:
			return;
	}
}


VOID D2GI::OnTextureStageSet(DWORD i, D3D7::D3DTEXTURESTAGESTATETYPE eState, DWORD dwValue)
{
	D3D9::D3DTEXTUREFILTERTYPE aeMagTexFMap[] =
	{
		D3D9::D3DTEXF_NONE,
		D3D9::D3DTEXF_POINT,
		D3D9::D3DTEXF_LINEAR,
		D3D9::D3DTEXF_PYRAMIDALQUAD,
		D3D9::D3DTEXF_GAUSSIANQUAD,
		D3D9::D3DTEXF_ANISOTROPIC,
	};

	D3D9::D3DTEXTUREFILTERTYPE aeMinTexFMap[] =
	{
		D3D9::D3DTEXF_NONE,
		D3D9::D3DTEXF_POINT,
		D3D9::D3DTEXF_LINEAR,
		D3D9::D3DTEXF_ANISOTROPIC,
	};

	D3D9::D3DTEXTUREFILTERTYPE aeMipTexFMap[] =
	{
		D3D9::D3DTEXF_NONE,
		D3D9::D3DTEXF_POINT,
		D3D9::D3DTEXF_LINEAR,
	};

	switch (eState)
	{
		case D3D7::D3DTSS_COLOROP:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_COLOROP, dwValue);
			break;
		case D3D7::D3DTSS_COLORARG1:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_COLORARG1, dwValue);
			break;
		case D3D7::D3DTSS_COLORARG2:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_COLORARG2, dwValue);
			break;
		case D3D7::D3DTSS_ALPHAOP:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_ALPHAOP, dwValue);
			break;
		case D3D7::D3DTSS_ALPHAARG1:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_ALPHAARG1, dwValue);
			break;
		case D3D7::D3DTSS_ALPHAARG2:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_ALPHAARG2, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVMAT00:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVMAT00, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVMAT01:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVMAT01, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVMAT10:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVMAT10, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVMAT11:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVMAT11, dwValue);
			break;
		case D3D7::D3DTSS_TEXCOORDINDEX:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_TEXCOORDINDEX, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVLSCALE:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVLSCALE, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVLOFFSET:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVLOFFSET, dwValue);
			break;
		case D3D7::D3DTSS_TEXTURETRANSFORMFLAGS:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_TEXTURETRANSFORMFLAGS, dwValue);
			break;

		case D3D7::D3DTSS_ADDRESS:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_ADDRESSU, dwValue);
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_ADDRESSV, dwValue);
			break;
		case D3D7::D3DTSS_ADDRESSU:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_ADDRESSU, dwValue);
			break;
		case D3D7::D3DTSS_ADDRESSV:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_ADDRESSV, dwValue);
			break;
		case D3D7::D3DTSS_BORDERCOLOR:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_BORDERCOLOR, dwValue);
			break;
		case D3D7::D3DTSS_MAGFILTER:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MAGFILTER, aeMagTexFMap[dwValue]);
			break;
		case D3D7::D3DTSS_MINFILTER:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MINFILTER, aeMinTexFMap[dwValue]);
			break;
		case D3D7::D3DTSS_MIPFILTER:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MIPFILTER, aeMipTexFMap[dwValue]);
			break;
		case D3D7::D3DTSS_MIPMAPLODBIAS:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MIPMAPLODBIAS, dwValue);
			break;
		case D3D7::D3DTSS_MAXMIPLEVEL:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MAXMIPLEVEL, dwValue);
			break;
		case D3D7::D3DTSS_MAXANISOTROPY:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MAXANISOTROPY, dwValue);
			break;
		default:
			return;
	}
}


VOID D2GI::OnTextureSet(DWORD i, D2GITexture* pTex)
{
	m_pDev->SetTexture(i, pTex == NULL ? NULL : pTex->GetD3D9Texture());
}


BOOL D2GI::OnDeviceValidate(DWORD* pdw)
{
	return SUCCEEDED(m_pDev->ValidateDevice(pdw));
}


VOID D2GI::OnTransformSet(D3D7::D3DTRANSFORMSTATETYPE eType, D3D7::D3DMATRIX* pMatrix)
{
	switch (eType)
	{
		case D3D7::D3DTRANSFORMSTATE_WORLD:
			m_pDev->SetTransform(D3D9::D3DTS_WORLD, (D3D9::D3DMATRIX*)pMatrix);
			break;
		case D3D7::D3DTRANSFORMSTATE_WORLD1:
			m_pDev->SetTransform(D3D9::D3DTS_WORLD1, (D3D9::D3DMATRIX*)pMatrix);
			break;
		case D3D7::D3DTRANSFORMSTATE_WORLD2:
			m_pDev->SetTransform(D3D9::D3DTS_WORLD2, (D3D9::D3DMATRIX*)pMatrix);
			break;
		case D3D7::D3DTRANSFORMSTATE_WORLD3:
			m_pDev->SetTransform(D3D9::D3DTS_WORLD3, (D3D9::D3DMATRIX*)pMatrix);
			break;
		default:
			m_pDev->SetTransform((D3D9::D3DTRANSFORMSTATETYPE)eType, (D3D9::D3DMATRIX*)pMatrix);
			break;
	}
}


VOID D2GI::OnLightSet(DWORD i, D3D7::LPD3DLIGHT7 pLight)
{
	m_pDev->SetLight(i, (D3D9::D3DLIGHT9*)pLight);
}


VOID D2GI::OnMaterialSet(D3D7::LPD3DMATERIAL7 pMaterial)
{
	m_pDev->SetMaterial((D3D9::D3DMATERIAL9*)pMaterial);
}


VOID D2GI::OnClipStatusSet(D3D7::LPD3DCLIPSTATUS pStatus)
{
	D3D9::D3DCLIPSTATUS9 sStatus9;

	if (pStatus->dwFlags != D3DCLIPSTATUS_STATUS)
		return;

	if (pStatus->dwStatus !=
		(D3DSTATUS_CLIPUNIONBACK | D3DSTATUS_CLIPUNIONLEFT | D3DSTATUS_CLIPUNIONRIGHT
			| D3DSTATUS_CLIPUNIONBOTTOM | D3DSTATUS_CLIPUNIONTOP))
		return;

	ZeroMemory(&sStatus9, sizeof(sStatus9));
	sStatus9.ClipUnion = D3DCS_BACK | D3DCS_LEFT | D3DCS_RIGHT | D3DCS_BOTTOM | D3DCS_TOP;
	m_pDev->SetClipStatus(&sStatus9);
}
