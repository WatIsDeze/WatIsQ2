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
// r_local.h
// Refresh only header file
//

#ifdef DEDICATED_ONLY
# error You should not be including this file in a dedicated server build
#endif // DEDICATED_ONLY

#define SHADOW_VOLUMES	2
#define SHADOW_ALPHA	0.5f

#ifdef _WIN32
# include <windows.h>
#endif

#include <GL/GLEW.h>
#include <GL/gl.h>
#include <math.h>

#include "r_public.h"
#include "r_typedefs.h"
#include "rb_gl.h"
#include "rf_image.h"
#include "rf_program.h"
#include "rf_material.h"
#include "rf_public.h"
#include "rb_public.h"
#include "rf_model.h"

/*
=============================================================================

	REFRESH INFO

=============================================================================
*/

enum { // rendererClass_t
	REND_CLASS_DEFAULT,
	REND_CLASS_MCD,

	REND_CLASS_3DLABS_GLINT_MX,
	REND_CLASS_3DLABS_PERMEDIA,
	REND_CLASS_3DLABS_REALIZM,
	REND_CLASS_ATI,
	REND_CLASS_ATI_RADEON,
	REND_CLASS_INTEL,
	REND_CLASS_NVIDIA,
	REND_CLASS_NVIDIA_GEFORCE,
	REND_CLASS_PMX,
	REND_CLASS_POWERVR_PCX1,
	REND_CLASS_POWERVR_PCX2,
	REND_CLASS_RENDITION,
	REND_CLASS_S3,
	REND_CLASS_SGI,
	REND_CLASS_SIS,
	REND_CLASS_VOODOO
};

typedef struct refMedia_s {
	struct font_s		*defaultFont;

	// This is hacky but necessary, fuck you Quake2.
	material_t			*worldLavaCaustics;
	material_t			*worldSlimeCaustics;
	material_t			*worldWaterCaustics;
} refMedia_t;

// FIXME: some of this can be moved to ri...
typedef struct refScene_s {
	// View
	cBspPlane_t			viewFrustum[5];

	mat4x4_t			modelViewMatrix;
	mat4x4_t			projectionMatrix;
	mat4x4_t			worldViewMatrix;

	qBool				mirrorView;
	qBool				portalView;
	vec3_t				portalOrigin;
	cBspPlane_t			clipPlane;

	float				zFar;

	refEntity_t			*defaultEntity;
	refModel_t			*defaultModel;

	// World model
	refModel_t			*worldModel;
	refEntity_t			*worldEntity;

	uint32				visFrameCount;	// Bumped when going to a new PVS
	int					viewCluster;
	int					oldViewCluster;

	// Items
	uint32				numDecals;
	uint32				drawnDecals;
	refDecal_t			*decalList[MAX_REF_DECALS];

	uint32				numEntities;
	refEntity_t			entityList[MAX_REF_ENTITIES];

	uint32				numPolys;
	refPoly_t			*polyList[MAX_REF_POLYS];

	uint32				numDLights;
	refDLight_t			dLightList[MAX_REF_DLIGHTS];

	refLightStyle_t		lightStyles[MAX_CS_LIGHTSTYLES];
} refScene_t;

enum {
	CULL_FAIL,
	CULL_PASS
};

typedef struct refStats_s {
	// Totals
	uint32				numTris;
	uint32				numVerts;
	uint32				numElements;

	uint32				meshCount;
	uint32				meshPasses;

	uint32				stateChanges;

	// Alias Models
	uint32				aliasElements;
	uint32				aliasPolys;

	// Batching
	uint32				meshBatches;
	uint32				meshBatchFlush;

	// Culling
	uint32				cullBounds[2];	// [CULL_FAIL|CULL_PASS]
	uint32				cullPlanar[2];	// [CULL_FAIL|CULL_PASS]
	uint32				cullRadius[2];	// [CULL_FAIL|CULL_PASS]
	uint32				cullSurf[2];	// [CULL_FAIL|CULL_PASS]
	uint32				cullVis[2];		// [CULL_FAIL|CULL_PASS]

	// Image
	uint32				textureBinds;
	uint32				textureEnvChanges;
	uint32				textureUnitChanges;

	uint32				texelsInUse;
	uint32				texturesInUse;

	// World model
	uint32				worldElements;
	uint32				worldPolys;

	// Time to process
	uint32				timeAddToList;
	uint32				timeSortList;
	uint32				timeDrawList;

	uint32				timeMarkLeaves;
	uint32				timeMarkLights;
	uint32				timeRecurseWorld;
} refStats_t;

