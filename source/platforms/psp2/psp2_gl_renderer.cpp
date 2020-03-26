//
// Created by cpasjuste on 21/11/16.
//

#ifdef __PSP2_GL__

#include "cross2d/c2d.h"

#define GLM_FORCE_PURE

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace c2d;

PSP2Renderer::PSP2Renderer(const Vector2f &s) : Renderer(s) {

    printf("PSP2Renderer(GL)\n");

    vglInit(0x800000);
    vglWaitVblankStart(GL_TRUE);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    // init shaders
    shaderList = (ShaderList *) new GLShaderList();

    available = true;

    printf("PSP2Renderer(GL)(%p): %ix%i\n", this, (int) s.x, (int) s.y);
}

void PSP2Renderer::draw(VertexArray *vertexArray, const Transform &transform, Texture *texture, Sprite *sprite) {

    Vertex *vertices;
    size_t vertexCount;
    GLTexture *glTexture;
    GLShader *shader;
    Vector2f inputSize, textureSize, outputSize;

    if (vertexArray == nullptr || vertexArray->getVertexCount() < 1) {
        //printf("gl_render::draw: no vertices\n");
        return;
    }

    vertices = vertexArray->getVertices()->data();
    vertexCount = vertexArray->getVertexCount();
    glTexture = sprite != nullptr ? (GLTexture *) sprite->getTexture() : (GLTexture *) texture;
    shader = glTexture != nullptr && glTexture->available ? (GLShader *) shaderList->get(0)->data :
             (GLShader *) ((GLShaderList *) shaderList)->color->data;
    if (glTexture != nullptr && glTexture->shader != nullptr) {
        shader = (GLShader *) glTexture->shader->data;
    }

    // set shader
    glUseProgram(shader->GetProgram());

    // bind vbo
    vertexArray->bind();

    // set vertex position
    /*
    glEnableVertexAttribArray(0));
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                                   (void *) offsetof(Vertex, position));

    // set vertex colors
    glEnableVertexAttribArray(1));
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
                                   (void *) offsetof(Vertex, color));
    */

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), (void *) offsetof(Vertex, position));

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (void *) offsetof(Vertex, color));

    if (glTexture != nullptr && glTexture->available) {
        // bind texture
        glBindTexture(GL_TEXTURE_2D, glTexture->texID);
        // set tex coords
        /*
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              (void *) offsetof(Vertex, texCoords));
        */
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void *) offsetof(Vertex, texCoords));

        // set retroarch shader params
        if (sprite != nullptr) {
            inputSize = {sprite->getTextureRect().width, sprite->getTextureRect().height};
            textureSize = {sprite->getTexture()->getSize().x, sprite->getTexture()->getSize().y};
            outputSize = {inputSize.x * sprite->getScale().x, inputSize.y * sprite->getScale().y};
        } else {
            inputSize = {texture->getTextureRect().width, texture->getTextureRect().height};
            textureSize = {texture->getSize().x, texture->getSize().y};
            outputSize = {inputSize.x * texture->getScale().x, inputSize.y * texture->getScale().y};
        }
        shader->SetUniform("InputSize", inputSize);
        shader->SetUniform("TextureSize", textureSize);
        shader->SetUniform("OutputSize", outputSize);

    }

    // projection
    auto projectionMatrix = glm::orthoLH(0.0f, (float) C2D_SCREEN_WIDTH, (float) C2D_SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
    // view
    auto viewMatrix = glm::make_mat4(transform.getMatrix());
    // mpv
    auto mpvMatrix = projectionMatrix * viewMatrix;
    // set mpv matrix uniform
    shader->SetUniformMatrix("MVPMatrix", glm::value_ptr(mpvMatrix));

    // enable blending if needed
    if (glTexture != nullptr || vertices[0].color.a < 255) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // draw
    const GLenum modes[] = {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES,
                            GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_QUADS};
    GLenum mode = modes[vertexArray->getPrimitiveType()];
    glDrawArrays(mode, 0, (GLsizei) vertexCount);

    //glDisableVertexAttribArray(0);
    //glDisableVertexAttribArray(1);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    if (glTexture != nullptr || vertices[0].color.a < 255) {
        glDisable(GL_BLEND);
        if (glTexture != nullptr && glTexture->available) {
            glBindTexture(GL_TEXTURE_2D, 0);
            //glDisableVertexAttribArray(2);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
    }

    // unbind object vbo
    vertexArray->unbind();

    glUseProgram(0);
}

void PSP2Renderer::flip(bool draw, bool inputs) {

    vglStartRendering();

    // call base class (draw childs)
    Renderer::flip(draw, inputs);

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
