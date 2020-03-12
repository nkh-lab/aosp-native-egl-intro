#include <iostream>
#include <thread>
#include <chrono>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <utils/StrongPointer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>

using namespace std::chrono_literals;

uint32_t gWidth  = 1000;
uint32_t gHeight = 500;
android::sp<android::SurfaceComposerClient> gSurfaceComposerClient;
android::sp<android::SurfaceControl>        gSurfaceControl;

/*
 * Next codes mostly withaout changes from Khronos intro
 */

/* Already defined in frameworks/native/opengl/include/EGL/eglplatform.h like
 * typedef struct ANativeWindow*           EGLNativeWindowType;
 * ...
 * typedef EGLNativeWindowType  NativeWindowType;
 */
// typedef ... NativeWindowType;

extern NativeWindowType createNativeWindow(void);
static EGLint const attribute_list[] = {
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_NONE
};

int main(int argc, char *argv[])
{
    std::cout << "Hello from EGL Intro!" << std::endl;
    std::cout << "Yellow surface should be shown!" << std::endl;

    if (argc >= 3)
    {
        gWidth = std::stoi(argv[1]);
        gHeight = std::stoi(argv[2]);
    }

    /***/

    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    NativeWindowType native_window;
    EGLint num_config;

    /* get an EGL display connection */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    /* initialize the EGL display connection */
    eglInitialize(display, NULL, NULL);

    /* get an appropriate EGL frame buffer configuration */
    eglChooseConfig(display, attribute_list, &config, 1, &num_config);

    /* create an EGL rendering context */
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);

    /* create a native window */
    native_window = createNativeWindow();

    /* create an EGL window surface */
    surface = eglCreateWindowSurface(display, config, native_window, NULL);

    /* connect the context to the surface */
    eglMakeCurrent(display, surface, surface, context);

    /* clear the color buffer */
    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    eglSwapBuffers(display, surface);

    /***/

    for(;;)
    {
        std::this_thread::sleep_for(1s);
    }

    return EXIT_SUCCESS;
}

NativeWindowType createNativeWindow()
{
    NativeWindowType ret = nullptr;

    gSurfaceComposerClient = android::sp<android::SurfaceComposerClient> {new android::SurfaceComposerClient()};
    gSurfaceControl = gSurfaceComposerClient->createSurface(android::String8("egl-intro"),
                                                            gWidth,
                                                            gHeight,
                                                            0); // set format to 0 because it is specified by attribute_list like RGBA-format

    if(!gSurfaceControl)
        std::cout << "!gSurfaceControl" << std::endl;
    else if(!gSurfaceControl->isValid())
        std::cout << "!gSurfaceControl->isValid()" << std::endl;
    else
    {
        android::SurfaceComposerClient::Transaction{}
                .setLayer(gSurfaceControl, INT_MAX)
                .show(gSurfaceControl)
                .apply();

        ret = gSurfaceControl->getSurface().get();
    }

    return ret;
}
