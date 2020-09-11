#include <RmlUi/Core.h>

#include "../shared/shared.h"
#include "../common/common.h"

#include "RenderInterface.h"

/*
==================
RmlUi_RenderInterface::RmlUi_RenderInterface

Constructor.
==================
*/
RmlUi_RenderInterface::RmlUi_RenderInterface() {

}

/*
==================
RmlUi_RenderInterface::RmlUi_RenderInterface

Destructor.
==================
*/
RmlUi_RenderInterface::~RmlUi_RenderInterface() {
    
}

/*
==================
RmlUi_RenderInterface::RenderGeometry

Called by RmlUi when it wants to render geometry that it does not wish to optimise.
==================
*/
void RmlUi_RenderInterface::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) {

}

/*
==================
RmlUi_RenderInterface::EnableScissorRegion

Called by RmlUi when it wants to enable or disable scissoring to clip content.
==================
*/
void RmlUi_RenderInterface::EnableScissorRegion(bool enable) {

}

/*
==================
RmlUi_RenderInterface::SetScissorRegion

Called by RmlUi when it wants to change the scissor region.
==================
*/
void RmlUi_RenderInterface::SetScissorRegion(int x, int y, int width, int height) {

}

/*
==================
RmlUi_RenderInterface::LoadTexture

Called by RmlUi when a texture is required.
==================
*/
bool RmlUi_RenderInterface::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) {

}

/*
==================
bool RmlUi_RenderInterface::GenerateTexture

Called by RmlUi when a texture is required to be built from an internally-generated sequence of pixels.
==================
*/
bool RmlUi_RenderInterface::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) {

}

/*
==================
void RmlUi_RenderInterface::ReleaseTexture

Called by RmlUi when a loaded texture is no longer required.
==================
*/
void RmlUi_RenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle) {

}