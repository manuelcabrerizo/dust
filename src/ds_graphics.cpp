//
// Created by manue on 8/23/2024.
//
#include <GL/gl3w.h>
#include <ds_graphics.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static ds::TempArray<char> ReadBinaryFile(const char *filename) {
    ds::TempArray<char> buffer;

    std::ifstream file;
    file.open(filename, std::ios::binary|std::ios::ate);
    if(file.is_open()) {
        size_t size = file.tellg();
        buffer.Initialize(size);
        file.seekg(0);
        file.read(buffer.Ptr(), buffer.Size());
        file.close();
    }

    return buffer; // ??? see if this is necesary
}

//////////////////////////////////////////////////////////////////////////////////
/// Shader class
//////////////////////////////////////////////////////////////////////////////////

ds::Shader::Shader(const char *vert, const char *frag)
    :program(0) {
    Initialize(vert, frag);
}

ds::Shader::~Shader() {
    Shutdown();
}

void ds::Shader::Initialize(const char *vert, const char *frag) {
    ds::TempArray<char> vertBinary = ReadBinaryFile(vert);
    ds::TempArray<char> fragBinary = ReadBinaryFile(frag);

    uint32_t vertShader = glCreateShader(GL_VERTEX_SHADER);
    uint32_t fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    char log[512];
    int success;
    glShaderBinary(1, &vertShader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, vertBinary.Ptr(), vertBinary.Size());
    glSpecializeShader(vertShader, "main", 0, nullptr, nullptr);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertShader, 512, nullptr, log);
        std::cout << log << std::endl;
    }

    glShaderBinary(1, &fragShader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, fragBinary.Ptr(), fragBinary.Size());
    glSpecializeShader(fragShader, "main", 0, nullptr, nullptr);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragShader, 512, nullptr, log);
        std::cout << log << std::endl;
    }

    program = glCreateProgram();

    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cout << log << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

void ds::Shader::Shutdown() const {
    if(glIsProgram(program)) {
        glDeleteProgram(program);
    }
}

void ds::Shader::Bind() const {
    glUseProgram(program);
}

uint32_t ds::Shader:: Get() const {
    return program;
}

//////////////////////////////////////////////////////////////////////////////////
/// Uniform Buffer class
//////////////////////////////////////////////////////////////////////////////////
ds::UniformBuffer::UniformBuffer(const Shader& shader, const char *uboName) {
    shader.Bind();
    index = glGetUniformBlockIndex(shader.Get(), uboName);

    GLint uboSize;
    glGetActiveUniformBlockiv(shader.Get(), index, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

    buffer.Initialize(uboSize);

    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, uboSize, buffer.Ptr(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo);
}

ds::UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &ubo);
}

void *ds::UniformBuffer::Map() {
    return glMapNamedBuffer(ubo, GL_WRITE_ONLY);
}

void ds::UniformBuffer::Unmap() {
    glUnmapNamedBuffer(ubo);
}

//////////////////////////////////////////////////////////////////////////////////
/// Buffer class
//////////////////////////////////////////////////////////////////////////////////

ds::Buffer::Buffer()
    : vao(0), vbo(0) {
}

ds::Buffer::Buffer(void *data, size_t size)
    : vao(0), vbo(0) {
    Initialize(data, size);
}

ds::Buffer::~Buffer() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void ds::Buffer::Initialize(void *data, size_t size) {
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, size, data, 0);
}

void ds::Buffer::SetLayouts(BufferLayout *layouts, uint32_t count) const {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    for(uint32_t i = 0; i < count; i++) {
        const ds::BufferLayout * layout = layouts + i;
        glVertexAttribPointer(layout->index, layout->elementCount,
                            GL_FLOAT, GL_FALSE, layout->stride,
                            reinterpret_cast<void *>(layout->offset));
        glEnableVertexAttribArray(i);
    }
}

void ds::Buffer::Bind() const {
    glBindVertexArray(vao);
}

//////////////////////////////////////////////////////////////////////////////////
/// Texture class
//////////////////////////////////////////////////////////////////////////////////

ds::Texture::Texture()
: texture(0), width(0), height(0) {

}

ds::Texture::Texture(const char *filename)
    : texture(0), width(0), height(0) {
    int width, height, channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 4);
    if(!data) {
        std::cout << "ERROR: loading png %s falied " << filename;
        throw std::runtime_error("error loading texture file");
    }
    Initialize(data, width, height, channels);
    stbi_image_free(data);
}

ds::Texture::~Texture() {
    if(glIsTexture(texture)) {
        glDeleteTextures(1, &texture);
    }
}

void ds::Texture::Initialize(void *data, int width_, int height_, int channels) {
    width = width_;
    height = height_;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned int format;
    switch(channels) {
        case 3: {
            format = GL_RGB;
        } break;
        case 4: {
            format = GL_RGBA;
        } break;
        default: {
            throw std::runtime_error("error image format not supported");
        } break;
    }

    glTexImage2D(
            GL_TEXTURE_2D, // target
            0, // mip level
            GL_RGBA, // internal format, often advisable to use BGR
            width, // width of the texture
            height, // height of the texture
            0, // border (always 0)
            format, // format
            GL_UNSIGNED_BYTE, // type
            data // Data to upload
    );

    glGenerateMipmap(GL_TEXTURE_2D);

}

void ds::Texture::Bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}

//////////////////////////////////////////////////////////////////////////////////
/// Mesh class
//////////////////////////////////////////////////////////////////////////////////

ds::Vertex *ds::Mesh::Vertices() {
    return vertices.Ptr();
}
int ds::Mesh::Size() {
    return vertices.Size();
}

int ds::Mesh::ByteSize() {
    return vertices.ByteSize();
}

void ds::Mesh::LoafObjFile(const char *filename) {
    std::ifstream file;
    file.open(filename,  std::ios::in);
    if(file.is_open()) {

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        int faceCount = 0;

        std::string line;
        while(getline(file, line)) {
            if(line.find("v ", 0, 2) == 0) {
                glm::vec3 position;
                sscanf_s(line.c_str(), "v %f %f %f", &position.x, &position.y, &position.z);
                positions.push_back(position);
            }
            if(line.find("vn", 0, 2) == 0) {
                glm::vec3 normal;
                sscanf_s(line.c_str(), "vn %f %f %f", &normal.x, &normal.y, &normal.z);
                normals.push_back(normal);
            }
            if(line.find("vt", 0, 2) == 0) {
                glm::vec2 uv;
                float pad;
                sscanf_s(line.c_str(), "vt %f %f %f", &uv.x, &uv.y, &pad);
                uvs.push_back(uv);
            }
            if(line.find("f ", 0, 2) == 0) {
                faceCount++;
            }
        }

        vertices.Initialize(faceCount * 3);

        file.clear();
        file.seekg(0);

        Vertex *vertex = vertices.Ptr();
        while(getline(file, line)) {
            if(line.find("f ", 0, 2) == 0) {

                int posIndex[3];
                int norIndex[3];
                int uvsIndex[3];
                sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &posIndex[0], &uvsIndex[0], &norIndex[0],
                    &posIndex[1], &uvsIndex[1], &norIndex[1],
                    &posIndex[2], &uvsIndex[2], &norIndex[2]);

                for(int i = 0 ; i < 3; i++) {
                    vertex[i].position = positions[posIndex[i] - 1];
                    vertex[i].normal = normals[norIndex[i] - 1];
                    vertex[i].uvs = uvs[uvsIndex[i] - 1];
                }

                vertex += 3;
            }
        }

        file.close();
    }
    else {
        throw std::runtime_error("error opening the obj file");
    }
}






