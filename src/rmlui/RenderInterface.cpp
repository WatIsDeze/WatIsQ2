#include <RmlUi/Core.h>

#include "../shared/shared.h"
#include "../common/common.h"

#include "RenderInterface.h"

// FIXME: Actually pass width and height externally to the RenderInterface.
// use a set function?
#include "../renderer/r_local.h"
#include "../renderer/r_public.h"
extern refInfo_t	ri;

// Included here due to glew.h being included in r_local.h...
#include <GL/gl.h>

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
    Rml::Vector<Rml::Vector2f> Positions(num_vertices);
    Rml::Vector<Rml::Colourb> Colors(num_vertices);
    Rml::Vector<Rml::Vector2f> TexCoords(num_vertices);
    float texw = 0.0f;
    float texh = 0.0f;

    // Push matrix and translate.
    glPushMatrix();
    glTranslatef(translation.x, translation.y, 0);
    
    if (texture && textureSizes.find((GLuint)texture) != textureSizes.end()) {
        texw = textureSizes[(GLuint)texture][0];
        texh = textureSizes[(GLuint)texture][1];
        Com_Printf(0, "WTF");
    }

    // Fetch texture width and height based on image ID.
    for(int  i = 0; i < num_vertices; i++) {
        Positions[i] = vertices[i].position;
        Colors[i] = vertices[i].colour;
        if (texture) {
            TexCoords[i].x = vertices[i].tex_coord.x * texw;
            TexCoords[i].y = vertices[i].tex_coord.y * texh;
        }
        else TexCoords[i] = vertices[i].tex_coord;
    };
 
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, &Positions[0]);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, &Colors[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &TexCoords[0]);
 
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
 
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
 
    // Reset default color.
    glColor4f(1.0, 1.0, 1.0, 1.0);

    // Disable blendering.
    glDisable(GL_BLEND);

    // Pop matrix.
    glPopMatrix();
}

/*
==================
RmlUi_RenderInterface::EnableScissorRegion

Called by RmlUi when it wants to enable or disable scissoring to clip content.
==================
*/
void RmlUi_RenderInterface::EnableScissorRegion(bool enable) {
    if (enable)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);
}

/*
==================
RmlUi_RenderInterface::SetScissorRegion

Called by RmlUi when it wants to change the scissor region.
==================
*/
void RmlUi_RenderInterface::SetScissorRegion(int x, int y, int width, int height) {
    int windowWidth = ri.config.vidWidth;
    int windowHeight = ri.config.vidHeight;
    glScissor(x, windowHeight - (y + height), width, height);
}

/*
==================
RmlUi_RenderInterface::LoadTexture

Called by RmlUi when a texture is required.
==================
*/
bool RmlUi_RenderInterface::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) {
	// Texture width, height, samples.
	int32_t width = 0, height = 0, samples = 0;
	// Texture pixel data.
    byte *pixelData = nullptr;

    // Determine whether the image is a png, jpg, or tga.
    if (source.find(".png", source.size() - 4) != std::string::npos) {
        R_LoadPNG((char*)source.c_str(), &pixelData, &width, &height, &samples);
        if (!pixelData) {
            Rml::Log::Message(Rml::Log::Type::LT_WARNING, "Failed to load image: %s", source.c_str());
            return false;
        }
    } else if (source.find(".tga", source.size() - 4) != std::string::npos) {
        R_LoadTGA((char*)source.c_str(), &pixelData, &width, &height, &samples);
        if (!pixelData) {
            Rml::Log::Message(Rml::Log::Type::LT_WARNING, "Failed to load image: %s", source.c_str());
            return false;
        }
    } else if (source.find(".jpg", source.size() - 4) != std::string::npos) {
        R_LoadJPG((char*)source.c_str(), &pixelData, &width, &height);
        if (!pixelData) {
            Rml::Log::Message(Rml::Log::Type::LT_WARNING, "Failed to load image: %s", source.c_str());
            return false;
        }
    } else {
        Rml::Log::Message(Rml::Log::Type::LT_WARNING, "Image of unsupported format prived: %s", source.c_str());
        texture_handle = 0;
        return false;
    }
    
    // If we've reached this point the image has been sucessfully loaded.
    // Generate and upload the OpenGL texture.
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
    
    // Last but not least, upload info to RmlUi.
    texture_handle = textureID;
    texture_dimensions.x = width;
    texture_dimensions.y = height;
    
    // Allocate and set textureSize in this slot.
    textureSizes[texture_handle][0] = width;
    textureSizes[texture_handle][1] = height;

    return true;
}

/*
==================
bool RmlUi_RenderInterface::GenerateTexture

Called by RmlUi when a texture is required to be built from an internally-generated sequence of pixels.
==================
*/
bool RmlUi_RenderInterface::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) {
    if (!source)
        return false;
    
    // If we've reached this point the image has been sucessfully loaded.
    // Generate and upload the OpenGL texture.
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, source_dimensions.x, source_dimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, source);
    
    // Last but not least, upload info to RmlUi.
    texture_handle = textureID;
    
    return true;
}

/*
==================
void RmlUi_RenderInterface::ReleaseTexture

Called by RmlUi when a loaded texture is no longer required.
==================
*/
void RmlUi_RenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle) {
    // In case it exists in our textureSizes list, remove it.
    if (textureSizes.find(texture_handle) != textureSizes.end()) {
        textureSizes.erase(texture_handle);
    }
    // Delete texture from GPU.
    glDeleteTextures(1, (const GLuint*)&texture_handle);
}