//
// Created by manue on 8/23/2024.
//

#ifndef DS_GRAPHICS_H
#define DS_GRAPHICS_H

#define DUST_API __declspec(dllexport)

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

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
        std::vector<char> buffer;
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
        Texture(std::string filename);

        void Initialize(std::string filename);
        void Initialize(void *data, int width, int height, int channels);

        void Bind();

        ~Texture();
    private:
        uint32_t texture;
        int32_t width;
        int32_t height;
    };

    class Vertex {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uvs;
    };

    class Material {
    public:
        float Ns;
        glm::vec3 Ka;
        glm::vec3 Kd;
        glm::vec3 Ks;
        float Ni;
        float d;
        int illumType;

        std::unique_ptr<Texture> map_Kd;
        std::unique_ptr<Texture> map_d;
        std::unique_ptr<Texture> map_Disp;
        std::unique_ptr<Texture> map_Ka;

    };

    class Submesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<std::string> mtlNames;
        std::vector<uint32_t> mtlOffsets;
    };

    class DUST_API Mesh {
    public:
        Mesh() = default;
        ~Mesh() = default;

        size_t SubmeshesCount();
        std::vector<Submesh>& GetSubmeshes();
        std::unordered_map<std::string, Material>& GetMaterials();
        void LoafObjFile(std::string filename);
        void LoadMtlFile(std::string filename);
    private:
        std::vector<Submesh> submeshes;
        std::unordered_map<std::string, Material> materials;
    };
}


#endif //DS_GRAPHICS_H
