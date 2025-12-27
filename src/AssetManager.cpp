#include "../include/AssetManager.h"
#include <iostream>

void AssetManager::LoadTexture(const std::string& name, const std::string& fileName) {
    if (!_textures[name].loadFromFile(fileName)) {
        std::cout << "HATA: Resim yuklenemedi: " << fileName << std::endl;
    }
}
sf::Texture& AssetManager::GetTexture(const std::string& name) {
    return _textures.at(name);
}

void AssetManager::LoadFont(const std::string& name, const std::string& fileName) {
    if (!_fonts[name].loadFromFile(fileName)) {
        std::cout << "HATA: Font yuklenemedi: " << fileName << std::endl;
    }
}
sf::Font& AssetManager::GetFont(const std::string& name) {
    return _fonts.at(name);
}

void AssetManager::LoadSoundBuffer(const std::string& name, const std::string& fileName) {
    if (!_soundBuffers[name].loadFromFile(fileName)) {
        std::cout << "HATA: Ses yuklenemedi: " << fileName << std::endl;
    }
}
sf::SoundBuffer& AssetManager::GetSoundBuffer(const std::string& name) {
    return _soundBuffers.at(name);
}