typedef struct refRegist_s {
	qBool				inSequence;			// True when in a registration sequence
	uint32				registerFrame;		// Used to determine what's kept and what's not

	// Fonts
	uint32				fontsReleased;
	uint32				fontsSeaked;
	uint32				fontsTouched;

	// Images
	uint32				imagesReleased;
	uint32				imagesResampled;
	uint32				imagesSeaked;
	uint32				imagesTouched;

	// Models
	uint32				modelsReleased;
	uint32				modelsSeaked;
	uint32				modelsTouched;

	// Materials
	uint32				matsReleased;
	uint32				matsSeaked;
	uint32				matsTouched;
} refRegist_t;

typedef struct refInfo_s {
	// Refresh information
	rendererClass_t		renderClass;

	const byte			*rendererString;
	const byte			*vendorString;
	const byte			*versionString;
	const byte			*extensionString;

	int					lastValidMode;

	// Frame information
	float				cameraSeparation;
	uint32				frameCount;

	// Hardware gamma
	qBool				rampDownloaded;
	uint16				originalRamp[768];
	uint16				gammaRamp[768];

	// PFD Stuff
	qBool				useStencil;

	byte				cAlphaBits;
	byte				cColorBits;
	byte				cDepthBits;
	byte				cStencilBits;

	// Texture
	float				inverseIntensity;

	GLint				texMinFilter;
	GLint				texMagFilter;

	int					rgbFormat;
	int					rgbaFormat;
	int					greyFormat;

	int					rgbFormatCompressed;
	int					rgbaFormatCompressed;
	int					greyFormatCompressed;

	float				pow2MapOvrbr;

	// Internal textures
	image_t				*noTexture;			// use for bad textures
	image_t				*whiteTexture;		// used in materials/fallback
	image_t				*blackTexture;		// used in materials/fallback
	image_t				*cinTexture;		// allocates memory on load as to not every cin frame
	image_t				*dLightTexture;		// dynamic light texture for q3 bsp
	image_t				*fogTexture;		// fog texture for q3 bsp

	// Memory management
	struct memPool_s	*decalSysPool;
	struct memPool_s	*fontSysPool;
	struct memPool_s	*genericPool;
	struct memPool_s	*imageSysPool;
	struct memPool_s	*lightSysPool;
	struct memPool_s	*modelSysPool;
	struct memPool_s	*programSysPool;
	struct memPool_s	*matSysPool;

	// Misc
	refConfig_t			config;			// Information output to the client/cgame
	refDef_t			def;			// Current refDef scene
	refMedia_t			media;			// Local media
	refRegist_t			reg;			// Registration counters
	refScene_t			scn;			// Local scene information
	refStats_t			pc;				// Performance counters
} refInfo_t;

extern refInfo_t	ri;

/*
=============================================================================

	CVARS

=============================================================================
*/

extern cVar_t	*e_test_0;
extern cVar_t	*e_test_1;

extern cVar_t	*intensity;

extern cVar_t	*gl_bitdepth;
extern cVar_t	*gl_clear;
extern cVar_t	*gl_cull;
extern cVar_t	*gl_drawbuffer;
extern cVar_t	*gl_dynamic;
extern cVar_t	*gl_errorcheck;

extern cVar_t	*r_ext_maxAnisotropy;

extern cVar_t	*gl_finish;
extern cVar_t	*gl_flashblend;
extern cVar_t	*gl_lightmap;
extern cVar_t	*gl_lockpvs;
extern cVar_t	*gl_log;
extern cVar_t	*gl_maxTexSize;
extern cVar_t	*gl_mode;
extern cVar_t	*gl_modulate;

extern cVar_t	*gl_screenshot;
extern cVar_t	*gl_shadows;
extern cVar_t	*gl_shownormals;
extern cVar_t	*gl_showtris;
extern cVar_t	*gl_stencilbuffer;
extern cVar_t	*gl_texturemode;

extern cVar_t	*gl_debug;

