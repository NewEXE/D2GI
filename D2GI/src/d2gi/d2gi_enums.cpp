
#define INITGUID
#include "../d3d7.h"

using namespace D3D7;


DDPIXELFORMAT g_pf8 = { sizeof(DDPIXELFORMAT), DDPF_PALETTEINDEXED8 | DDPF_RGB, 0, 8 };
DDPIXELFORMAT g_pf16 = { sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16 };

DDSURFACEDESC2 g_asAvailableDisplayModes[] =
{
	// 640x480x8
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		480, 640, 640, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf8
	},
	// 640x480x16
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		480, 640, 1280, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf16
	},
	// 800x600x8
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		600, 800, 800, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf8
	},
	// 800x600x16
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		600, 800, 1600, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf16
	},
	// 1024x768x8
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		768, 1024, 1024, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf8
	},
	// 1024x768x16
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		768, 1024, 2048, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf16
	},
	// 1600x1200x8
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		1200, 1600, 1600, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf8
	},
	// 1600x1200x16
	{
		sizeof(DDSURFACEDESC2), DDSD_WIDTH | DDSD_HEIGHT | DDSD_REFRESHRATE | DDSD_PITCH | DDSD_PIXELFORMAT,
		1200, 1600, 3200, 0, 0, 0, 0, NULL, {0, 0}, {0, 0}, {0, 0}, {0, 0}, g_pf16
	},
};

UINT g_uAvailableDisplayModesCount = ARRAYSIZE(g_asAvailableDisplayModes);


DDCAPS g_sHALCaps = 
{
	380, 2513600449, 2691346992, 189201, 581784800,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1778384896, 1774188544, 1, 0, 18, 0, 0, 0, 0, 0,
	{0, 0, 0, 0, 0, 0, 4096, 0}, {809923324},
	32, 2048000, 0, 0, 0, 0, 0, 0, 0, 64, 0, 0,
	{0, 0, 0, 0, 0, 0, 4096, 0}, 64, 0, 0,
	{0, 0, 0, 0, 0, 0, 4096, 0}, 0, 0, 0,
	{0, 0, 0, 0, 0, 0, 0, 0}, 0, 0, 0, 2513600449,
	2690822704, 189201, 581784800,
	{0, 0, 0, 0, 0, 0, 4096, 0}, 809923324, 512, 0, 0,
};


DDCAPS g_sHELCaps =
{
	380, 4106256961, 1, 512, 261347, 0, 839, 
	0, 0, 0, 0, 0, 0, 0, 1024, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	{1, 0, 0, 0, 0, 0, 4096, 2147483648}, {4330320},
	0, 0, 0, 0, 0, 0, 0, 0, 0, 4106256961, 512,
	261347, {1, 0, 0, 0, 0, 0, 4096, 2147483648},
	4106256961, 512, 261347,
	{1, 0, 0, 0, 0, 0, 4096, 2147483648}, 
	4106256961, 512, 261347,
	{1, 0, 0, 0, 0, 0, 4096, 2147483648},
	0, 0, 0, 4106256961, 1, 512, 261347,
	{1, 0, 0, 0, 0, 0, 4096, 2147483648},
	4330320, 512, 0, 0,
};


D3DDEVICEDESC7 g_asDeviceDescs[] =
{
	{
		1361,
		{
			56, 112, 2171383, 255, 8191, 2047, 255,
			807562, 223, 50529087, 207, 31, 4, 4,
		},
		{
			56, 112, 2171383, 255, 8191, 2047,
			255, 807562, 223, 50529087, 207, 31, 4, 4,
		},
		3840, 1024, 1, 1, 1024, 1024, 256, 0, 1,
		-4096.0, -4096.0, 4095.0, 4095.0, 0.0, 255, 8,
		30975, 8, 8, 4294967295, 10000.0,
		IID_IDirect3DRGBDevice, 6, 4, 63, 0, 0, 0, 0,
	},
	{
		235449937,
		{
			56, 2, 3301809, 255, 8191, 2047, 255,
			807562, 6237, 117638975, 207, 63, 0, 0,
		},
		{
			56, 114, 3301809, 255, 8191, 2047, 255, 
			807562, 6237, 117638975, 207, 63, 0, 0,
		},
		1280, 1536, 1, 1, 8192, 8192, 8192, 8192, 16,
		-16384.0, -16384.0, 16384.0, 16384.0, 0.0,
		255, 524296, 16777215, 4, 4, 4294967295,
		1.0e+10, IID_IDirect3DHALDevice, 6, 4, 63,
		0, 0, 0, 0, 
	},
	{
		235515473,
		{
			56, 2, 3301809, 255, 8191, 2047, 255,
			807562, 6237, 117638975, 207, 63, 0, 0
		},
		{
			56, 114, 3301809, 255, 8191, 2047, 255,
			807562, 6237, 117638975, 207, 63, 0, 0
		},
		1280, 1536, 1, 1, 8192, 8192, 8192, 8192, 16,
		-16384.0, -16384.0, 16384.0, 16384.0, 0.0,
		255, 524296, 16777215, 4, 4, 8, 1.0e+10,
		IID_IDirect3DTnLHalDevice, 6, 4, 379, 0, 0, 0, 0, 
	},
};


CHAR* g_lpszDeviceDescs[] = 
{
	"Microsoft Direct3D RGB Software Emulation",
	"Microsoft Direct3D Hardware acceleration through Direct3D HAL",
	"Microsoft Direct3D Hardware Transform and Lighting acceleration capable device"
};

CHAR* g_lpszDeviceNames[] = { 
	"RGB Emulation",
	"Direct3D HAL",
	"Direct3D T&L HAL" 
};

UINT g_uDeviceCount = ARRAYSIZE(g_asDeviceDescs);
