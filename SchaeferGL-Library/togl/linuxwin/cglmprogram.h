//========= Copyright Valve Corporation, All rights reserved. ============//
//                       TOGL CODE LICENSE
//
//  Copyright 2011-2014 Valve Corporation
//  All Rights Reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//
// cglmprogram.h
//	GLMgr programs (ARBVP/ARBfp)
//
//===============================================================================

#ifndef CGLMPROGRAM_H
#define	CGLMPROGRAM_H

#include <sys/stat.h>

#pragma once

// good ARB program references
// http://petewarden.com/notes/archives/2005/05/fragment_progra_2.html
// http://petewarden.com/notes/archives/2005/06/fragment_progra_3.html

// ext links

// http://www.opengl.org/registry/specs/ARB/vertex_program.txt
// http://www.opengl.org/registry/specs/ARB/fragment_program.txt
// http://www.opengl.org/registry/specs/EXT/gpu_program_parameters.txt


//===============================================================================

// tokens not in the SDK headers

//#ifndef	GL_DEPTH_STENCIL_ATTACHMENT_EXT
//	#define GL_DEPTH_STENCIL_ATTACHMENT_EXT 0x84F9
//#endif

//===============================================================================

// forward declarations

class GLMContext;
class CGLMShaderPair;
class CGLMShaderPairCache;

// CGLMProgram can contain two flavors of the same program, one in assembler, one in GLSL.
// these flavors are pretty different in terms of the API's that are used to activate them - 
// for example, assembler programs can just get bound to the context, whereas GLSL programs
// have to be linked.  To some extent we try to hide that detail inside GLM.

// for now, make CGLMProgram a container, it does not set policy or hold a preference as to which
// flavor you want to use.  GLMContext has to handle that. 

enum EGLMProgramType
{
	kGLMVertexProgram,
	kGLMFragmentProgram,
	
	kGLMNumProgramTypes
};

enum EGLMProgramLang
{
	kGLMARB,
	kGLMGLSL,
	
	kGLMNumProgramLangs
};

struct GLMShaderDesc
{
	union
	{
		GLuint		arb;		// ARB program object name
		GLhandleARB	glsl;		// GLSL shader object handle (void*)
	}	m_object;

	// these can change if shader text is edited
	bool	m_textPresent;	// is this flavor(lang) of text present in the buffer?
	int		m_textOffset;	// where is it
	int		m_textLength;	// how big
	
	bool	m_compiled;		// has this text been through a compile attempt
	bool	m_valid;		// and if so, was the compile successful

	int		m_slowMark;		// has it been flagged during a non native draw batch before. increment every time it's slow.
	
	int		m_highWater;	// count of vec4's in the major uniform array ("vc" on vs, "pc" on ps)
							// written by dxabstract.... gross!
	int		m_VSHighWaterBone; // count of vec4's in the bone-specific uniform array (only valid for vertex shaders)
};

GLenum	GLMProgTypeToARBEnum( EGLMProgramType type );	// map vert/frag to ARB asm bind target
GLenum	GLMProgTypeToGLSLEnum( EGLMProgramType type );	// map vert/frag to ARB asm bind target

#define GL_SHADER_PAIR_CACHE_STATS 0

class CGLMProgram
{
public:
	friend class CGLMShaderPairCache;
	friend class CGLMShaderPair;
	friend class GLMContext;			// only GLMContext can make CGLMProgram objects
	friend class GLMTester;	
	friend struct IDirect3D9;
	friend struct IDirect3DDevice9;
		
	//===============================
	
	// constructor is very light, it just makes one empty program object per flavor.
	CGLMProgram( GLMContext *ctx, EGLMProgramType type );
	~CGLMProgram( );	

	void	SetProgramText			( char *text );				// import text to GLM object - invalidate any prev compiled program
	void	SetShaderName			( const char *name );				// only used for debugging/telemetry markup
	
	bool	CompileActiveSources	( void );					// compile only the flavors that were provided.
	bool	Compile					( EGLMProgramLang lang );	
	bool	CheckValidity			( EGLMProgramLang lang );

