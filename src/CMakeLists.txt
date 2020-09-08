cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

#############################################################################
# Fetch architecture                                                        #
#                                                                           #
# Found at: https://github.com/axr/solar-cmake/blob/master/TargetArch.cmake #
#############################################################################
set(archdetect_c_code "
#if defined(__arm__) || defined(__TARGET_ARCH_ARM)
    #if defined(__ARM_ARCH_7__) \\
        || defined(__ARM_ARCH_7A__) \\
        || defined(__ARM_ARCH_7R__) \\
        || defined(__ARM_ARCH_7M__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7)
        #error cmake_ARCH armv7
    #elif defined(__ARM_ARCH_6__) \\
        || defined(__ARM_ARCH_6J__) \\
        || defined(__ARM_ARCH_6T2__) \\
        || defined(__ARM_ARCH_6Z__) \\
        || defined(__ARM_ARCH_6K__) \\
        || defined(__ARM_ARCH_6ZK__) \\
        || defined(__ARM_ARCH_6M__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 6)
        #error cmake_ARCH armv6
    #elif defined(__ARM_ARCH_5TEJ__) \\
        || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 5)
        #error cmake_ARCH armv5
    #else
        #error cmake_ARCH arm
    #endif
#elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #error cmake_ARCH i386
#elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
    #error cmake_ARCH x86_64
#elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
    #error cmake_ARCH ia64
#elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) \\
      || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC)  \\
      || defined(_M_MPPC) || defined(_M_PPC)
    #if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
        #error cmake_ARCH ppc64
    #else
        #error cmake_ARCH ppc
    #endif
#endif
#error cmake_ARCH unknown
")

# Set ppc_support to TRUE before including this file or ppc and ppc64
# will be treated as invalid architectures since they are no longer supported by Apple

function(target_architecture output_var)
    if(APPLE AND CMAKE_OSX_ARCHITECTURES)
        # On OS X we use CMAKE_OSX_ARCHITECTURES *if* it was set
        # First let's normalize the order of the values

        # Note that it's not possible to compile PowerPC applications if you are using
        # the OS X SDK version 10.6 or later - you'll need 10.4/10.5 for that, so we
        # disable it by default
        # See this page for more information:
        # http://stackoverflow.com/questions/5333490/how-can-we-restore-ppc-ppc64-as-well-as-full-10-4-10-5-sdk-support-to-xcode-4

        # Architecture defaults to i386 or ppc on OS X 10.5 and earlier, depending on the CPU type detected at runtime.
        # On OS X 10.6+ the default is x86_64 if the CPU supports it, i386 otherwise.

        foreach(osx_arch ${CMAKE_OSX_ARCHITECTURES})
            if("${osx_arch}" STREQUAL "ppc" AND ppc_support)
                set(osx_arch_ppc TRUE)
            elseif("${osx_arch}" STREQUAL "i386")
                set(osx_arch_i386 TRUE)
            elseif("${osx_arch}" STREQUAL "x86_64")
                set(osx_arch_x86_64 TRUE)
            elseif("${osx_arch}" STREQUAL "ppc64" AND ppc_support)
                set(osx_arch_ppc64 TRUE)
            else()
                message(FATAL_ERROR "Invalid OS X arch name: ${osx_arch}")
            endif()
        endforeach()

        # Now add all the architectures in our normalized order
        if(osx_arch_ppc)
            list(APPEND ARCH ppc)
        endif()

        if(osx_arch_i386)
            list(APPEND ARCH i386)
        endif()

        if(osx_arch_x86_64)
            list(APPEND ARCH x86_64)
        endif()

        if(osx_arch_ppc64)
            list(APPEND ARCH ppc64)
        endif()
    else()
        file(WRITE "${CMAKE_BINARY_DIR}/arch.c" "${archdetect_c_code}")

        enable_language(C)

        # Detect the architecture in a rather creative way...
        # This compiles a small C program which is a series of ifdefs that selects a
        # particular #error preprocessor directive whose message string contains the
        # target architecture. The program will always fail to compile (both because
        # file is not a valid C program, and obviously because of the presence of the
        # #error preprocessor directives... but by exploiting the preprocessor in this
        # way, we can detect the correct target architecture even when cross-compiling,
        # since the program itself never needs to be run (only the compiler/preprocessor)
        try_run(
            run_result_unused
            compile_result_unused
            "${CMAKE_BINARY_DIR}"
            "${CMAKE_BINARY_DIR}/arch.c"
            COMPILE_OUTPUT_VARIABLE ARCH
            CMAKE_FLAGS CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
        )

        # Parse the architecture name from the compiler output
        string(REGEX MATCH "cmake_ARCH ([a-zA-Z0-9_]+)" ARCH "${ARCH}")

        # Get rid of the value marker leaving just the architecture name
        string(REPLACE "cmake_ARCH " "" ARCH "${ARCH}")

        # If we are compiling with an unknown architecture this variable should
        # already be set to "unknown" but in the case that it's empty (i.e. due
        # to a typo in the code), then set it to unknown
        if (NOT ARCH)
            set(ARCH unknown)
        endif()
    endif()

    set(${output_var} "${ARCH}" PARENT_SCOPE)
