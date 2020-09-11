// When including after common.h it collides with DotProduct somehow.
// RmlUi Core includes.
#define RMLUI_STATIC_LIB
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <RmlUi/Lua.h>

#include "cl_local.h"

// RmlUi include.
#include "../rmlui/SystemInterface.h"
#include "../rmlui/RenderInterface.h"
#include "../rmlui/FileInterface.h"

// RmlUi interfaces.
static std::unique_ptr<RmlUi_SystemInterface> rmSystemInterface;
static std::unique_ptr<RmlUi_RenderInterface> rmRenderInterface;
static std::unique_ptr<RmlUi_FileInterface>   rmFileInterface;

// RmlUi context.
static Rml::Context* rmMainContext = NULL;

// FIXME: Actually pass width and height externally to the RenderInterface.
// use a set function?
#include "../renderer/r_local.h"
#include "../renderer/r_public.h"
extern refInfo_t	ri;


/*
==================
CL_RmlUI_Init

Initializes RmlUi and its interfaces and loads in the mainmenu RML document.
==================
*/
void CL_RmlUI_Init() {
	Com_Printf (0, "\n--------- RmlUI Initialization ---------\n");
    // Initialize RmlUi engine binding interfaces.
    rmSystemInterface = std::make_unique<RmlUi_SystemInterface>();
    rmRenderInterface = std::make_unique<RmlUi_RenderInterface>();
    rmFileInterface = std::make_unique<RmlUi_FileInterface>();
    
    // Initialize Rml.
    Rml::SetSystemInterface(rmSystemInterface.get());
    Rml::SetRenderInterface(rmRenderInterface.get());
    //Rml::SetFileInterface(rmFileInterface.get());
    
    if (!Rml::Initialise()) {
        Com_Error(eComError_t::ERR_FATAL, "%s", "Failed to initialize RmlUi library.");
        return;
    }

    // Initialize Rml Lua bindings.
  //  Rml::Lua::Initialise();

    // Load our default font faces.
	Rml::LoadFontFace("baseq2/gui/assets/Delicious-Bold.otf");
	Rml::LoadFontFace("baseq2/gui/assets/Delicious-BoldItalic.otf");
	Rml::LoadFontFace("baseq2/gui/assets/Delicious-Italic.otf");
	Rml::LoadFontFace("baseq2/gui/assets/Delicious-Roman.otf");

    // Create our default context.
	Rml::Context* rmMainContext = Rml::CreateContext("default",
		Rml::Vector2i(ri.config.vidWidth, ri.config.vidHeight));

    // Initialize debugger context.
	//Rml::Debugger::Initialise(rmMainContext);

    // MainMenu document.
	Rml::ElementDocument* Document = rmMainContext->LoadDocument("baseq2/gui/assets/demo.rml");

    // Show it.
	if (Document)
	{
		Document->Show();
		Com_Printf(0, "Loaded MainMenu\n");
	}
	else
	{
		Com_Printf(0, "Failed to load baseq2/gui/assets/demo.rml\n");
	}
	
	Com_Printf (0, "----------------------------------------\n");
}
auto lambda = [](auto x, auto y) {return x + y;};
/*
==================
CL_RmlUI_Frame

Called each client frame in order to update Rml
==================
*/
void CL_RmlUI_Frame() {
    if (rmMainContext) {
		RB_SetupGL2D();
		glDisable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);
		rmMainContext->Render();
    	rmMainContext->Update();
	}
}

/*
==================
CL_RmlUI_Shutdown() {

Shuts down Rml and deallocs its interface ptrs.
==================
*/
void CL_RmlUI_Shutdown() {
	// Shutdown RmlUi.
	Rml::Shutdown();

	// Delete its interfaces from memory.
	rmFileInterface.reset();
	rmSystemInterface.reset();
	rmRenderInterface.reset();
}