	void	LogSlow					( EGLMProgramLang lang );	// detailed spew when called for first time; one liner or perhaps silence after that
	
	void	GetLabelIndexCombo		( char *labelOut, int labelOutMaxChars, int *indexOut, int *comboOut );	
	void	GetComboIndexNameString	( char *stringOut, int stringOutMaxChars );		// mmmmmmmm-nnnnnnnn-filename
	
#if GLMDEBUG
	bool	PollForChanges( void );			// check mirror for changes.
	void	ReloadStringFromEditable( void );	// populate m_string from editable item (react to change)
	bool	SyncWithEditable( void );
#endif

	//===============================
	
	// common stuff

	GLMContext				*m_ctx;					// link back to parent context

	EGLMProgramType			m_type;					// vertex or pixel

	unsigned int					m_nHashTag;				// serial number for hashing
	
	char					*m_text;				// copy of text passed into constructor.  Can change if editable shaders is enabled.
													// note - it can contain multiple flavors, so use CGLMTextSectioner to scan it and locate them
#if GLMDEBUG
	CGLMEditableTextItem	*m_editable;			// editable text item for debugging
#endif	
	
	GLMShaderDesc			m_descs[ kGLMNumProgramLangs ];	

	unsigned int					m_samplerMask;			// (1<<n) mask of sampler active locs, if this is a fragment shader (dxabstract sets this field)
	unsigned int					m_samplerTypes;			// SAMPLER_2D, etc.
	unsigned int					m_fragDataMask;			// (1<<n) mask of gl_FragData[n] outputs referenced, if this is a fragment shader (dxabstract sets this field)
	unsigned int					m_numDrawBuffers;		// number of draw buffers used
	GLenum					m_drawBuffers[4];		// GL_COLOR_ATTACHMENT0_EXT1, etc
	unsigned int					m_nNumUsedSamplers;
	unsigned int					m_maxSamplers;
	unsigned int					m_maxVertexAttrs;
	unsigned int					m_nCentroidMask;
	unsigned int					m_nShadowDepthSamplerMask;
	
	bool					m_bTranslatedProgram;

	char					m_shaderName[64];
};	

//===============================================================================

struct GLMShaderPairInfo
{
	int		m_status;		// -1 means req'd index was out of bounds (loop stop..)  0 means not present.  1 means present/active.
	
	char	m_vsName[ 128 ];
	int		m_vsStaticIndex;
	int		m_vsDynamicIndex;
	
	char	m_psName[ 128 ];
	int		m_psStaticIndex;
	int		m_psDynamicIndex;
};

class CGLMShaderPair					// a container for a linked GLSL shader pair, and metadata obtained post-link
{

public:

	friend class CGLMProgram;
	friend class GLMContext;
	friend class CGLMShaderPairCache;
		
	//===============================
	
	// constructor just sets up a GLSL program object and leaves it empty.
	CGLMShaderPair( GLMContext *ctx  );
	~CGLMShaderPair( );	

	bool	SetProgramPair			( CGLMProgram *vp, CGLMProgram *fp );
		// true result means successful link and query

	bool	RefreshProgramPair		( void );
		// re-link and re-query the uniforms

	FORCEINLINE void UpdateScreenUniform( unsigned int nWidthHeight )
	{
		if ( m_nScreenWidthHeight == nWidthHeight )
			return;
		
		m_nScreenWidthHeight = nWidthHeight;

		float fWidth = (float)( nWidthHeight & 0xFFFF ), fHeight = (float)( nWidthHeight >> 16 );
		// Apply half pixel offset to output vertices to account for the pixel center difference between D3D9 and OpenGL.
		// We output vertices in clip space, which ranges from [-1,1], so 1.0/width in clip space transforms into .5/width in screenspace, see: "Viewports and Clipping (Direct3D 9)" in the DXSDK
		float v[4] = { 1.0f / fWidth, 1.0f / fHeight, fWidth, fHeight };
		if ( m_locVertexScreenParams >= 0 )
			gGL->glUniform4fv( m_locVertexScreenParams, 1, v );
	}
	
