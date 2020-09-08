/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//
// rb_state.c
// FIXME TODO:
// - Statebit pushing, which will require that all state changes are local to the backend
//

#include "rb_local.h"

rb_glState_t	rb_glState;

/*
==============================================================================

	STATEBIT MANAGEMENT

==============================================================================
*/

/*
===============
RB_StateForBits
===============
*/
void RB_StateForBits (uint32 bits1)
{
	uint32	diff;

	// Process bit group one
	diff = bits1 ^ rb_glState.stateBits1;
	if (diff) {
		// Alpha testing
		if (diff & SB1_ATEST_BITS) {
			switch (bits1 & SB1_ATEST_BITS) {
			case 0:
				glDisable (GL_ALPHA_TEST);
				break;
			case SB1_ATEST_GT0:
				glEnable (GL_ALPHA_TEST);
				glAlphaFunc (GL_GREATER, 0);
				break;
			case SB1_ATEST_LT128:
				glEnable (GL_ALPHA_TEST);
				glAlphaFunc (GL_LESS, 0.5f);
				break;
			case SB1_ATEST_GE128:
				glEnable (GL_ALPHA_TEST);
				glAlphaFunc (GL_GEQUAL, 0.5f);
				break;
			default:
				assert (0);
				break;
			}

			ri.pc.stateChanges++;
		}

		// Blending
		if (diff & SB1_BLEND_ON) {
			if (bits1 & SB1_BLEND_ON)
				glEnable (GL_BLEND);
			else
				glDisable (GL_BLEND);
			ri.pc.stateChanges++;
		}

		if (diff & (SB1_BLENDSRC_BITS|SB1_BLENDDST_BITS)
		&& bits1 & (SB1_BLENDSRC_BITS|SB1_BLENDDST_BITS)) {
			GLenum	sFactor = GL_ONE, dFactor = GL_ONE;

			switch (bits1 & SB1_BLENDSRC_BITS) {
			case SB1_BLENDSRC_ZERO:					sFactor = GL_ZERO;					break;
			case SB1_BLENDSRC_ONE:					sFactor = GL_ONE;					break;
			case SB1_BLENDSRC_DST_COLOR:			sFactor = GL_DST_COLOR;				break;
			case SB1_BLENDSRC_ONE_MINUS_DST_COLOR:	sFactor = GL_ONE_MINUS_DST_COLOR;	break;
			case SB1_BLENDSRC_SRC_ALPHA:			sFactor = GL_SRC_ALPHA;				break;
			case SB1_BLENDSRC_ONE_MINUS_SRC_ALPHA:	sFactor = GL_ONE_MINUS_SRC_ALPHA;	break;
			case SB1_BLENDSRC_DST_ALPHA:			sFactor = GL_DST_ALPHA;				break;
			case SB1_BLENDSRC_ONE_MINUS_DST_ALPHA:	sFactor = GL_ONE_MINUS_DST_ALPHA;	break;
			case SB1_BLENDSRC_SRC_ALPHA_SATURATE:	sFactor = GL_SRC_ALPHA_SATURATE;	break;
			default:								assert (0);							break;
			}

			switch (bits1 & SB1_BLENDDST_BITS) {
			case SB1_BLENDDST_ZERO:					dFactor = GL_ZERO;					break;
			case SB1_BLENDDST_ONE:					dFactor = GL_ONE;					break;
			case SB1_BLENDDST_SRC_COLOR:			dFactor = GL_SRC_COLOR;				break;
			case SB1_BLENDDST_ONE_MINUS_SRC_COLOR:	dFactor = GL_ONE_MINUS_SRC_COLOR;	break;
			case SB1_BLENDDST_SRC_ALPHA:			dFactor = GL_SRC_ALPHA;				break;
			case SB1_BLENDDST_ONE_MINUS_SRC_ALPHA:	dFactor = GL_ONE_MINUS_SRC_ALPHA;	break;
			case SB1_BLENDDST_DST_ALPHA:			dFactor = GL_DST_ALPHA;				break;
			case SB1_BLENDDST_ONE_MINUS_DST_ALPHA:	dFactor = GL_ONE_MINUS_DST_ALPHA;	break;
			default:								assert (0);							break;
			}

			glBlendFunc (sFactor, dFactor);
			ri.pc.stateChanges++;
		}

		// Culling
		if (diff & SB1_CULL_BITS) {
			switch (bits1 & SB1_CULL_BITS) {
			case 0:
				glDisable (GL_CULL_FACE);
				break;
			case SB1_CULL_FRONT:
				glCullFace (GL_FRONT);
				glEnable (GL_CULL_FACE);
				break;
			case SB1_CULL_BACK:
				glCullFace (GL_BACK);
				glEnable (GL_CULL_FACE);
				break;
			default:
				assert (0);
				break;
			}
			ri.pc.stateChanges++;
		}

		// Depth masking
		if (diff & SB1_DEPTHMASK_ON) {
			if (bits1 & SB1_DEPTHMASK_ON)
				glDepthMask (GL_TRUE);
			else
				glDepthMask (GL_FALSE);
			ri.pc.stateChanges++;
		}

		// Depth testing
		if (diff & SB1_DEPTHTEST_ON) {
			if (bits1 & SB1_DEPTHTEST_ON)
				glEnable (GL_DEPTH_TEST);
			else
				glDisable (GL_DEPTH_TEST);
			ri.pc.stateChanges++;
		}

		// Polygon offset
		if (diff & SB1_POLYOFFSET_ON) {
			if (bits1 & SB1_POLYOFFSET_ON) {
				glEnable (GL_POLYGON_OFFSET_FILL);
				glPolygonOffset (r_offsetFactor->intVal, r_offsetUnits->intVal);
			}
			else
				glDisable (GL_POLYGON_OFFSET_FILL);
			ri.pc.stateChanges++;
		}

		// Save for the next diff
		rb_glState.stateBits1 = bits1;
	}
}

