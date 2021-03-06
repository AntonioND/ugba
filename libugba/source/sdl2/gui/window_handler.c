// SPDX-License-Identifier: LGPL-3.0-only
//
// Copyright (c) 2011-2015, 2019-2020 Antonio Niño Díaz

// This is here for conveniency during development. It should be set from the
// CMake build options.
//#define OPENGL_BLIT

#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#ifdef OPENGL_BLIT
#include <SDL_opengl.h>
#endif

#include "window_handler.h"

#include "../debug_utils.h"

#define MAX_WINDOWS 10

typedef struct
{
    // Window data
    SDL_Window *mWindow;
    SDL_Renderer *mRenderer;
    SDL_GLContext GLContext;
    SDL_Texture *mTexture;
    int mWindowID;

    WH_CallbackFn mEventCallback;

    // Window dimensions
    int mWidth;
    int mHeight;
    int mTexWidth;
    int mTexHeight;
    int mTexScale; // If 0, the texture will be scaled to window size. If not,
                   // it will be centered and scaled to this factor.

    // Window focus
    int mMouseFocus;
    int mKeyboardFocus;
    int mShown;
} window_handle_t;

// Windows
static window_handle_t gWindows[MAX_WINDOWS];

// All unhandled events will be sent to this window
static window_handle_t *gMainWindow = NULL;

static window_handle_t *wh_get_from_index(int index)
{
    if ((index >= MAX_WINDOWS) || (index < 0))
        return NULL;

    return &(gWindows[index]);
}

static window_handle_t *wh_get_from_windowID(int id)
{
    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        if (gWindows[i].mWindowID == id)
            return &(gWindows[i]);
    }

    return NULL;
}

void WH_Init(void)
{
    memset(gWindows, 0, sizeof(gWindows));
}

//------------------------------------------------------------------------------

// Returns -1 on error
int WH_Create(int width, int height, int texw, int texh, int scale)
{
    window_handle_t *w;
    int window_handle_tIndex = -1;

    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        if (gWindows[i].mWindow == NULL)
        {
            w = &(gWindows[i]);
            window_handle_tIndex = i;
            break;
        }
    }
    if (window_handle_tIndex == -1)
        return -1;

    if (texw == 0)
        texw = width;
    if (texh == 0)
        texh = height;

    // Initialize non-existant window
    w->mWindow = NULL;
    w->mRenderer = NULL;
    w->mEventCallback = NULL;
    w->mMouseFocus = 0;
    w->mKeyboardFocus = 0;
    w->mShown = 0;
    w->mWindowID = -1;
    w->mTexScale = scale;

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);

    // Create window
    w->mWindow = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED, width, height,
                                  SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (w->mWindow == NULL)
    {
        Debug_Log("Window could not be created! SDL Error: %s\n",
                  SDL_GetError());
        return -1;
    }

    w->mMouseFocus = 1;
    w->mKeyboardFocus = 1;
    w->mWidth = width;
    w->mHeight = height;
    w->mTexWidth = texw;
    w->mTexHeight = texh;
    w->GLContext = SDL_GL_CreateContext(w->mWindow);

    int oglIdx = -1;
    int nRD = SDL_GetNumRenderDrivers();

    for (int i = 0; i < nRD; i++)
    {
        SDL_RendererInfo info;
        if (!SDL_GetRenderDriverInfo(i, &info))
        {
            if (!strcmp(info.name, "opengl"))
            {
                oglIdx = i;
                break;
            }
        }
    }

    // Create renderer for window
    w->mRenderer = SDL_CreateRenderer(w->mWindow, oglIdx,
                                   // SDL_RENDERER_PRESENTVSYNC |
                                      SDL_RENDERER_ACCELERATED);
    //SDL_SetWindowFullscreen(w->mWindow, SDL_WINDOW_FULLSCREEN);
    if (w->mRenderer == NULL)
    {
        Debug_Log("Renderer could not be created! SDL Error: %s\n",
                  SDL_GetError());
        SDL_DestroyWindow(w->mWindow);
        w->mWindow = NULL;

        return -1;
    }

    // SDL_RenderSetLogicalSize(w->mRenderer, w->mWidth, w->mHeight);

    // Grab window identifier
    w->mWindowID = SDL_GetWindowID(w->mWindow);
    w->mShown = 1; // Flag as opened

    w->mTexture = SDL_CreateTexture(w->mRenderer, SDL_PIXELFORMAT_RGB24,
                                    SDL_TEXTUREACCESS_STREAMING, texw, texh);
    if (w->mTexture == NULL)
    {
        // TODO: This message shows even if everything is correct...
        Debug_Log("Couldn't create texture! SDL Error: %s\n",
                  SDL_GetError());
        SDL_DestroyWindow(w->mWindow);
        SDL_GL_DeleteContext(w->GLContext);
        w->mWindow = NULL;

        return -1;
    }

    return window_handle_tIndex;
}

