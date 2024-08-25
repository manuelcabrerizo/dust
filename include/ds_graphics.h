//
// Created by manue on 8/23/2024.
//

#ifndef DS_GRAPHICS_H
#define DS_GRAPHICS_H

#define DUST_API __declspec(dllexport)

#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "ds_containers.h"

namespace ds {

    class DUST_API Shader {
    public:
        Shader() = default;
        Shader(const char *vert, const char *frag);
        ~Shader();

        void Initialize(const char *vert, const char *frag);
        void Shutdown() const;

        void Bind() const;
        uint32_t Get() const;

    private:
        uint32_t program;
    };

    class DUST_API UniformBuffer {
    public:
        UniformBuffer(const Shader& shader, const char *uboName);
        ~UniformBuffer();

        void *Map();
        void Unmap();


    private:
        uint32_t index;
        uint32_t ubo;
        ds::TempArray<char> buffer;
    };


    struct DUST_API BufferLayout {
        uint32_t index;
        int32_t elementCount;
        int32_t stride;
        size_t offset;
    };

    class DUST_API Buffer {
    public:
        Buffer();
        Buffer(void *data, size_t size);
        ~Buffer();

        void Initialize(void *data, size_t size);
        void SetLayouts(BufferLayout *layouts, uint32_t count) const;
        void Bind() const;
    private:
        uint32_t vao;
        uint32_t vbo;
    };

    class DUST_API Texture {
    public:
        Texture();
        Texture(const char *filename);
        void Initialize(void *data, int width, int height, int channels);

        void Bind();

        ~Texture();
    private:
        uint32_t texture;
        int32_t width;
        int32_t height;
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uvs;
    };

    class DUST_API Mesh {
    public:
        Mesh() = default;
        ~Mesh() = default;

        Vertex *Vertices();
        int Size();
        int ByteSize();

        void LoafObjFile(const char *filename);
    private:
        TempArray<Vertex> vertices;
    };




}


#endif //DS_GRAPHICS_H
