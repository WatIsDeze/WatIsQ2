cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# Enforce "Debug" as standard build type.
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# CMake project configuration.
project(WiQ2)

# Cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/stuff/cmake/modules ${CMAKE_MODULE_PATH})

# Add extended path for FreeBSD and Homebrew on OS X.
list(APPEND CMAKE_PREFIX_PATH /usr/local)

# Enforce C compiler flags:
#  -Wall                -> More warnings
#  -fno-strict-aliasing -> Quake 2 is far away from strict aliasing
#  -fwrapv              -> Make signed integer overflows defined
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -fno-strict-aliasing -fwrapv")

# Enforce CXX compiler flags:
#  -Wall                    -> More warnings
#  -fno-strict-aliasing     -> Quake 2 is far away from strict aliasing
#  -fwrapv                  -> Make signed integer overflows defined
#  -fno-writable-strings	-> Prevent 'warning: ISO C++11 and up does not allow conversion from string literal to 'char *' [-Wwritable-strings]' warnings.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -fno-strict-aliasing -Wno-writable-strings -fwrapv")

# Switch of some annoying warnings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-braces")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	if (CMAKE_CXX_COMPILER_VERSION GREATER 7.99)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-truncation -Wno-format-overflow")
	endif()
endif()

# Use -O2 as maximum optimization level. -O3 has it's problems with .
#string(REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

# Build Targets.
option(BUILD_WiQ2 "Build WatIsQ2 executable" ON)
option(BUILD_WiQ2_DED "Build WatIsQ2 dedicated server executable" OFF)
option(BUILD_WiQ2_GAME "Build WatIsQ2 Game DLL" ON)
option(BUILD_WiQ2_CGAME "Build WatIsQ2 Client CGame DLL" ON)