void WH_SetSize(int index, int width, int height, int texw, int texh, int scale)
{
    window_handle_t *w = wh_get_from_index(index);

    if (texw == 0)
        texw = width;
    if (texh == 0)
        texh = height;

    w->mTexScale = scale;

    if (!((w->mWidth == width) && (w->mHeight == height)))
    {
        SDL_SetWindowSize(w->mWindow, width, height);
        w->mWidth = width;
        w->mHeight = height;
    }

    if (!((w->mTexWidth == texw) && (w->mTexHeight == texh)))
    {
        w->mTexWidth = texw;
        w->mTexHeight = texh;
        SDL_DestroyTexture(w->mTexture);
        w->mTexture = SDL_CreateTexture(w->mRenderer, SDL_PIXELFORMAT_RGB24,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        texw, texh);
        if (w->mTexture == NULL)
        {
            Debug_Log("Couldn't create texture! SDL Error: %s\n",
                      SDL_GetError());
        }
    }
}

static void wh_free_from_handle(window_handle_t *w)
{
    if (w == gMainWindow)
        gMainWindow = NULL;

    if (w->mWindow != NULL)
    {
        SDL_DestroyTexture(w->mTexture);
        SDL_GL_DeleteContext(w->GLContext);
        SDL_DestroyWindow(w->mWindow);
    }

    w->mWindow = NULL;
    w->mWindowID = -1;

    w->mMouseFocus = 0;
    w->mKeyboardFocus = 0;
    w->mShown = 0;
    w->mWidth = 0;
    w->mHeight = 0;
}

int WH_SetEventCallback(int index, WH_CallbackFn fn)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return 0;

    w->mEventCallback = fn;
    return 1;
}

int WH_SetEventMainWindow(int index)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return 0;

    gMainWindow = w;
    return 1;
}

// Returns 1 if handled, 0 if not (it has been sent to main window)
static int wh_handle_event(SDL_Event *e)
{
    if (e->type == SDL_QUIT)
    {
        for (int i = 0; i < MAX_WINDOWS; i++)
            WH_Close(i);
        return 1;
    }

    // All the events in the first switch are specific for a window. If the
    // events are not handled, ignore them. Always return 1 from this function.
    //
    // The events in the second switch are global.

    // If an event was detected for this window
    if (e->type == SDL_WINDOWEVENT)
    {
        int windowID = e->window.windowID;
        window_handle_t *w = wh_get_from_windowID(windowID);
        if (w == NULL)
            return 1;

        switch (e->window.event)
        {
            // Window appeared
            case SDL_WINDOWEVENT_SHOWN:
                w->mShown = 1;
                break;

            // Window disappeared
            case SDL_WINDOWEVENT_HIDDEN:
                w->mShown = 0;
                break;

            // Get new dimensions and repaint
            //case SDL_WINDOWEVENT_SIZE_CHANGED:
            //    w->mWidth = e->window.data1;
            //    w->mHeight = e->window.data2;
            //    SDL_RenderPresent(w->mRenderer);
            //    break;

            // Repaint on expose
            case SDL_WINDOWEVENT_EXPOSED:
                SDL_RenderPresent(w->mRenderer);
                break;

            // Mouse enter
            case SDL_WINDOWEVENT_ENTER:
                w->mMouseFocus = 1;
                break;

            // Mouse exit
            case SDL_WINDOWEVENT_LEAVE:
                w->mMouseFocus = 0;
                break;

            // Keyboard focus gained
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                w->mKeyboardFocus = 1;
                break;

            // Keyboard focus lost
            case SDL_WINDOWEVENT_FOCUS_LOST:
                w->mKeyboardFocus = 0;
                break;

            // Hide on close
            case SDL_WINDOWEVENT_CLOSE:
                if (w->mWindow)
                {
                    if (w->mEventCallback)
                        w->mEventCallback(e);
                }
                wh_free_from_handle(w);
                //SDL_HideWindow(w->mWindow);
                return 1;

            default:
                break;
        }
    }

    int windowID = 0;
    switch (e->type)
    {
        case SDL_WINDOWEVENT:
            windowID = e->window.windowID;
            break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            windowID = e->key.windowID;
            break;
        case SDL_TEXTEDITING:
            windowID = e->edit.windowID;
            break;
        case SDL_TEXTINPUT:
            windowID = e->text.windowID;
            break;
        case SDL_MOUSEMOTION:
            windowID = e->motion.windowID;
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            windowID = e->button.windowID;
            break;
        case SDL_MOUSEWHEEL:
            windowID = e->wheel.windowID;
            break;
        default:
            // This event is not window-specific. Send it to the main window
            // handler.
            windowID = -1;
            return 0;
    }

    window_handle_t *w = wh_get_from_windowID(windowID);
    if (w == NULL)
        return 1;

    if (w->mWindow)
    {
        if (w->mEventCallback)
            w->mEventCallback(e);
    }

    return 1;
}