/*
==============================================================================

	TEXTURE STATE

==============================================================================
*/

/*
===============
RB_BindTexture
===============
*/
void RB_BindTexture (image_t *image)
{
	// Performance evaluation option
	if (gl_nobind->intVal || !image)
		image = ri.noTexture;

	// Determine if it's already bound
	if (rb_glState.texBound[rb_glState.texUnit] == image)
		return;
	rb_glState.texBound[rb_glState.texUnit] = image;

	// Nope, bind it
	glBindTexture (image->target, image->texNum);

	// Performance counters
	if (r_speeds->intVal) {
		ri.pc.textureBinds++;
		if (image->visFrame != ri.frameCount) {
			image->visFrame = ri.frameCount;
			ri.pc.texturesInUse++;
			ri.pc.texelsInUse += image->upWidth * image->upHeight;
		}
	}
}


/*
===============
RB_SelectTexture
===============
*/
void RB_SelectTexture (texUnit_t texUnit)
{
	if (texUnit == rb_glState.texUnit)
		return;
	if (texUnit > ri.config.maxTexUnits) {
		Com_Error (ERR_DROP, "Attempted selection of an out of bounds (%d) texture unit!", texUnit);
		return;
	}

	// Select the unit
	rb_glState.texUnit = texUnit;
	if (ri.config.extArbMultitexture) {
		glActiveTextureARB (texUnit + GL_TEXTURE0_ARB);
		glClientActiveTextureARB (texUnit + GL_TEXTURE0_ARB);
	}
	else if (ri.config.extSGISMultiTexture) {
		glSelectTextureSGIS (texUnit + GL_TEXTURE0_SGIS);
	}
	else {
		return;
	}

	// Performance counter
	ri.pc.textureUnitChanges++;
}


/*
===============
RB_TextureEnv
===============
*/
void RB_TextureEnv (GLfloat mode)
{
	if (mode == GL_ADD && !ri.config.extTexEnvAdd)
		mode = GL_MODULATE;

	if (mode != rb_glState.texEnvModes[rb_glState.texUnit]) {
		glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode);
		rb_glState.texEnvModes[rb_glState.texUnit] = mode;

		// Performance counter
		ri.pc.textureEnvChanges++;
	}
}


