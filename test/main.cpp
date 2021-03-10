//
// Created by cpasjuste on 08/12/16.
//

#include "cross2d/c2d.h"

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace c2d;

int main(int argc, char *argv[]) {

    // create main renderer
    auto *renderer = new C2DRenderer(Vector2f(C2D_SCREEN_WIDTH, C2D_SCREEN_HEIGHT));
    renderer->setPrintStats(true);
    renderer->setClearColor(Color::Black);

    int fd = open((renderer->getIo()->getRomFsPath() + "comicate.ttf").c_str(), O_RDONLY);
    struct stat st{};
    if (fstat(fd, &st) != 0) {
        printf("fstat failed...\n");
    } else {
        printf("file size: %li\n", st.st_size);
    }
    close(fd);

#if 0
    auto *border = new C2DRectangle({2, 2,
                                     renderer->getSize().x - 4, renderer->getSize().y - 4});
    border->setFillColor(Color::Transparent);
    border->setOutlineColor(Color::Red);
    border->setOutlineThickness(2);
    renderer->add(border);

    // scale font and texture for different resolution (devices),
    // based on 720p resolution
    float scaling = renderer->getSize().y / 720.0f;

    // create a rect
    auto *rect = new C2DRectangle(
            {renderer->getSize().x / 2, renderer->getSize().y / 2,
             renderer->getSize().x / 2, renderer->getSize().y / 2});

    rect->setOrigin(Origin::Center);
    rect->setFillColor(Color::Gray);
    rect->setOutlineColor(Color::Orange);
    rect->setOutlineThickness(8 * scaling);

    // create a texture and add it to the rect
    C2DTexture *tex = nullptr;
    if (renderer->getIo()->exist(renderer->getIo()->getRomFsPath() + "gbatemp.png")) {
        tex = new C2DTexture(renderer->getIo()->getRomFsPath() + "gbatemp.png");
    } else {
        tex = new C2DTexture(renderer->getIo()->getDataPath() + "gbatemp.png");
    }
    if (tex->available) {
        tex->setPosition(rect->getSize().x / 2, rect->getSize().y / 2);
        tex->setScale(0.5f * scaling, 0.5f * scaling);
        tex->setOrigin(Origin::Center);
        rect->add(tex);
    }

    // create a text
    auto *font = new Font();
    font->loadFromFile(renderer->getIo()->getRomFsPath() + "comicate.ttf");
    auto *text = new Text("libcross2d @ Cpasjuste", C2D_DEFAULT_CHAR_SIZE, font);
    text->setOutlineThickness(2);
    text->setPosition(rect->getSize().x - 16 * scaling, rect->getSize().y - 16 * scaling);
    text->setOrigin(Origin::BottomRight);
    text->setScale(scaling, scaling);
    rect->add(text);

    // add all this crap to the renderer
    renderer->add(rect);

    // add some tweening :)
    auto *tweenPos = new TweenPosition(
            {renderer->getSize().x / 2 - (256 * scaling), rect->getPosition().y},
            {renderer->getSize().x / 2 + (256 * scaling), rect->getPosition().y},
            4.0f, TweenLoop::PingPong);
    tweenPos->play();
    rect->add(tweenPos);
    auto *tweenRot = new TweenRotation(0, 360, 5.0f, TweenLoop::PingPong);
    tweenRot->play();
    rect->add(tweenRot);
    auto *tweenScale = new TweenScale(rect->getScale(), {2.0f, 2.0f}, 3.0f, TweenLoop::PingPong);
    tweenScale->play();
    rect->add(tweenScale);
    auto *tweenColor = new TweenColor(rect->getFillColor(), Color(0x9bc4e2ff), 3.0f, TweenLoop::PingPong);
    tweenColor->play();
    rect->add(tweenColor);
    auto *tweenAlpha = new TweenAlpha(255, 200, 3.0f, TweenLoop::PingPong);
    tweenAlpha->play();
    rect->add(tweenAlpha);

    while (true) {

        // stop if any key is pressed
        if (renderer->getInput()->getKeys() != 0) {
            break;
        }

        // renderer everything
        renderer->flip();
    }

    // will delete child's (textures, shapes, text..)
    delete (renderer);
#endif
    return 0;
}
