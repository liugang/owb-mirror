if(USE_GRAPHICS STREQUAL "GTK")
    pkg_check_modules(CAIRO REQUIRED cairo>=1.4)
    pkg_check_modules(GTK REQUIRED gtk+-2.0>=2.8)
    set(GRAPHICS_INCLUDE_DIRS ${GTK_INCLUDE_DIRS})
    set(GRAPHICS_LIBRARIES ${GTK_LDFLAGS})

    set(USE_GRAPHICS_GTK TRUE)
    mark_as_advanced(USE_GRAPHICS_GTK)
endif(USE_GRAPHICS STREQUAL "GTK")

if(USE_GRAPHICS STREQUAL "SDL")
    pkg_check_modules(SDL REQUIRED sdl>=1.2.10)
    include(FindSDL_gfx)
    set(GRAPHICS_INCLUDE_DIRS ${SDL_INCLUDE_DIRS} ${SDLGFX_INCLUDE_DIR})
    set(GRAPHICS_LIBRARIES ${SDL_LDFLAGS} ${SDLGFX_LIBRARY})

    set(USE_GRAPHICS_SDL TRUE)
    mark_as_advanced(USE_GRAPHICS_SDL)
endif(USE_GRAPHICS STREQUAL "SDL")