endfunction()

# Find arch and set arch variable.
target_architecture(ARCH)

# Enforce "Debug" as standard build type.
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# CMake project configuration.
project(WIQ2)

# Cmake module search path.
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/stuff/cmake/modules ${CMAKE_MODULE_PATH})

# Add extended path for FreeBSD and Homebrew on OS X.
list(APPEND CMAKE_PREFIX_PATH /usr/local)

# Define LIBARCH to architecture found.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}-DLIBARCH=${ARCH}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-DLIBARCH=${ARCH}")

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
option(BUILD_WIQ2 "Build WatIsQ2 executable" ON)
option(BUILD_WIQ2_DED "Build WatIsQ2 dedicated server executable" OFF)
option(BUILD_WIQ2_GAME "Build WatIsQ2 Game DLL" ON)
option(BUILD_WIQ2_CGAME "Build WatIsQ2 Client CGame DLL" ON)

# Set directory locations (allowing us to move directories easily)
set(WIQ2_SRCDIR_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(WIQ2_SRCDIR_CGAME ${WIQ2_SRCDIR_ROOT_DIR}/cgame)
set(WIQ2_SRCDIR_CLIENT ${WIQ2_SRCDIR_ROOT_DIR}/client)
set(WIQ2_SRCDIR_COMMON ${WIQ2_SRCDIR_ROOT_DIR}/common)
set(WIQ2_SRCDIR_GAME ${WIQ2_SRCDIR_ROOT_DIR}/game)
set(WIQ2_SRCDIR_RENDERER ${WIQ2_SRCDIR_ROOT_DIR}/renderer)
set(WIQ2_SRCDIR_SERVER ${WIQ2_SRCDIR_ROOT_DIR}/server)
set(WIQ2_SRCDIR_SHARED ${WIQ2_SRCDIR_ROOT_DIR}/shared)
set(WIQ2_SRCDIR_UNIX ${WIQ2_SRCDIR_ROOT_DIR}/unix)
set(WIQ2_SRCDIR_WIN32_DIR ${WIQ2_SRCDIR_ROOT_DIR}/win32)

# Required libraries to build the different components of the binaries. Find
find_package(SDL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(X11 REQUIRED)
find_package(GLEW REQUIRED)

# Minizip..
INCLUDE (FindPkgConfig)
if (PKG_CONFIG_FOUND)
        PKG_CHECK_MODULES(UNZIP minizip)
endif (PKG_CONFIG_FOUND)

# General linker flags.
set(WIQ2_LINKER_FLAGS "")
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	list(APPEND WIQ2_LINKER_FLAGS "-lm -static-libgcc")
else()
	if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Haiku")
		list(APPEND WIQ2_LINKER_FLAGS "-lm -rdynamic")
	else()
		list(APPEND WIQ2_LINKER_FLAGS "-lm -lnetwork")
	endif()
endif()

list(APPEND WIQ2_LINKER_FLAGS ${CMAKE_DL_LIBS})

###
# Set CGAME Source Files.
###
set(WIQ2_SRC_CGAME
#    ${WIQ2_SRCDIR_CGAME}/gmenu/menu_id_1.c
    
    ${WIQ2_SRCDIR_CGAME}/menu/m_local.h
    ${WIQ2_SRCDIR_CGAME}/menu/m_main.cpp
    ${WIQ2_SRCDIR_CGAME}/menu/m_mp.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_mp_downloading.cpp
    ${WIQ2_SRCDIR_CGAME}/menu/m_mp_join.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_mp_join_addrbook.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_mp_player.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_mp_start.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_mp_start_dmflags.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_opts.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_opts_controls.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_opts_effects.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_opts_gloom.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_opts_hud.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_opts_input.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_opts_misc.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_opts_screen.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_opts_sound.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_quit.cpp
    ${WIQ2_SRCDIR_CGAME}/menu/m_sp.cpp
    ${WIQ2_SRCDIR_CGAME}/menu/m_sp_credits.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_sp_loadgame.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_sp_savegame.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/m_vid.cpp
    ${WIQ2_SRCDIR_CGAME}/menu/m_vid_exts.cpp
    ${WIQ2_SRCDIR_CGAME}/menu/m_vid_settings.cpp 
    ${WIQ2_SRCDIR_CGAME}/menu/menu.cpp
    ${WIQ2_SRCDIR_CGAME}/menu/menu.h
    
    ${WIQ2_SRCDIR_CGAME}/ui/ui_backend.cpp
    ${WIQ2_SRCDIR_CGAME}/ui/ui_cursor.cpp
    ${WIQ2_SRCDIR_CGAME}/ui/ui_draw.cpp
    ${WIQ2_SRCDIR_CGAME}/ui/ui_items.cpp
    ${WIQ2_SRCDIR_CGAME}/ui/ui_keys.cpp
    ${WIQ2_SRCDIR_CGAME}/ui/ui_local.h
    
    ${WIQ2_SRCDIR_CGAME}/cg_api.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_decals.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_draw.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_entities.cpp
    ${WIQ2_SRCDIR_CGAME}/cg_hud.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_inventory.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_keys.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_light.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_loadscreen.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_localents.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_location.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_main.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_mapeffects.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_media.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_muzzleflash.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_parse.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_parteffects.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_partgloom.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_particles.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_partsustain.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_partthink.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_parttrail.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_players.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_predict.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_screen.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_tempents.cpp
    ${WIQ2_SRCDIR_CGAME}/cg_view.cpp 
    ${WIQ2_SRCDIR_CGAME}/cg_weapon.cpp 
    
    ${WIQ2_SRCDIR_CGAME}/pmove.cpp

    ${WIQ2_SRCDIR_SHARED}/byteswap.cpp
    ${WIQ2_SRCDIR_SHARED}/infostrings.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_angles.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_bounds.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_flash.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_mat3.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_mat4.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_plane.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_quat.cpp 
    ${WIQ2_SRCDIR_SHARED}/mathlib.cpp 
    ${WIQ2_SRCDIR_SHARED}/mersennetwister.cpp
    ${WIQ2_SRCDIR_SHARED}/shared.cpp 
    ${WIQ2_SRCDIR_SHARED}/string.cpp
)

###
# Set GAME Source Files.
###
set(WIQ2_SRC_GAME
    ${WIQ2_SRCDIR_GAME}/g_ai.cpp 
    ${WIQ2_SRCDIR_GAME}/g_chase.cpp
    ${WIQ2_SRCDIR_GAME}/g_cmds.cpp
    ${WIQ2_SRCDIR_GAME}/g_combat.cpp 
    ${WIQ2_SRCDIR_GAME}/g_func.cpp 
    ${WIQ2_SRCDIR_GAME}/g_items.cpp 
    ${WIQ2_SRCDIR_GAME}/g_main.cpp 
    ${WIQ2_SRCDIR_GAME}/g_menus.cpp 
    ${WIQ2_SRCDIR_GAME}/g_misc.cpp 
    ${WIQ2_SRCDIR_GAME}/g_monster.cpp 
    ${WIQ2_SRCDIR_GAME}/g_phys.cpp 
    ${WIQ2_SRCDIR_GAME}/g_save.cpp 
    ${WIQ2_SRCDIR_GAME}/g_spawn.cpp 
    ${WIQ2_SRCDIR_GAME}/g_svcmds.cpp 
    ${WIQ2_SRCDIR_GAME}/g_target.cpp 
    ${WIQ2_SRCDIR_GAME}/g_trigger.cpp 
    ${WIQ2_SRCDIR_GAME}/g_turret.cpp 
    ${WIQ2_SRCDIR_GAME}/g_utils.cpp 
    ${WIQ2_SRCDIR_GAME}/g_weapon.cpp 
    ${WIQ2_SRCDIR_GAME}/m_actor.cpp 
    ${WIQ2_SRCDIR_GAME}/m_berserk.cpp 
    ${WIQ2_SRCDIR_GAME}/m_boss2.cpp 
    ${WIQ2_SRCDIR_GAME}/m_boss3.cpp 
    ${WIQ2_SRCDIR_GAME}/m_boss31.cpp 
    ${WIQ2_SRCDIR_GAME}/m_boss32.cpp 
    ${WIQ2_SRCDIR_GAME}/m_brain.cpp 
    ${WIQ2_SRCDIR_GAME}/m_chick.cpp 
    ${WIQ2_SRCDIR_GAME}/m_flipper.cpp 
    ${WIQ2_SRCDIR_GAME}/m_float.cpp 
    ${WIQ2_SRCDIR_GAME}/m_flyer.cpp 
    ${WIQ2_SRCDIR_GAME}/m_gladiator.cpp 
    ${WIQ2_SRCDIR_GAME}/m_gunner.cpp 
    ${WIQ2_SRCDIR_GAME}/m_hover.cpp 
    ${WIQ2_SRCDIR_GAME}/m_infantry.cpp 
    ${WIQ2_SRCDIR_GAME}/m_insane.cpp 
    ${WIQ2_SRCDIR_GAME}/m_medic.cpp 
    ${WIQ2_SRCDIR_GAME}/m_move.cpp 
    ${WIQ2_SRCDIR_GAME}/m_mutant.cpp 
    ${WIQ2_SRCDIR_GAME}/m_parasite.cpp 
    ${WIQ2_SRCDIR_GAME}/m_soldier.cpp 
    ${WIQ2_SRCDIR_GAME}/m_supertank.cpp 
    ${WIQ2_SRCDIR_GAME}/m_tank.cpp 
    ${WIQ2_SRCDIR_GAME}/p_client.cpp 
    ${WIQ2_SRCDIR_GAME}/p_hud.cpp 
    ${WIQ2_SRCDIR_GAME}/p_trail.cpp 
    ${WIQ2_SRCDIR_GAME}/p_view.cpp 
    ${WIQ2_SRCDIR_GAME}/p_weapon.cpp

    ${WIQ2_SRCDIR_SHARED}/byteswap.cpp
    ${WIQ2_SRCDIR_SHARED}/infostrings.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_angles.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_bounds.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_flash.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_mat3.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_mat4.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_plane.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_quat.cpp 
    ${WIQ2_SRCDIR_SHARED}/mathlib.cpp 
    ${WIQ2_SRCDIR_SHARED}/mersennetwister.cpp
    ${WIQ2_SRCDIR_SHARED}/shared.cpp 
    ${WIQ2_SRCDIR_SHARED}/string.cpp
)

###
# Set ENGINE EXECUTABLE Source Files.
###
set(WIQ2_SRC_ENGINE
    ${WIQ2_SRCDIR_CLIENT}/cl_acapi.cpp
    ${WIQ2_SRCDIR_CLIENT}/cl_cgapi.cpp 
    ${WIQ2_SRCDIR_CLIENT}/cl_cin.cpp 
    ${WIQ2_SRCDIR_CLIENT}/cl_console.cpp 
    ${WIQ2_SRCDIR_CLIENT}/cl_demo.cpp 
    ${WIQ2_SRCDIR_CLIENT}/cl_download.cpp 
    ${WIQ2_SRCDIR_CLIENT}/cl_input.cpp 
    ${WIQ2_SRCDIR_CLIENT}/cl_keys.cpp 
    ${WIQ2_SRCDIR_CLIENT}/cl_main.cpp 
    ${WIQ2_SRCDIR_CLIENT}/cl_parse.cpp 
    ${WIQ2_SRCDIR_CLIENT}/cl_screen.cpp 
    ${WIQ2_SRCDIR_CLIENT}/gui_cursor.cpp 
    ${WIQ2_SRCDIR_CLIENT}/gui_draw.cpp 
    ${WIQ2_SRCDIR_CLIENT}/gui_events.cpp 
    ${WIQ2_SRCDIR_CLIENT}/gui_items.cpp 
    ${WIQ2_SRCDIR_CLIENT}/gui_init.cpp
    ${WIQ2_SRCDIR_CLIENT}/gui_keys.cpp 
    ${WIQ2_SRCDIR_CLIENT}/gui_main.cpp 
    ${WIQ2_SRCDIR_CLIENT}/gui_vars.cpp
    ${WIQ2_SRCDIR_CLIENT}/sdl_main.cpp 
    ${WIQ2_SRCDIR_CLIENT}/sdl_glimp.cpp
    ${WIQ2_SRCDIR_CLIENT}/snd_dma.cpp 
    ${WIQ2_SRCDIR_CLIENT}/snd_main.cpp
    ${WIQ2_SRCDIR_CLIENT}/snd_openal.cpp
    ${WIQ2_SRCDIR_CLIENT}/snd_sdl.cpp

    ${WIQ2_SRCDIR_COMMON}/alias.cpp
    ${WIQ2_SRCDIR_COMMON}/cbuf.cpp 
    ${WIQ2_SRCDIR_COMMON}/cm_common.cpp 
    ${WIQ2_SRCDIR_COMMON}/cm_q2_main.cpp
    ${WIQ2_SRCDIR_COMMON}/cm_q2_trace.cpp
    ${WIQ2_SRCDIR_COMMON}/cm_q3_main.cpp
    ${WIQ2_SRCDIR_COMMON}/cm_q3_trace.cpp 
    ${WIQ2_SRCDIR_COMMON}/cm_q3_patch.cpp 
    ${WIQ2_SRCDIR_COMMON}/cmd.cpp 
    ${WIQ2_SRCDIR_COMMON}/common.cpp 
    ${WIQ2_SRCDIR_COMMON}/crc.cpp 
    ${WIQ2_SRCDIR_COMMON}/cvar.cpp 
    ${WIQ2_SRCDIR_COMMON}/files.cpp 
    ${WIQ2_SRCDIR_COMMON}/md4.cpp 
    ${WIQ2_SRCDIR_COMMON}/memory.cpp 
    ${WIQ2_SRCDIR_COMMON}/net_chan.cpp 
    ${WIQ2_SRCDIR_COMMON}/net_msg.cpp 
    ${WIQ2_SRCDIR_COMMON}/parse.cpp

    ${WIQ2_SRCDIR_RENDERER}/r_math.cpp
    ${WIQ2_SRCDIR_RENDERER}/rb_batch.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rb_entity.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rb_light.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rb_math.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rb_gl.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rb_render.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rb_shadow.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rb_state.cpp 

    ${WIQ2_SRCDIR_RENDERER}/rf_2d.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_alias.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_cull.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_decal.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_font.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_image.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_init.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_light.cpp
    ${WIQ2_SRCDIR_RENDERER}/rf_main.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_material.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_meshbuffer.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_model.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_program.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_sky.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_sprite.cpp 
    ${WIQ2_SRCDIR_RENDERER}/rf_world.cpp

    ${WIQ2_SRCDIR_SERVER}/sv_ccmds.cpp 
    ${WIQ2_SRCDIR_SERVER}/sv_ents.cpp 
    ${WIQ2_SRCDIR_SERVER}/sv_gameapi.cpp 
    ${WIQ2_SRCDIR_SERVER}/sv_init.cpp 
    ${WIQ2_SRCDIR_SERVER}/sv_main.cpp 
    ${WIQ2_SRCDIR_SERVER}/sv_pmove.cpp 
    ${WIQ2_SRCDIR_SERVER}/sv_send.cpp 
    ${WIQ2_SRCDIR_SERVER}/sv_user.cpp 
    ${WIQ2_SRCDIR_SERVER}/sv_world.cpp 

    ${WIQ2_SRCDIR_SHARED}/byteswap.cpp
    ${WIQ2_SRCDIR_SHARED}/infostrings.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_angles.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_bounds.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_flash.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_mat3.cpp
    ${WIQ2_SRCDIR_SHARED}/m_mat4.cpp
    ${WIQ2_SRCDIR_SHARED}/m_plane.cpp 
    ${WIQ2_SRCDIR_SHARED}/m_quat.cpp 
    ${WIQ2_SRCDIR_SHARED}/mathlib.cpp 
    ${WIQ2_SRCDIR_SHARED}/mersennetwister.cpp 
    ${WIQ2_SRCDIR_SHARED}/shared.cpp 
    ${WIQ2_SRCDIR_SHARED}/string.cpp 
)

# Append the proper Operating System specific code to the executable file its sources.
if(UNIX)
    list(APPEND WIQ2_SRC_ENGINE
        ${WIQ2_SRCDIR_UNIX}/unix_console.cpp
        ${WIQ2_SRCDIR_UNIX}/unix_main.cpp 
        ${WIQ2_SRCDIR_UNIX}/unix_snd_cd.cpp 
        ${WIQ2_SRCDIR_UNIX}/unix_udp.cpp
    )
elseif(WIN32)
    list(APPEND WIQ2_SRC_ENGINE 
        #${WIQ2_SRCDIR_WIN32}/win_console.cpp 
        #${WIQ2_SRCDIR_WIN32}/win_main.cpp 
        #${WIQ2_SRCDIR_WIN32}/win_snd_cd.cpp 
        #${WIQ2_SRCDIR_WIN32}/win_sock.cpp 
    )
endif()

# Build the cgame(client game) dynamic library
if (${BUILD_WIQ2_CGAME})
	add_library(cgame_${ARCH} MODULE ${WIQ2_SRC_CGAME})
	set_target_properties(cgame_${ARCH} PROPERTIES
			PREFIX ""
			LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/baseq2
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/baseq2
			SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX}
			)
	target_link_libraries(cgame_${ARCH} ${WIQ2_LINKER_FLAGS})
endif()

# Build the game dynamic library
if (${BUILD_WIQ2_GAME})
    set(tname game)
    set(tname ${tname}_${ARCH})
	add_library(game_${ARCH} MODULE ${WIQ2_SRC_GAME})
	set_target_properties(game_${ARCH} PROPERTIES
			PREFIX ""
			LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/baseq2
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/baseq2
			SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX}
			)
	target_link_libraries(game_${ARCH} ${WIQ2_LINKER_FLAGS})
endif()

# Build the WIQ2 executable
if(${BUILD_WIQ2})
    add_executable(WIQ2_${ARCH} ${WIQ2_SRC_ENGINE})
    set_target_properties(WIQ2_${ARCH} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
            )
    if(UNIX)
        target_link_libraries(WIQ2_${ARCH}
            ${WIQ2_LINKER_FLAGS} 
            ${SDL_LIBRARY} 
            ${OPENAL_LIBRARY} 
            ${PNG_LIBRARIES} 
            ${JPEG_LIBRARIES} 
            ${ZLIB_LIBRARIES} 
            ${UNZIP_LIBRARIES} 
            ${X11_LIBRARIES}
        )# ws2_32 winmm)
    elseif(WIN32)
        target_link_libraries(WIQ2_${ARCH}
            ${WIQ2_LINKER_FLAGS} 
            ${SDL_LIBRARY} 
            ${OPENAL_LIBRARY} 
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

