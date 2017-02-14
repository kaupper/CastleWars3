///@author Kaupper

#include "Texture.h"
#include <iostream>
#include <sstream>


static int texturecount = 0;


std::map<std::string, std::shared_ptr<TextureWrapper>> Texture::imageCache;
std::map<std::string, std::map<Color, std::map<short, std::shared_ptr<TextureWrapper>>>>
Texture::textCache;

std::shared_ptr<TextureWrapper> Texture::getCached(const std::string &s)
{
    return imageCache[s];
}

std::shared_ptr<TextureWrapper> Texture::getCached(const std::string &s,
        const Color &c, short size)
{
    return textCache[s][c][size];
}

bool Texture::hasTextureCached(const std::string &s)
{
    if (imageCache.find(s) == imageCache.end()) {
        return false;
    }

    return true;
}

bool Texture::hasTextureCached(const std::string &s, const Color &c, short size)
{
    if (textCache.find(s) == textCache.end()) {
        return false;
    }

    if (textCache[s].find(c) == textCache[s].end()) {
        return false;
    }

    if (textCache[s][c].find(size) == textCache[s][c].end()) {
        return false;
    }

    return true;
}

Texture::Texture() : texture(NULL)
{
    flip = false;
}

Texture::~Texture()
{
    if (texture) {
        Logger::LogDebug("Texture::~Texture");
    }
}

int Texture::loadText(const std::string &text, const Color &color,
                      short size)
{
    if (hasTextureCached(text, color, size)) {
        auto tex = getCached(text, color, size);
        int w, h;
        texture = tex;
        MANGLE_SDL(SDL_QueryTexture)(tex->get(), NULL, NULL, &w, &h);
        Texture::clip = Texture::size = { 0, 0, w, h };
        return 0;
    }

    TTF_Font *font = NULL;

    if (size < 0) {
        font = Core::getInstance()->getFont(FontSize::SMALL);
    } else if (size == 0) {
        font = Core::getInstance()->getFont(FontSize::MEDIUM);
    } else if (size > 0) {
        font = Core::getInstance()->getFont(FontSize::LARGE);
    }

    SDL_Surface *textSurface = MANGLE_SDL(TTF_RenderText_Solid)(font, text.c_str(), {color.r, color.g, color.b, color.a });

    if (textSurface == NULL) {
        return -1;
    }

    SDL_Texture *nTexture = NULL;
    nTexture = MANGLE_SDL(SDL_CreateTextureFromSurface)
               (Core::getInstance()->getRenderer(),
                textSurface);

    if (nTexture == NULL) {
        return -2;
    }

    Logger::LogDebug("Texture::Texture");
    MANGLE_SDL(SDL_FreeSurface)(textSurface);
    texture = std::make_shared<TextureWrapper>(nTexture);
    textCache[text][color][size] = texture;
    int w, h;
    MANGLE_SDL(SDL_QueryTexture)(texture->get(), NULL, NULL, &w, &h);
    Texture::clip = Texture::size = { 0, 0, w, h };
    return 0;
}

int Texture::loadImage(const std::string &path)
{
    if (hasTextureCached(path)) {
        auto tex = getCached(path);
        int w, h;
        texture = tex;
        MANGLE_SDL(SDL_QueryTexture)(tex->get(), NULL, NULL, &w, &h);
        Texture::clip = Texture::size = { 0, 0, w, h };
        return 0;
    }

    SDL_Texture *nTexture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());

    if (loadedSurface == NULL) {
        return -1;
    }

    nTexture = MANGLE_SDL(SDL_CreateTextureFromSurface)
               (Core::getInstance()->getRenderer(),
                loadedSurface);

    if (nTexture == NULL) {
        return -2;
    }

    Logger::LogDebug("Texture::Texture");
    clip = { 0, 0, loadedSurface->w, loadedSurface->h };
    size = clip;
    MANGLE_SDL(SDL_FreeSurface)(loadedSurface);
    texture = imageCache[path] = std::make_shared<TextureWrapper>(nTexture);
    return 0;
}

int Texture::loadImage(const std::string &path, const SDL_Rect &clip)
{
    int status = loadImage(path);
    Texture::clip = clip;
    Texture::size = clip;
    return status;
}

int Texture::loadImage(const std::string &path, const SDL_Rect &clip,
                       const SDL_Rect &size)
{
    int status = loadImage(path);
    Texture::clip = clip;
    Texture::size = size;
    return status;
}

SDL_Rect Texture::getSize()
{
    return size;
}

void Texture::setSize(int w, int h)
{
    if (w != 0) {
        size.w = w;
    }

    if (h != 0) {
        size.h = h;
    }
}

void Texture::setSize(const SDL_Rect &rect)
{
    size = rect;
}

int Texture::onRender(const Location &loc, bool flip)
{
    SDL_Renderer *renderer = Core::getInstance()->getRenderer();
    SDL_Rect rect { (int) loc.x, (int) loc.y, (int) size.w, (int) size.h };
    SDL_Point point { getSize().x / 2, 0 };
    int ret;

    if (flip) {
        ret = MANGLE_SDL(SDL_RenderCopyEx)(renderer, texture->get(), &clip, &rect, 0,
                                           &point,
                                           SDL_FLIP_HORIZONTAL);
    } else {
        ret =  MANGLE_SDL(SDL_RenderCopy)(renderer, texture->get(), &clip, &rect);
    }

    return ret;
}