/*
===============
RB_TextureTarget

Supplements glEnable/glDisable on GL_TEXTURE_1D/2D/3D/CUBE_MAP_ARB.
===============
*/
void RB_TextureTarget (GLenum target)
{
	if (target == rb_glState.texTarget[rb_glState.texUnit])
		return;

	if (rb_glState.texTarget[rb_glState.texUnit])
		glDisable (rb_glState.texTarget[rb_glState.texUnit]);

	rb_glState.texTarget[rb_glState.texUnit] = target;

	if (target)
		glEnable (target);
}


/*
===============
RB_LoadTexMatrix
===============
*/
void RB_LoadTexMatrix (mat4x4_t m)
{
	glLoadMatrixf (m);
	rb_glState.texMatIdentity[rb_glState.texUnit] = false;
}


/*
===============
RB_LoadIdentityTexMatrix
===============
*/
void RB_LoadIdentityTexMatrix (void)
{
	if (!rb_glState.texMatIdentity[rb_glState.texUnit]) {
		glLoadIdentity ();
		rb_glState.texMatIdentity[rb_glState.texUnit] = true;
	}
}

/*
==============================================================================

	PROGRAM STATE

==============================================================================
*/

/*
===============
RB_BindProgram
===============
*/
void RB_BindProgram (program_t *program)
{
	switch (program->target) {
	case GL_FRAGMENT_PROGRAM_ARB:
		if (rb_glState.boundFragProgram == program->progNum)
			return;
		rb_glState.boundFragProgram = program->progNum;

		glBindProgramARB (GL_FRAGMENT_PROGRAM_ARB, program->progNum);
		break;

	case GL_VERTEX_PROGRAM_ARB:
		if (rb_glState.boundVertProgram == program->progNum)
			return;
		rb_glState.boundVertProgram = program->progNum;

		glBindProgramARB (GL_VERTEX_PROGRAM_ARB, program->progNum);
		break;

#ifdef _DEBUG
	default:
		assert (0);
		break;
#endif // _DEBUG
	}
}

/*
==============================================================================

	GENERIC STATE MANAGEMENT

==============================================================================
*/

/*
==================
RB_SetupGL2D
==================
*/
void RB_SetupGL2D (void)
{
	// State
	rb_glState.in2D = true;
	rb_glState.stateBits1 &= ~SB1_DEPTHMASK_ON;
	glDepthMask (GL_FALSE);

	// Set 2D virtual screen size
	glViewport (0, 0, ri.config.vidWidth, ri.config.vidHeight);
	glScissor (0, 0, ri.config.vidWidth, ri.config.vidHeight);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (0, ri.config.vidWidth, ri.config.vidHeight, 0, -99999, 99999);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}


/*
=============
RB_SetupGL3D
=============
*/
void RB_SetupGL3D (void)
{
	// State
	rb_glState.in2D = false;
	rb_glState.stateBits1 |= SB1_DEPTHMASK_ON;
	glDepthMask (GL_TRUE);

	// Set up viewport
	if (!ri.scn.mirrorView && !ri.scn.portalView) {
		glScissor (ri.def.x, ri.config.vidHeight - ri.def.height - ri.def.y, ri.def.width, ri.def.height);
		glViewport (ri.def.x, ri.config.vidHeight - ri.def.height - ri.def.y, ri.def.width, ri.def.height);
		glClear (GL_DEPTH_BUFFER_BIT);
	}

	// Set up projection matrix
	R_SetupProjectionMatrix (&ri.def, ri.scn.projectionMatrix);
	if (ri.scn.mirrorView)
		ri.scn.projectionMatrix[0] = -ri.scn.projectionMatrix[0];

	glMatrixMode (GL_PROJECTION);
	glLoadMatrixf (ri.scn.projectionMatrix);

	// Set up the world view matrix
	R_SetupModelviewMatrix (&ri.def, ri.scn.worldViewMatrix);

	glMatrixMode (GL_MODELVIEW);
	glLoadMatrixf (ri.scn.worldViewMatrix);

	// Handle portal/mirror rendering
	if (ri.scn.mirrorView || ri.scn.portalView) {
		GLdouble	clip[4];

		clip[0] = ri.scn.clipPlane.normal[0];
		clip[1] = ri.scn.clipPlane.normal[1];
		clip[2] = ri.scn.clipPlane.normal[2];
		clip[3] = -ri.scn.clipPlane.dist;

		glClipPlane (GL_CLIP_PLANE0, clip);
		glEnable (GL_CLIP_PLANE0);
	}
}


