//
// Created by cpasjuste on 21/11/16.
//

#ifdef __PSP2_GL__

#ifndef PSP2_GL_RENDERER_H
#define PSP2_GL_RENDERER_H

namespace c2d {

    class PSP2Renderer : public Renderer {

    public:

        explicit PSP2Renderer(const Vector2f &size = Vector2f(0, 0));

        ~PSP2Renderer() override;

        void draw(VertexArray *vertexArray, const Transform &transform, Texture *texture, Sprite *sprite) override;

        void flip(bool draw = true, bool inputs = true) override;

        void delay(unsigned int ms) override;
    };
}

#endif // PSP2_GL_RENDERER_H
#endif // __PSP2_GL__