	//===============================
	
	// common stuff

	GLMContext				*m_ctx;					// link back to parent context

	CGLMProgram				*m_vertexProg;	
	CGLMProgram				*m_fragmentProg;

	GLhandleARB				m_program;				// linked program object

	// need meta data for attribs / samplers / params
	// actually we only need it for samplers and params.
	// attributes are hardwired.
	
	// vertex stage uniforms
	GLint					m_locVertexParams;		// "vc" per dx9asmtogl2 convention
	GLint					m_locVertexBoneParams;	// "vcbones"
	GLint					m_locVertexInteger0;	// "i0"
			
	enum { cMaxVertexShaderBoolUniforms = 4, cMaxFragmentShaderBoolUniforms = 1 };

	GLint					m_locVertexBool[cMaxVertexShaderBoolUniforms];		// "b0", etc.
	GLint					m_locFragmentBool[cMaxFragmentShaderBoolUniforms];		// "fb0", etc.
	bool					m_bHasBoolOrIntUniforms;
			
	// fragment stage uniforms
	GLint					m_locFragmentParams;			// "pc" per dx9asmtogl2 convention
	
	int						m_NumUniformBufferParams[kGLMNumProgramTypes];
	GLint					m_UniformBufferParams[kGLMNumProgramTypes][256];
	
	GLint					m_locFragmentFakeSRGBEnable;	// "flSRGBWrite" - set to 1.0 to effect sRGB encoding on output
	float					m_fakeSRGBEnableValue;			// shadow to avoid redundant sets of the m_locFragmentFakeSRGBEnable uniform
		// init it to -1.0 at link or relink, so it will trip on any legit incoming value (0.0 or 1.0)

	GLint					m_locSamplers[ GLM_SAMPLER_COUNT ];			// "sampler0 ... sampler1..."

	// other stuff
	bool					m_valid;				// true on successful link
	unsigned int					m_revision;				// if this pair is relinked, bump this number.

	GLint					m_locVertexScreenParams; // vcscreen
	unsigned int					m_nScreenWidthHeight;
		
};	

//===============================================================================

// N-row, M-way associative cache with LRU per row.
// still needs some metric dump ability and some parameter tuning.
// extra credit would be to make an auto-tuner.

struct CGLMPairCacheEntry
{
	long long		m_lastMark;				// a mark of zero means an empty entry
	CGLMProgram		*m_vertexProg;
	CGLMProgram		*m_fragmentProg;
	unsigned int			m_extraKeyBits;
	CGLMShaderPair	*m_pair;
};

class CGLMShaderPairCache				// cache for linked GLSL shader pairs
{

public:

protected:
	friend class CGLMShaderPair;
	friend class CGLMProgram;
	friend class GLMContext;
		
	//===============================
	
	CGLMShaderPairCache( GLMContext *ctx  );
	~CGLMShaderPairCache( );	

	FORCEINLINE CGLMShaderPair *SelectShaderPair	( CGLMProgram *vp, CGLMProgram *fp, unsigned int extraKeyBits );
	void			QueryShaderPair		( int index, GLMShaderPairInfo *infoOut );
	
	// shoot down linked pairs that use the program in the arg
	// return true if any had to be skipped due to conflict with currently bound pair
	bool			PurgePairsWithShader( CGLMProgram *prog );
	
	// purge everything (when would GLM know how to do this ?  at context destroy time, but any other times?)
	// return true if any had to be skipped due to conflict with currently bound pair
	bool			Purge				( void );
	
	// stats
	void			DumpStats			( void );
	
	//===============================

	FORCEINLINE unsigned int HashRowIndex( CGLMProgram *vp, CGLMProgram *fp, unsigned int extraKeyBits ) const;
	FORCEINLINE CGLMPairCacheEntry*	HashRowPtr( unsigned int hashRowIndex ) const;
	
	FORCEINLINE void HashRowProbe( CGLMPairCacheEntry *row, CGLMProgram *vp, CGLMProgram *fp, unsigned int extraKeyBits, int &hitway, int &emptyway, int &oldestway );
		
