#include "common.h"
#include "platform/platform.h"
#include "ref_api.h"
#include "ref_common.h"
#include "vid_common.h"
#include "xash3d_types.h"
#include <EGL/egl.h>
#include <whb/proc.h>

#define STUB Con_Printf("TODO: %s, (%s:%d)\n", __func__, __FILE_NAME__, __LINE__);

static EGLDisplay egl_display;
static EGLSurface egl_surface;
static EGLContext egl_context;

qboolean R_Init_Video( ref_graphic_apis_t type )
{
    if (type == REF_SOFTWARE) return false;

    WHBProcInit();

    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_display == EGL_NO_DISPLAY) {
        Con_Printf("Can't get egl display\n");
        return false;
    }

    if (eglInitialize(egl_display, NULL, NULL) != EGL_TRUE) {
        Con_Printf("Can't initialize egl\n");
        return false;
    }

    static const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_ALPHA_SIZE,      8,
        EGL_STENCIL_SIZE,      8,
        EGL_DEPTH_SIZE,      24,
        EGL_NONE
    };

    static const EGLint context_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 3,
        EGL_CONTEXT_MINOR_VERSION, 0,
        EGL_NONE
    };

    EGLConfig egl_conf;
    EGLint egl_conf_cnt;
    if (eglChooseConfig(egl_display, config_attribs, &egl_conf, 1, &egl_conf_cnt) != EGL_TRUE) {
        Con_Printf("Failed to choose egl config\n");
        return false;
    }

    if (egl_conf_cnt == 0) {
        Con_Printf("No matching egl configs\n");
        return false;  
    }

    static const EGLint window_surface_attribs[] = {
        EGL_WIDTH, 1280,
        EGL_HEIGHT, 720,
        EGL_NONE
    };

    egl_surface = eglCreateWindowSurface(egl_display, egl_conf, (EGLNativeWindowType)NULL, window_surface_attribs);
    if (egl_surface == EGL_NO_SURFACE) {
        Con_Printf("Can't create window surface\n");
        return false;
    }

    // TODO angle doesn't like desktop yet i guess
    // at least the commit I'm using
    if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE) {
        Con_Printf("Can't bind API\n");
        return false;  
    }

    egl_context = eglCreateContext(egl_display, egl_conf, EGL_NO_CONTEXT, context_attribs);
    if (egl_context == EGL_NO_CONTEXT) {
        Con_Printf("Can't create egl context\n");
        return false;
    }

    if (eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context) != EGL_TRUE) {
        Con_Printf("Made current failed\n");
    }

    // we should be able to more or less implement this
    ref.dllFuncs.GL_SetupAttributes( glw_state.safe );
    
    VID_SetMode();
	ref.dllFuncs.GL_InitExtensions();
    
    host.renderinfo_changed = false;

    return true;
}

void R_Free_Video()
{
    STUB
    WHBProcShutdown();
}

int GL_SetAttribute( int attr, int val )
{
    STUB
    return -1;
}

int GL_GetAttribute( int attr, int *val )
{
    switch (attr){
        case REF_GL_RED_SIZE:
        case REF_GL_BLUE_SIZE:
        case REF_GL_GREEN_SIZE:
        case REF_GL_ALPHA_SIZE:
        case REF_GL_STENCIL_SIZE:
            *val = 8;
            break;
        case REF_GL_DEPTH_SIZE:
            *val = 24;
            break;
        default:
        *val = 0;
    }
	return 0;
}

int R_MaxVideoModes( void )
{
    STUB
	return 0;
}

vidmode_t* R_GetVideoMode( int num )
{
    STUB
	return NULL;
}

void* GL_GetProcAddress( const char *name ) // RenderAPI requirement
{
	Con_Printf("GL_GetProcAddress: %s\n", name);
    void* addr = eglGetProcAddress(name);
    Con_Printf("addr = 0x%X\n", addr);
    return addr;
}

void GL_SwapBuffers( void )
{
    eglSwapBuffers(egl_display, egl_surface);
}

void *SW_LockBuffer( void )
{
    STUB
}
void SW_UnlockBuffer( void )
{
    STUB
}

qboolean SW_CreateBuffer( int width, int height, uint *stride, uint *bpp, uint *r, uint *g, uint *b )
{
    STUB
    return false;
}

void GL_UpdateSwapInterval( void )
{
    if( FBitSet( gl_vsync.flags, FCVAR_CHANGED ))
	{
		ClearBits( gl_vsync.flags, FCVAR_CHANGED );

		if( eglSwapInterval( egl_display, gl_vsync.value ) < 0 )
			Con_Reportf( S_ERROR  "eglSwapInterval: %d\n", eglGetError( ));
	}
}

rserr_t   R_ChangeDisplaySettings( int width, int height, window_mode_t window_mode ){
    STUB
    return rserr_ok;
}

ref_window_type_t R_GetWindowHandle( void **handle, ref_window_type_t type )
{
    STUB
    return REF_WINDOW_TYPE_NULL;
}

qboolean  VID_SetMode( void )
{
    int render_w = 1280, render_h = 720;
	VID_SetDisplayTransform( &render_w, &render_h );
    R_SaveVideoMode(1280, 720, render_w, render_h, true);
    return true;
}

void VID_Info_f( void )
{
    Con_Printf("Wii U EGL GLES3 thingy i hope\n");
}

void Platform_Minimize_f( void )
{
    STUB
}

void VID_RestoreScreenResolution( window_mode_t window_mode )
{
    STUB
}

void VID_SaveWindowSize( int width, int height )
{
    STUB
}