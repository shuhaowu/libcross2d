//
// Created by cpasjuste on 21/11/16.
//

#ifdef __PSP2_GL__

#include "cross2d/c2d.h"

using namespace c2d;

PSP2Renderer::PSP2Renderer(const Vector2f &s) : GLRenderer(s) {

    printf("PSP2Renderer(GL)\n");

    vglInit(0x800000);
    vglWaitVblankStart(GL_TRUE);

    initGL();

    available = true;

    printf("PSP2Renderer(GL)(%p): %ix%i\n", this, (int) s.x, (int) s.y);
}

void PSP2Renderer::flip(bool draw, bool inputs) {

    vglStartRendering();

    // call base class (draw childs)
    GLRenderer::flip(draw, inputs);

    vglStopRendering();
}

void PSP2Renderer::delay(unsigned int ms) {

    SDL_Delay(ms);
}

PSP2Renderer::~PSP2Renderer() {

    printf("~PSP2Renderer(GL)\n");
    vglEnd();
}

#endif // __PSP2_GL__