extern cVar_t	*r_caustics;
extern cVar_t	*r_colorMipLevels;
extern cVar_t	*r_debugBatching;
extern cVar_t	*r_debugCulling;
extern cVar_t	*r_debugLighting;
extern cVar_t	*r_debugSorting;
extern cVar_t	*r_defaultFont;
extern cVar_t	*r_detailTextures;
extern cVar_t	*r_displayFreq;
extern cVar_t	*r_drawDecals;
extern cVar_t	*r_drawEntities;
extern cVar_t	*r_drawPolys;
extern cVar_t	*r_drawworld;
extern cVar_t	*r_facePlaneCull;
extern cVar_t	*r_flares;
extern cVar_t	*r_flareFade;
extern cVar_t	*r_flareSize;
extern cVar_t	*r_fontScale;
extern cVar_t	*r_fullbright;
extern cVar_t	*r_hwGamma;
extern cVar_t	*r_lerpmodels;
extern cVar_t	*r_lightlevel;	// FIXME: This is a HACK to get the client's light level
extern cVar_t	*r_lmMaxBlockSize;
extern cVar_t	*r_lmModulate;
extern cVar_t	*r_lmPacking;
extern cVar_t	*r_noCull;
extern cVar_t	*r_noRefresh;
extern cVar_t	*r_noVis;
extern cVar_t	*r_offsetFactor;
extern cVar_t	*r_offsetUnits;
extern cVar_t	*r_patchDivLevel;
extern cVar_t	*r_roundImagesDown;
extern cVar_t	*r_skipBackend;
extern cVar_t	*r_speeds;
extern cVar_t	*r_sphereCull;
extern cVar_t	*r_swapInterval;
extern cVar_t	*r_textureBits;
extern cVar_t	*r_times;
extern cVar_t	*r_vertexLighting;
extern cVar_t	*r_zFarAbs;
extern cVar_t	*r_zFarMin;
extern cVar_t	*r_zNear;

extern cVar_t	*r_alphabits;
extern cVar_t	*r_colorbits;
extern cVar_t	*r_depthbits;
extern cVar_t	*r_stencilbits;
extern cVar_t	*cl_stereo;
extern cVar_t	*gl_allow_software;
extern cVar_t	*gl_stencilbuffer;

extern cVar_t	*vid_gammapics;
extern cVar_t	*vid_gamma;
extern cVar_t	*vid_width;
extern cVar_t	*vid_height;

extern cVar_t	*intensity;

extern cVar_t	*gl_nobind;
extern cVar_t	*gl_picmip;
extern cVar_t	*gl_screenshot;
extern cVar_t	*gl_texturemode;

/*
=============================================================================

	SUPPORTING FUNCTIONS

=============================================================================
*/

//
// r_math.c
//

void		R_SetupProjectionMatrix (refDef_t *rd, mat4x4_t m);
void		R_SetupModelviewMatrix (refDef_t *rd, mat4x4_t m);

void		Matrix4_Multiply_Vector (const mat4x4_t m, const vec4_t v, vec4_t out);

//
// rb_batch.c
//

qBool		RB_BackendOverflow (int numVerts, int numIndexes);
qBool		RB_InvalidMesh (const mesh_t *mesh);
void		RB_PushMesh (mesh_t *mesh, meshFeatures_t meshFeatures);

//
// rf_init.c
//

qBool		R_GetInfoForMode (int mode, int *width, int *height);

//
// rb_light.c
//

void		RB_DrawDLights (void);

//
// rb_math.c
//

float		RB_FastSin (float t);

//
// rb_render.c
//

void		RB_LockArrays (int numVerts);
void		RB_UnlockArrays (void);
void		RB_ResetPointers (void);

void		RB_RenderMeshBuffer (meshBuffer_t *mb, qBool shadowPass);
void		RB_FinishRendering (void);

void		RB_BeginTriangleOutlines (void);
void		RB_EndTriangleOutlines (void);

void		RB_BeginFrame (void);
void		RB_EndFrame (void);

void		RB_Init (void);
void		RB_Shutdown (void);

//
// rb_shadow.c
//

#ifdef SHADOW_VOLUMES
void		RB_SetShadowState (qBool start);
void		RB_DrawShadowVolumes (mesh_t *mesh, refEntity_t *ent, vec3_t mins, vec3_t maxs, float radius);
void		RB_ShadowBlend (void);
#endif

