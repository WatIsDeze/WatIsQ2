#ifndef _RMLUI_RENDERINTERFACE_H_
#define _RMLUI_RENDERINTERFACE_H_

//==================
//  RmlUi_RenderInterface
//  
//  Implements a render interface class for RmlUi.
//  Nothing fancy.
//==================
class RmlUi_RenderInterface : public Rml::RenderInterface {
public:
    //==================
    // Constructor/Destructor.
    //==================
    RmlUi_RenderInterface();
    ~RmlUi_RenderInterface();

    //==================
    // Implementation functions.
    //==================
    // Render.
	void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;

    // Scissor.
	void EnableScissorRegion(bool enable) override;
	void SetScissorRegion(int x, int y, int width, int height) override;

    // Texture.
	bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;
	bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;
	void ReleaseTexture(Rml::TextureHandle texture_handle) override;

    // TODO:
    // Implement the following compiled geometry rendering functions to replace
    // the the currently in use, immediate mode, RenderGeometry function with.
    //
    // // Called by RmlUi when it wants to compile geometry it believes will be static for the forseeable future.
    // Rml::CompiledGeometryHandle CompileGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture);
    // // Called by RmlUi when it wants to render application-compiled geometry.
    // void RenderCompiledGeometry(Rml::CompiledGeometryHandle geometry, const Rml::Vector2f& translation);
    // // Called by RmlUi when it wants to release application-compiled geometry.
    // void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle geometry);


private:
    std::map<Rml::TextureHandle, vec2_t> textureSizes;
};

#endif