	CGLMShaderPair *SelectShaderPairInternal( CGLMProgram *vp, CGLMProgram *fp, unsigned int extraKeyBits, int rowIndex );
	//===============================

	// common stuff

	GLMContext				*m_ctx;					// link back to parent context

	long long				m_mark;

	unsigned int					m_rowsLg2;
	unsigned int					m_rows;
	unsigned int					m_rowsMask;
	
	unsigned int					m_waysLg2;
	unsigned int					m_ways;
	
	unsigned int					m_entryCount;
	
	CGLMPairCacheEntry		*m_entries;				// array[ m_rows ][ m_ways ]

	unsigned int					*m_evictions;			// array[ m_rows ];

#if GL_SHADER_PAIR_CACHE_STATS
	unsigned int					*m_hits;				// array[ m_rows ];
#endif
};	

FORCEINLINE unsigned int CGLMShaderPairCache::HashRowIndex( CGLMProgram *vp, CGLMProgram *fp, unsigned int extraKeyBits ) const
{
	return ( vp->m_nHashTag + fp->m_nHashTag + extraKeyBits * 7 ) & m_rowsMask;
}

FORCEINLINE CGLMPairCacheEntry*	CGLMShaderPairCache::HashRowPtr( unsigned int hashRowIndex ) const
{
	return &m_entries[ hashRowIndex * m_ways ];
}

FORCEINLINE void CGLMShaderPairCache::HashRowProbe( CGLMPairCacheEntry *row, CGLMProgram *vp, CGLMProgram *fp, unsigned int extraKeyBits, int& hitway, int& emptyway, int& oldestway )
{
	hitway = -1;
	emptyway = -1;
	oldestway = -1;

	// scan this row to see if the desired pair is present
	CGLMPairCacheEntry *cursor = row;
	long long oldestmark = 0xFFFFFFFFFFFFFFFFLL;

	for( unsigned int way = 0; way < m_ways; ++way )
	{
		if ( cursor->m_lastMark != 0 )	// occupied slot
		{
			// check if this is the oldest one on the row - only occupied slots are checked
			if ( cursor->m_lastMark < oldestmark )
			{
				oldestway = way;
				oldestmark = cursor->m_lastMark;
			}

			if ( ( cursor->m_vertexProg == vp ) && ( cursor->m_fragmentProg == fp ) && ( cursor->m_extraKeyBits == extraKeyBits ) )	// match?
			{
				// found it
				hitway = way;
				break;
			}
		}
		else
		{
			// empty way, log it if first one seen
			if (emptyway<0)
			{
				emptyway = way;
			}
		}
		cursor++;
	}
}

FORCEINLINE CGLMShaderPair *CGLMShaderPairCache::SelectShaderPair( CGLMProgram *vp, CGLMProgram *fp, unsigned int extraKeyBits )
{
	// select row where pair would be found if it exists
	unsigned int rowIndex = HashRowIndex( vp, fp, extraKeyBits );

	CGLMPairCacheEntry *pCursor = HashRowPtr( rowIndex );
	
	if ( ( pCursor->m_fragmentProg != fp ) || ( pCursor->m_vertexProg != vp ) || ( pCursor->m_extraKeyBits != extraKeyBits ) )
	{
		CGLMPairCacheEntry *pLastCursor = pCursor + m_ways;

		++pCursor;

		while ( pCursor != pLastCursor )
		{
			if ( ( pCursor->m_fragmentProg == fp ) && ( pCursor->m_vertexProg == vp ) && ( pCursor->m_extraKeyBits == extraKeyBits ) )	// match?
				break;
			++pCursor;
		};
	
		if ( pCursor == pLastCursor )
			return SelectShaderPairInternal( vp, fp, extraKeyBits, rowIndex );
	}
		
	// found it.  mark it and return
	pCursor->m_lastMark = m_mark++;

#if GL_SHADER_PAIR_CACHE_STATS
	// count the hit
	m_hits[ rowIndex ] ++;
#endif

	return pCursor->m_pair;
}

#endif