void		RB_SimpleShadow (refEntity_t *ent, vec3_t shadowSpot);

//
// rb_state.c
//

void		RB_BindTexture (image_t *image);

void		RB_SetupGL2D (void);
void		RB_SetupGL3D (void);
void		RB_ClearBuffers (void);

void		RB_SetDefaultState (void);

//
// rf_cull.c
//

void		R_SetupFrustum (void);

qBool		R_CullBox (vec3_t mins, vec3_t maxs, int clipFlags);
qBool		R_CullSphere (const vec3_t origin, const float radius, int clipFlags);
qBool		R_CullNode (struct mBspNode_s *node);
qBool		R_CullSurface (struct mBspSurface_s *surf);

//
// rf_decal.c
//

void		R_AddDecalsToList (void);
void		R_PushDecal (meshBuffer_t *mb, meshFeatures_t features);
qBool		R_DecalOverflow (meshBuffer_t *mb);
void		R_DecalInit (void);
void		R_ClearDecals (void);

//
// rf_font.c
//

void		R_EndFontRegistration (void);

void		R_CheckFont (void);

void		R_FontInit (void);
void		R_FontShutdown (void);

//
// rf_light.c
//

void		R_Q2BSP_MarkWorldLights (void);
void		R_Q2BSP_MarkBModelLights (refEntity_t *ent, vec3_t mins, vec3_t maxs);

void		R_Q2BSP_UpdateLightmap (mBspSurface_t *surf);
void		R_Q2BSP_BeginBuildingLightmaps (void);
void		R_Q2BSP_CreateSurfaceLightmap (mBspSurface_t *surf);
void		R_Q2BSP_EndBuildingLightmaps (void);

void		R_Q3BSP_MarkWorldLights (void);
void		R_Q3BSP_MarkBModelLights (refEntity_t *ent, vec3_t mins, vec3_t maxs);

void		R_Q3BSP_BuildLightmaps (int numLightmaps, int w, int h, const byte *data, mQ3BspLightmapRect_t *rects);

void		R_LightBounds (const vec3_t origin, float intensity, vec3_t mins, vec3_t maxs);
void		R_LightPoint (vec3_t point, vec3_t light);
void		R_SetLightLevel (void);

void		R_TouchLightmaps (void);

qBool		R_ShadowForEntity (refEntity_t *ent, vec3_t shadowSpot);
void		R_LightForEntity (refEntity_t *ent, int numVerts, byte *bArray);

//
// rf_main.c
//

void		R_PushPoly (meshBuffer_t *mb, meshFeatures_t features);
qBool		R_PolyOverflow (meshBuffer_t *mb);
void		R_PolyInit (void);

void		R_EntityInit (void);

void		R_TransformToScreen_Vec3 (vec3_t in, vec3_t out);
void		R_RenderToList (refDef_t *rd, meshList_t *list);

void		GL_CheckForError (char *where);

//
// rf_sky.c
//

#define SKY_MAXCLIPVERTS	64
#define SKY_BOXSIZE			8192

void		R_ClipSkySurface (mBspSurface_t *surf);
void		R_AddSkyToList (void);

void		R_ClearSky (void);
void		R_DrawSky (meshBuffer_t *mb);

void		R_SetSky (char *name, float rotate, vec3_t axis);

void		R_SkyInit (void);
void		R_SkyShutdown (void);

//
// rf_world.c
//

void		R_AddQ2BrushModel (refEntity_t *ent);
void		R_AddQ3BrushModel (refEntity_t *ent);
void		R_AddWorldToList (void);

void		R_WorldInit (void);
void		R_WorldShutdown (void);

/*
=============================================================================

	IMPLEMENTATION SPECIFIC

=============================================================================
*/

extern cVar_t	*vid_fullscreen;
extern cVar_t	*vid_xpos;
extern cVar_t	*vid_ypos;

//
// glimp_imp.c
//

void	GLimp_BeginFrame (void);
void	GLimp_EndFrame (void);

void	GLimp_AppActivate (qBool active);
qBool	GLimp_GetGammaRamp (uint16 *ramp);
void	GLimp_SetGammaRamp (uint16 *ramp);

void	GLimp_Shutdown (qBool full);
qBool	GLimp_Init (void);
qBool	GLimp_AttemptMode (qBool fullScreen, int width, int height);
