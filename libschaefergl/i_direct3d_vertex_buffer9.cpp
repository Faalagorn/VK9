/*
 * Copyright (C) 2014 Christopher Joseph Dean Schaefer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
 
#include "i_direct3d_vertex_buffer9.h"
#include <iostream> 

IDirect3DVertexBuffer9::IDirect3DVertexBuffer9()
{
}

IDirect3DVertexBuffer9::~IDirect3DVertexBuffer9()
{
}

HRESULT IDirect3DVertexBuffer9::GetDesc(D3DVERTEXBUFFER_DESC *pDesc)
{
	std::cout << "IDirect3DVertexBuffer9::etDesc(D3DVERTEXBUFFER_DESC *pDesc)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVertexBuffer9::Lock(UINT OffsetToLock,UINT SizeToLock,VOID **ppbData,DWORD Flags)
{
	std::cout << "IDirect3DVertexBuffer9::Lock(UINT OffsetToLock,UINT SizeToLock,VOID **ppbData,DWORD Flags)" << std::endl;	
	
	return E_NOTIMPL;
}

HRESULT IDirect3DVertexBuffer9::Unlock()
{
	std::cout << "IDirect3DVertexBuffer9::Unlock()" << std::endl;	
	
	return E_NOTIMPL;
}