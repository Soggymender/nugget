#include "Texture.h"

#include <stdio.h>

#include "glad/glad.h"

#include "stb/stb_img.h"
#include <iostream>

GLuint LoadTextureFromFile(const char* filename) {
    // Load the image using stb_image
    int width, height, channels;

    stbi_set_flip_vertically_on_load(false);
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (!image) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return 0;  // Return 0 if the texture loading failed
    }

    // Generate an OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload the image to the GPU as a texture
    if (channels == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    else if (channels == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    }
    else {
        std::cerr << "Unsupported image format" << std::endl;
        stbi_image_free(image);
        return 0;  // Return 0 if the image format is unsupported
    }

    // Generate mipmaps for the texture
    glGenerateMipmap(GL_TEXTURE_2D);

    // Free the image data as it's no longer needed
    stbi_image_free(image);

    return textureID;
}