# Set directory locations (allowing us to move directories easily)
set(WiQ2_SRCDIR_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(WiQ2_SRCDIR_CGAME ${WiQ2_SRCDIR_ROOT_DIR}/cgame)
set(WiQ2_SRCDIR_CLIENT ${WiQ2_SRCDIR_ROOT_DIR}/client)
set(WiQ2_SRCDIR_COMMON ${WiQ2_SRCDIR_ROOT_DIR}/common)
set(WiQ2_SRCDIR_GAME ${WiQ2_SRCDIR_ROOT_DIR}/game)
set(WiQ2_SRCDIR_RENDERER ${WiQ2_SRCDIR_ROOT_DIR}/renderer)
set(WiQ2_SRCDIR_RMLUI ${WiQ2_SRCDIR_ROOT_DIR}/rmlui)
set(WiQ2_SRCDIR_SERVER ${WiQ2_SRCDIR_ROOT_DIR}/server)
set(WiQ2_SRCDIR_SHARED ${WiQ2_SRCDIR_ROOT_DIR}/shared)
set(WiQ2_SRCDIR_UNIX ${WiQ2_SRCDIR_ROOT_DIR}/unix)
set(WiQ2_SRCDIR_WIN32 ${WiQ2_SRCDIR_ROOT_DIR}/win32)

# Required libraries to build the different components of the binaries. Find
find_package(SDL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(X11 REQUIRED)
find_package(GLEW REQUIRED)

# Lua
find_package(Lua51 REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

# Minizip..
INCLUDE (FindPkgConfig)
if (PKG_CONFIG_FOUND)
        PKG_CHECK_MODULES(UNZIP minizip)
endif (PKG_CONFIG_FOUND)

# RmlUI.
include_directories(libraries/RmlUi/Include)
link_directories(libraries/RmlUi/build)

# General linker flags.
set(WiQ2_LINKER_FLAGS "")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	list(APPEND WiQ2_LINKER_FLAGS "-lm -static-libgcc")
else()
	if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Haiku")
		list(APPEND WiQ2_LINKER_FLAGS "-lm -rdynamic")
	else()
		list(APPEND WiQ2_LINKER_FLAGS "-lm -lnetwork")
	endif()
endif()

list(APPEND WiQ2_LINKER_FLAGS ${CMAKE_DL_LIBS})

###
# Set CGAME Source Files.
###
set(WiQ2_SRC_CGAME
#    ${WiQ2_SRCDIR_CGAME}/gmenu/menu_id_1.c
    
    ${WiQ2_SRCDIR_CGAME}/menu/m_local.h
    ${WiQ2_SRCDIR_CGAME}/menu/m_main.cpp
    ${WiQ2_SRCDIR_CGAME}/menu/m_mp.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_mp_downloading.cpp
    ${WiQ2_SRCDIR_CGAME}/menu/m_mp_join.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_mp_join_addrbook.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_mp_player.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_mp_start.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_mp_start_dmflags.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_opts.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_opts_controls.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_opts_effects.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_opts_gloom.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_opts_hud.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_opts_input.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_opts_misc.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_opts_screen.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_opts_sound.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_quit.cpp
    ${WiQ2_SRCDIR_CGAME}/menu/m_sp.cpp
    ${WiQ2_SRCDIR_CGAME}/menu/m_sp_credits.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_sp_loadgame.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_sp_savegame.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/m_vid.cpp
    ${WiQ2_SRCDIR_CGAME}/menu/m_vid_exts.cpp
    ${WiQ2_SRCDIR_CGAME}/menu/m_vid_settings.cpp 
    ${WiQ2_SRCDIR_CGAME}/menu/menu.cpp
    ${WiQ2_SRCDIR_CGAME}/menu/menu.h
    
    ${WiQ2_SRCDIR_CGAME}/ui/ui_backend.cpp
    ${WiQ2_SRCDIR_CGAME}/ui/ui_cursor.cpp
    ${WiQ2_SRCDIR_CGAME}/ui/ui_draw.cpp
    ${WiQ2_SRCDIR_CGAME}/ui/ui_items.cpp
    ${WiQ2_SRCDIR_CGAME}/ui/ui_keys.cpp
    ${WiQ2_SRCDIR_CGAME}/ui/ui_local.h
    
    ${WiQ2_SRCDIR_CGAME}/cg_api.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_decals.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_draw.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_entities.cpp
    ${WiQ2_SRCDIR_CGAME}/cg_hud.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_inventory.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_keys.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_light.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_loadscreen.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_localents.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_location.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_main.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_mapeffects.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_media.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_muzzleflash.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_parse.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_parteffects.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_partgloom.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_particles.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_partsustain.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_partthink.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_parttrail.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_players.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_predict.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_screen.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_tempents.cpp
    ${WiQ2_SRCDIR_CGAME}/cg_view.cpp 
    ${WiQ2_SRCDIR_CGAME}/cg_weapon.cpp 
    
    ${WiQ2_SRCDIR_CGAME}/pmove.cpp

    ${WiQ2_SRCDIR_SHARED}/byteswap.cpp
    ${WiQ2_SRCDIR_SHARED}/infostrings.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_angles.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_bounds.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_flash.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_mat3.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_mat4.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_plane.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_quat.cpp 
    ${WiQ2_SRCDIR_SHARED}/mathlib.cpp 
    ${WiQ2_SRCDIR_SHARED}/mersennetwister.cpp
    ${WiQ2_SRCDIR_SHARED}/shared.cpp 
    ${WiQ2_SRCDIR_SHARED}/string.cpp
)

###
# Set GAME Source Files.
###
set(WiQ2_SRC_GAME
    ${WiQ2_SRCDIR_GAME}/g_ai.cpp 
    ${WiQ2_SRCDIR_GAME}/g_chase.cpp
    ${WiQ2_SRCDIR_GAME}/g_cmds.cpp
    ${WiQ2_SRCDIR_GAME}/g_combat.cpp 
    ${WiQ2_SRCDIR_GAME}/g_func.cpp 
    ${WiQ2_SRCDIR_GAME}/g_items.cpp 
    ${WiQ2_SRCDIR_GAME}/g_main.cpp 
    ${WiQ2_SRCDIR_GAME}/g_menus.cpp 
    ${WiQ2_SRCDIR_GAME}/g_misc.cpp 
    ${WiQ2_SRCDIR_GAME}/g_monster.cpp 
    ${WiQ2_SRCDIR_GAME}/g_phys.cpp 
    ${WiQ2_SRCDIR_GAME}/g_save.cpp 
    ${WiQ2_SRCDIR_GAME}/g_spawn.cpp 
    ${WiQ2_SRCDIR_GAME}/g_svcmds.cpp 
    ${WiQ2_SRCDIR_GAME}/g_target.cpp 
    ${WiQ2_SRCDIR_GAME}/g_trigger.cpp 
    ${WiQ2_SRCDIR_GAME}/g_turret.cpp 
    ${WiQ2_SRCDIR_GAME}/g_utils.cpp 
    ${WiQ2_SRCDIR_GAME}/g_weapon.cpp 
    ${WiQ2_SRCDIR_GAME}/m_actor.cpp 
    ${WiQ2_SRCDIR_GAME}/m_berserk.cpp 
    ${WiQ2_SRCDIR_GAME}/m_boss2.cpp 
    ${WiQ2_SRCDIR_GAME}/m_boss3.cpp 
    ${WiQ2_SRCDIR_GAME}/m_boss31.cpp 
    ${WiQ2_SRCDIR_GAME}/m_boss32.cpp 
    ${WiQ2_SRCDIR_GAME}/m_brain.cpp 
    ${WiQ2_SRCDIR_GAME}/m_chick.cpp 
    ${WiQ2_SRCDIR_GAME}/m_flipper.cpp 
    ${WiQ2_SRCDIR_GAME}/m_float.cpp 
    ${WiQ2_SRCDIR_GAME}/m_flyer.cpp 
    ${WiQ2_SRCDIR_GAME}/m_gladiator.cpp 
    ${WiQ2_SRCDIR_GAME}/m_gunner.cpp 
    ${WiQ2_SRCDIR_GAME}/m_hover.cpp 
    ${WiQ2_SRCDIR_GAME}/m_infantry.cpp 
    ${WiQ2_SRCDIR_GAME}/m_insane.cpp 
    ${WiQ2_SRCDIR_GAME}/m_medic.cpp 
    ${WiQ2_SRCDIR_GAME}/m_move.cpp 
    ${WiQ2_SRCDIR_GAME}/m_mutant.cpp 
    ${WiQ2_SRCDIR_GAME}/m_parasite.cpp 
    ${WiQ2_SRCDIR_GAME}/m_soldier.cpp 
    ${WiQ2_SRCDIR_GAME}/m_supertank.cpp 
    ${WiQ2_SRCDIR_GAME}/m_tank.cpp 
    ${WiQ2_SRCDIR_GAME}/p_client.cpp 
    ${WiQ2_SRCDIR_GAME}/p_hud.cpp 
    ${WiQ2_SRCDIR_GAME}/p_trail.cpp 
    ${WiQ2_SRCDIR_GAME}/p_view.cpp 
    ${WiQ2_SRCDIR_GAME}/p_weapon.cpp

    ${WiQ2_SRCDIR_SHARED}/byteswap.cpp
    ${WiQ2_SRCDIR_SHARED}/infostrings.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_angles.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_bounds.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_flash.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_mat3.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_mat4.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_plane.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_quat.cpp 
    ${WiQ2_SRCDIR_SHARED}/mathlib.cpp 
    ${WiQ2_SRCDIR_SHARED}/mersennetwister.cpp
    ${WiQ2_SRCDIR_SHARED}/shared.cpp 
    ${WiQ2_SRCDIR_SHARED}/string.cpp
)

###
# Set ENGINE EXECUTABLE Source Files.
###
set(WiQ2_SRC_ENGINE
    ${WiQ2_SRCDIR_CLIENT}/cl_acapi.cpp
    ${WiQ2_SRCDIR_CLIENT}/cl_cgapi.cpp 
    ${WiQ2_SRCDIR_CLIENT}/cl_cin.cpp 
    ${WiQ2_SRCDIR_CLIENT}/cl_console.cpp 
    ${WiQ2_SRCDIR_CLIENT}/cl_demo.cpp 
    ${WiQ2_SRCDIR_CLIENT}/cl_download.cpp 
    ${WiQ2_SRCDIR_CLIENT}/cl_input.cpp 
    ${WiQ2_SRCDIR_CLIENT}/cl_keys.cpp 
    ${WiQ2_SRCDIR_CLIENT}/cl_main.cpp 
    ${WiQ2_SRCDIR_CLIENT}/cl_parse.cpp 
    ${WiQ2_SRCDIR_CLIENT}/cl_rmlui.cpp
    ${WiQ2_SRCDIR_CLIENT}/cl_screen.cpp 
    ${WiQ2_SRCDIR_CLIENT}/gui_cursor.cpp 
    ${WiQ2_SRCDIR_CLIENT}/gui_draw.cpp 
    ${WiQ2_SRCDIR_CLIENT}/gui_events.cpp 
    ${WiQ2_SRCDIR_CLIENT}/gui_items.cpp 
    ${WiQ2_SRCDIR_CLIENT}/gui_init.cpp
    ${WiQ2_SRCDIR_CLIENT}/gui_keys.cpp 
    ${WiQ2_SRCDIR_CLIENT}/gui_main.cpp 
    ${WiQ2_SRCDIR_CLIENT}/gui_vars.cpp
    ${WiQ2_SRCDIR_CLIENT}/sdl_main.cpp 
    ${WiQ2_SRCDIR_CLIENT}/sdl_glimp.cpp
    ${WiQ2_SRCDIR_CLIENT}/snd_dma.cpp 
    ${WiQ2_SRCDIR_CLIENT}/snd_main.cpp
    ${WiQ2_SRCDIR_CLIENT}/snd_openal.cpp
    ${WiQ2_SRCDIR_CLIENT}/snd_sdl.cpp

    ${WiQ2_SRCDIR_COMMON}/alias.cpp
    ${WiQ2_SRCDIR_COMMON}/cbuf.cpp 
    ${WiQ2_SRCDIR_COMMON}/cm_common.cpp 
    ${WiQ2_SRCDIR_COMMON}/cm_q2_main.cpp
    ${WiQ2_SRCDIR_COMMON}/cm_q2_trace.cpp
    ${WiQ2_SRCDIR_COMMON}/cm_q3_main.cpp
    ${WiQ2_SRCDIR_COMMON}/cm_q3_trace.cpp 
    ${WiQ2_SRCDIR_COMMON}/cm_q3_patch.cpp 
    ${WiQ2_SRCDIR_COMMON}/cmd.cpp 
    ${WiQ2_SRCDIR_COMMON}/common.cpp 
    ${WiQ2_SRCDIR_COMMON}/crc.cpp 
    ${WiQ2_SRCDIR_COMMON}/cvar.cpp 
    ${WiQ2_SRCDIR_COMMON}/files.cpp 
    ${WiQ2_SRCDIR_COMMON}/md4.cpp 
    ${WiQ2_SRCDIR_COMMON}/memory.cpp 
    ${WiQ2_SRCDIR_COMMON}/net_chan.cpp 
    ${WiQ2_SRCDIR_COMMON}/net_msg.cpp 
    ${WiQ2_SRCDIR_COMMON}/parse.cpp

    ${WiQ2_SRCDIR_RENDERER}/r_math.cpp
    ${WiQ2_SRCDIR_RENDERER}/rb_batch.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rb_entity.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rb_light.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rb_math.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rb_gl.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rb_render.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rb_shadow.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rb_state.cpp 

    ${WiQ2_SRCDIR_RENDERER}/rf_2d.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_alias.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_cull.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_decal.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_font.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_image.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_init.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_light.cpp
    ${WiQ2_SRCDIR_RENDERER}/rf_main.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_material.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_meshbuffer.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_model.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_program.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_sky.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_sprite.cpp 
    ${WiQ2_SRCDIR_RENDERER}/rf_world.cpp

    ${WiQ2_SRCDIR_RMLUI}/SystemInterface.cpp
    ${WiQ2_SRCDIR_RMLUI}/RenderInterface.cpp
    ${WiQ2_SRCDIR_RMLUI}/FileInterface.cpp

    ${WiQ2_SRCDIR_SERVER}/sv_ccmds.cpp 
    ${WiQ2_SRCDIR_SERVER}/sv_ents.cpp 
    ${WiQ2_SRCDIR_SERVER}/sv_gameapi.cpp 
    ${WiQ2_SRCDIR_SERVER}/sv_init.cpp 
    ${WiQ2_SRCDIR_SERVER}/sv_main.cpp 
    ${WiQ2_SRCDIR_SERVER}/sv_pmove.cpp 
    ${WiQ2_SRCDIR_SERVER}/sv_send.cpp 
    ${WiQ2_SRCDIR_SERVER}/sv_user.cpp 
    ${WiQ2_SRCDIR_SERVER}/sv_world.cpp 

    ${WiQ2_SRCDIR_SHARED}/byteswap.cpp
    ${WiQ2_SRCDIR_SHARED}/infostrings.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_angles.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_bounds.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_flash.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_mat3.cpp
    ${WiQ2_SRCDIR_SHARED}/m_mat4.cpp
    ${WiQ2_SRCDIR_SHARED}/m_plane.cpp 
    ${WiQ2_SRCDIR_SHARED}/m_quat.cpp 
    ${WiQ2_SRCDIR_SHARED}/mathlib.cpp 
    ${WiQ2_SRCDIR_SHARED}/mersennetwister.cpp 
    ${WiQ2_SRCDIR_SHARED}/shared.cpp 
    ${WiQ2_SRCDIR_SHARED}/string.cpp 
)

# Append the proper Operating System specific code to the executable file its sources.
if(UNIX)
    list(APPEND WiQ2_SRC_ENGINE
        ${WiQ2_SRCDIR_UNIX}/unix_console.cpp
        ${WiQ2_SRCDIR_UNIX}/unix_main.cpp 
        ${WiQ2_SRCDIR_UNIX}/unix_snd_cd.cpp 
        ${WiQ2_SRCDIR_UNIX}/unix_udp.cpp
    )
elseif(WIN32)
    list(APPEND WiQ2_SRC_ENGINE 
        ${WiQ2_SRCDIR_WIN32}/win_console.cpp 
        ${WiQ2_SRCDIR_WIN32}/win_main.cpp 
        ${WiQ2_SRCDIR_WIN32}/win_snd_cd.cpp 
        ${WiQ2_SRCDIR_WIN32}/win_sock.cpp 
    )
endif()

# Build the cgame(client game) dynamic library
if (${BUILD_WiQ2_CGAME})
	add_library(cgame MODULE ${WiQ2_SRC_CGAME})
	set_target_properties(cgame PROPERTIES
			PREFIX ""
			LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/baseq2
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/baseq2
			SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX}
			)
	target_link_libraries(cgame ${WiQ2_LINKER_FLAGS})
endif()

# Build the game dynamic library
if (${BUILD_WiQ2_GAME})
	add_library(game MODULE ${WiQ2_SRC_GAME})
	set_target_properties(game PROPERTIES
			PREFIX ""
			LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/baseq2
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/baseq2
			SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX}
			)
	target_link_libraries(game ${WiQ2_LINKER_FLAGS})
endif()

# Build the WiQ2 executable
if(${BUILD_WiQ2})
    add_executable(WiQ2 ${WiQ2_SRC_ENGINE})
    set_target_properties(WiQ2 PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
            )
    if(UNIX)
        target_link_libraries(WiQ2
            ${WiQ2_LINKER_FLAGS} 
            ${SDL_LIBRARY} 
            ${OPENGL_LIBRARY}
            ${GLEW_LIBRARIES}
            ${OPENAL_LIBRARY} 
            ${PNG_LIBRARIES} 
            ${JPEG_LIBRARIES} 
            ${ZLIB_LIBRARIES} 
            ${UNZIP_LIBRARIES} 
            ${X11_LIBRARIES}

            ${LUA_LIBRARIES}
            RmlCore
            RmlDebugger
            RmlLua
            #shell
        )
    elseif(WIN32)
        target_link_libraries(WiQ2_${ARCH}
            ${WiQ2_LINKER_FLAGS} 
            ${SDL_LIBRARY} 
            ${OPENAL_LIBRARY} 
            ${OPENGL_LIBRARY}
            ${GLEW_LIBRARY}
            ${PNG_LIBRARIES} 
            ${JPEG_LIBRARIES} 
            ${ZLIB_LIBRARIES} 
            ${UNZIP_LIBRARIES} 
            ${X11_LIBRARIES}
            ws2_32
            winmm
        )
    else()
        
    endif()
endif()