void WH_HandleEvents(void)
{
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        // Handle window events
        if (wh_handle_event(&e) == 0)
        {
            if (gMainWindow)
            {
                if (gMainWindow->mEventCallback)
                    gMainWindow->mEventCallback(&e);
            }
        }
    }
}

void WH_SetCaption(int index, const char *caption)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return;

    SDL_SetWindowTitle(w->mWindow, caption);
}

void WH_Render(int index, const unsigned char *buffer)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return;
    if (w->mWindow == NULL)
        return;

    SDL_UpdateTexture(w->mTexture, NULL, (const void *)buffer,
                      w->mTexWidth * 3);

#ifdef OPENGL_BLIT
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glClearColor(0, 0, 0, 0);
    glViewport(0, 0, w->mWidth, w->mHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0, w->mWidth, w->mHeight, 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    glClear(GL_COLOR_BUFFER_BIT); // Clear screen

    SDL_GL_BindTexture(w->mTexture, NULL, NULL); // Returns 0 if OK

    if (w->mTexScale)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_QUADS);

        glTexCoord2i(0, 0); // Top-left vertex
        glVertex3f(0, 0, 0);

        glTexCoord2i(w->mTexWidth, 0); // Bottom-left vertex
        glVertex3f(w->mWidth, 0, 0);

        glTexCoord2i(w->mTexWidth, w->mTexHeight); // Bottom-right vertex
        glVertex3f(w->mWidth, w->mHeight, 0);

        glTexCoord2i(0, w->mTexHeight); // Top-right vertex
        glVertex3f(0, w->mHeight, 0);

        glEnd();
    }
    else
    {
        // TODO: Render window with autoscale
    }

#else

    SDL_RenderClear(w->mRenderer);

    if (w->mTexScale == 0)
    {
        SDL_RenderCopy(w->mRenderer, w->mTexture, NULL, NULL);
    }
    else
    {
        int x_size = w->mTexWidth * w->mTexScale;
        int y_size = w->mTexHeight * w->mTexScale;
        int x_offset = (w->mWidth - x_size) / 2;
        int y_offset = (w->mHeight - y_size) / 2;

        if ((x_offset < 0) || (y_offset < 0))
        {
            Debug_Log("%s(): Invalid scaling. Using default.", __func__);
            w->mTexScale = 0;
            return;
        }

        SDL_Rect src;
        src.x = 0;
        src.y = 0;
        src.w = w->mTexWidth;
        src.h = w->mTexHeight;

        SDL_Rect dst;
        dst.x = x_offset;
        dst.y = y_offset;
        dst.w = x_size;
        dst.h = y_size;

        SDL_RenderCopy(w->mRenderer, w->mTexture, &src, &dst);
    }

#endif // OPENGL_BLIT

    SDL_RenderPresent(w->mRenderer);
}

int WH_AreAllWindowsClosed(void)
{
    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        if (gWindows[i].mWindow)
            return 0;
    }

    return 1;
}

void WH_Close(int index)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return;

    SDL_Event e;
    e.type = SDL_WINDOWEVENT;
    e.window.event = SDL_WINDOWEVENT_CLOSE;
    e.window.windowID = w->mWindowID;

    SDL_PushEvent(&e);
}

void WH_CloseAll(void)
{
    for (int i = 0; i < MAX_WINDOWS; i++)
        WH_Close(i);
}

void WH_CloseAllBut(int index)
{
    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        if (i != index)
            WH_Close(i);
    }
}

void WH_CloseAllButMain(void)
{
    for (int i = 0; i < MAX_WINDOWS; i++)
    {
        if ((&(gWindows[i])) != gMainWindow)
            WH_Close(i);
    }
}

void WH_Focus(int index)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return;

    // Restore window if needed
    if (!w->mShown)
        SDL_ShowWindow(w->mWindow);

    // Move window forward
    SDL_RaiseWindow(w->mWindow);
}

int WH_GetWidth(int index)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return 0;

    return w->mWidth;
}

int WH_GetHeight(int index)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return 0;

    return w->mHeight;
}

int WH_HasMouseFocus(int index)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return 0;

    return w->mMouseFocus;
}

int WH_HasKeyboardFocus(int index)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return 0;

    return w->mKeyboardFocus;
}

int WH_IsShown(int index)
{
    window_handle_t *w = wh_get_from_index(index);

    if (w == NULL)
        return 0;

    return w->mShown;
}