/*
================
RB_ClearBuffers
================
*/
void RB_ClearBuffers (void)
{
	int			clearBits;

	clearBits = GL_DEPTH_BUFFER_BIT;
	if (gl_clear->intVal) {
		glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
		clearBits |= GL_COLOR_BUFFER_BIT;
	}

	if (ri.useStencil && gl_shadows->intVal) {
		glClearStencil (128);
		clearBits |= GL_STENCIL_BUFFER_BIT;
	}

	glClear (clearBits);

	glDepthRange (0, 1);
}


/*
==================
RB_SetDefaultState

Sets our default OpenGL state
==================
*/
void RB_SetDefaultState (void)
{
	texUnit_t	i;

	rb_glState.stateBits1 = 0;

	glFinish ();

	glColor4f (1, 1, 1, 1);
	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);

	glEnable (GL_SCISSOR_TEST);

	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset (0, 0);

	// Texture-unit specific
	for (i=MAX_TEXUNITS-1 ; i>=0 ; i--) {
		rb_glState.texBound[i] = NULL;
		rb_glState.texEnvModes[i] = 0;
		rb_glState.texMatIdentity[i] = true;
		if (i >= ri.config.maxTexUnits)
			continue;

		// Texture
		RB_SelectTexture (i);

		glDisable (GL_TEXTURE_1D);
		glBindTexture (GL_TEXTURE_1D, 0);

		if (ri.config.extTex3D) {
			glDisable (GL_TEXTURE_3D);
			glBindTexture (GL_TEXTURE_3D, 0);
		}
		if (ri.config.extTexCubeMap) {
			glDisable (GL_TEXTURE_CUBE_MAP_ARB);
			glBindTexture (GL_TEXTURE_CUBE_MAP_ARB, 0);
		}

		if (i == 0) {
			glEnable (GL_TEXTURE_2D);
			rb_glState.texTarget[i] = GL_TEXTURE_2D;
		}
		else {
			glDisable (GL_TEXTURE_2D);
			rb_glState.texTarget[i] = 0;
		}
		glBindTexture (GL_TEXTURE_2D, 0);

		// Texgen
		glDisable (GL_TEXTURE_GEN_S);
		glDisable (GL_TEXTURE_GEN_T);
		glDisable (GL_TEXTURE_GEN_R);
		glDisable (GL_TEXTURE_GEN_Q);
	}

	// Fragment programs
	if (ri.config.extFragmentProgram)
		glDisable (GL_FRAGMENT_PROGRAM_ARB);

	// Vertex programs
	if (ri.config.extVertexProgram)
		glDisable (GL_VERTEX_PROGRAM_ARB);

	// Stencil testing
	if (ri.useStencil)
		glDisable (GL_STENCIL_TEST);

	// Polygon offset testing
	glDisable (GL_POLYGON_OFFSET_FILL);

	// Depth testing
	glDisable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);
	glDepthRange (0, 1);

	// Face culling
	glDisable (GL_CULL_FACE);
	glCullFace (GL_FRONT);
	rb_glState.stateBits1 |= SB1_CULL_FRONT;

	// Alpha testing
	glDisable (GL_ALPHA_TEST);
	glAlphaFunc (GL_GREATER, 0);

	// Blending
	glDisable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	rb_glState.stateBits1 |= SB1_BLENDSRC_SRC_ALPHA|SB1_BLENDDST_ONE_MINUS_SRC_ALPHA;

	// Model shading
	glShadeModel (GL_SMOOTH);

	// Check for errors
	GL_CheckForError ("RB_SetDefaultState");
}
