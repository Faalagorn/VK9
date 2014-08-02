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
 
#ifndef IDIRECT3DVERTEXSHADER9_H
#define IDIRECT3DVERTEXSHADER9_H

#include "i_unknown.h" // Base class: IUnknown

class IDirect3DDevice9;

class IDirect3DVertexShader9 : public IUnknown
{
public:
	IDirect3DVertexShader9();
	~IDirect3DVertexShader9();

	/*
	 * Gets the device.
	 */
	HRESULT GetDevice(IDirect3DDevice9 **ppDevice);
	
	/*
	 * Gets a pointer to the shader data.
	 */
	HRESULT GetFunction(void *pData,UINT *pSizeOfData);
	
};

typedef struct IDirect3DVertexShader9 *LPDIRECT3DVERTEXSHADER9, *PDIRECT3DVERTEXSHADER9;

#endif // IDIRECT3DVERTEXSHADER9_H
