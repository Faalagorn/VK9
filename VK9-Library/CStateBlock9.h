/*
Copyright(c) 2016 Christopher Joseph Dean Schaefer

This software is provided 'as-is', without any express or implied
warranty.In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions :

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software.If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef CSTATEBLOCK9_H
#define CSTATEBLOCK9_H

#include "d3d9.h"
#include <vulkan/vulkan.h>
#include "Perf_CommandStreamManager.h"
#include "CTypes.h"

class CDevice9;

class CStateBlock9 : public IDirect3DStateBlock9

{
private:
	CDevice9* mDevice = nullptr;	
public:
	CStateBlock9(CDevice9* device, D3DSTATEBLOCKTYPE Type);
	CStateBlock9(CDevice9* device);
	~CStateBlock9();

	size_t mId;
	std::shared_ptr<CommandStreamManager> mCommandStreamManager;
	ULONG mReferenceCount = 1;

	//Device State
	D3DSTATEBLOCKTYPE mType = D3DSBT_ALL;
	DeviceState mDeviceState = {};

public:

	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppv);
	virtual ULONG STDMETHODCALLTYPE AddRef(void);
	virtual ULONG STDMETHODCALLTYPE Release(void);

	//IDirect3DStateBlock9
	virtual HRESULT STDMETHODCALLTYPE Capture();
	virtual HRESULT STDMETHODCALLTYPE Apply();
	virtual HRESULT STDMETHODCALLTYPE GetDevice(IDirect3DDevice9** ppDevice);

};

#endif // CSTATEBLOCK9_H
