//
// Created by manue on 8/23/2024.
//
#include <GL/gl3w.h>
#include <ds_graphics.h>
#include <fstream>
#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static std::vector<char> ReadBinaryFile(const char *filename) {
    std::vector<char> buffer;

    std::ifstream file;
    file.open(filename, std::ios::binary|std::ios::ate);
    if(file.is_open()) {
        size_t size = file.tellg();
        buffer.resize(size);
        file.seekg(0);
        file.read(buffer.data(), buffer.size());
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
    std::vector<char> vertBinary = ReadBinaryFile(vert);
    std::vector<char> fragBinary = ReadBinaryFile(frag);

    uint32_t vertShader = glCreateShader(GL_VERTEX_SHADER);
    uint32_t fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    char log[512];
    int success;
    glShaderBinary(1, &vertShader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, vertBinary.data(), vertBinary.size());
    glSpecializeShader(vertShader, "main", 0, nullptr, nullptr);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertShader, 512, nullptr, log);
        std::cout << log << std::endl;
    }

    glShaderBinary(1, &fragShader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, fragBinary.data(), fragBinary.size());
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

    buffer.resize(uboSize);

    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glNamedBufferStorage(ubo, uboSize, buffer.data(), GL_MAP_WRITE_BIT);
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

ds::Texture::Texture(std::string filename)
    : texture(0), width(0), height(0) {
    Initialize(filename);
}

ds::Texture::~Texture() {
    if(glIsTexture(texture)) {
        glDeleteTextures(1, &texture);
    }
}
void ds::Texture::Initialize(std::string filename) {
    int width, height, channels;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
    if(!data) {
        std::cout << "ERROR: loading png falied " << filename << "\n";
        throw std::runtime_error("error loading texture file");
    }
    Initialize(data, width, height, channels);
    stbi_image_free(data);
}


void ds::Texture::Initialize(void *data, int width_, int height_, int channels) {
    width = width_;
    height = height_;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
        format, GL_UNSIGNED_BYTE, data);

    // TODO: see if this is working ...
    glGenerateMipmap(GL_TEXTURE_2D);

}

void ds::Texture::Bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}

//////////////////////////////////////////////////////////////////////////////////
/// Mesh class
//////////////////////////////////////////////////////////////////////////////////
size_t ds::Mesh::SubmeshesCount() {
    return submeshes.size();
}

std::vector<ds::Submesh>&  ds::Mesh::GetSubmeshes() {
    return submeshes;
}

std::unordered_map<std::string, ds::Material>& ds::Mesh::GetMaterials() {
    return materials;
}

void ds::Mesh::LoafObjFile(std::string filename) {
    std::ifstream file;
    file.open(filename,  std::ios::in);
    if(file.is_open()) {

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;

        int submeshesCount = 0;

        std::string line;
        while(getline(file, line)) {
            if(line.find("mtllib ", 0, 7) == 0) {
                std::string name = line.substr(7, line.length() - 7);

                std::string path;
                size_t lastSlash = filename.rfind('/');
                if(lastSlash != std::string::npos) {
                    path = filename.substr(0, lastSlash + 1);
                }

                std::string materialFilePath = path + name;
                LoadMtlFile(materialFilePath);
            }
            else if(line.find("v ", 0, 2) == 0) {
                glm::vec3 position;
                sscanf_s(line.c_str(), "v %f %f %f", &position.x, &position.y, &position.z);
                positions.push_back(position);
            }
            else if(line.find("vn", 0, 2) == 0) {
                glm::vec3 normal;
                sscanf_s(line.c_str(), "vn %f %f %f", &normal.x, &normal.y, &normal.z);
                normals.push_back(normal);
            }
            else if(line.find("vt", 0, 2) == 0) {
                glm::vec2 uv;
                float pad;
                sscanf_s(line.c_str(), "vt %f %f %f", &uv.x, &uv.y, &pad);
                uvs.push_back(uv);
            }
            else if(line.find("o ", 0, 2) == 0) {
                submeshesCount++;
            }
            else if(line.find("# object ", 0, 9) == 0) {
                submeshesCount++;
            }
        }

        file.clear();
        file.seekg(0);

        submeshes.resize(submeshesCount);
        int submeshIndex = -1;

        while(getline(file, line)) {
            if(line.find("o ", 0, 2) == 0) {
                submeshIndex++;
            }
            else if(line.find("# object ", 0, 9) == 0) {
                submeshIndex++;
            }
            else if(line.find("usemtl ", 0, 7) == 0) {
                assert(submeshIndex >= 0);
                std::string mtlName =  line.substr(7, line.length() - 7);
                submeshes[submeshIndex].mtlNames.push_back(mtlName);
                submeshes[submeshIndex].mtlOffsets.push_back(submeshes[submeshIndex].vertices.size());
            }
            else if(line.find("f ", 0, 2) == 0) {
                int posIndex[3];
                int norIndex[3];
                int uvsIndex[3];
                sscanf_s(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
                    &posIndex[0], &uvsIndex[0], &norIndex[0],
                    &posIndex[1], &uvsIndex[1], &norIndex[1],
                    &posIndex[2], &uvsIndex[2], &norIndex[2]);

                Vertex vertex {};
                for(int i = 0 ; i < 3; i++) {
                    vertex.position = positions[posIndex[i] - 1];
                    vertex.normal = normals[norIndex[i] - 1];
                    vertex.uvs = uvs[uvsIndex[i] - 1];
                    assert(submeshIndex >= 0);
                    submeshes[submeshIndex].vertices.push_back(vertex);
                }

            }
        }

        file.close();
    }
    else {
        throw std::runtime_error("error opening the obj file");
    }
}

// TODO: asset manager so we dont have repeated texture for materials ...
void ds::Mesh::LoadMtlFile(std::string filename) {

    std::string path;
    size_t lastSlash = filename.rfind('/');
    if(lastSlash != std::string::npos) {
        path = filename.substr(0, lastSlash + 1);
    }

    std::ifstream file;
    file.open(filename,  std::ios::in);
    if(file.is_open()) {

        std::string line;
        getline(file, line);
        while(file.peek() != EOF) {
            if(line.find("newmtl ", 0, 7) == 0) {
                std::string name = line.substr(7, line.length() - 7);
                Material &mtl = materials[name];

                getline(file, line);
                while((line.compare(0, 7, "newmtl ") != 0) && (file.peek() != EOF)) {

                    if(line.find("Ns ", 0, 3) == 0) {
                        sscanf_s(line.c_str(), "Ns %f", &mtl.Ns);
                    }
                    else if(line.find("Ka ", 0, 3) == 0) {
                        sscanf_s(line.c_str(), "Ka %f %f %f", &mtl.Ka.x, &mtl.Ka.y, &mtl.Ka.z);
                    }
                    else if(line.find("Kd ", 0, 3) == 0) {
                        sscanf_s(line.c_str(), "Kd %f %f %f", &mtl.Kd.x, &mtl.Kd.y, &mtl.Kd.z);
                    }
                    else if(line.find("Ks ", 0, 3) == 0) {
                        sscanf_s(line.c_str(), "Ks %f %f %f", &mtl.Ks.x, &mtl.Ks.y, &mtl.Ks.z);
                    }
                    else if(line.find("Ni ", 0, 3) == 0) {
                        sscanf_s(line.c_str(), "Ni %f", &mtl.Ni);
                    }
                    else if(line.find("d ", 0, 2) == 0) {
                        sscanf_s(line.c_str(), "d %f", &mtl.d);
                    }
                    else if(line.find("illum ", 0, 6) == 0) {
                        sscanf_s(line.c_str(), "illum %d", &mtl.illumType);
                    }
                    else if(line.find("map_Kd ", 0, 7) == 0) {
                        mtl.map_Kd = std::make_unique<Texture>(path + line.substr(7, line.length() - 7));
                    }
                    else if(line.find("map_d ", 0, 6) == 0) {
                        //mtl.map_d = std::make_unique<Texture>(path + line.substr(6, line.length() - 6));
                    }
                    else if(line.find("map_Disp ", 0, 9) == 0) {
                        mtl.map_Disp = std::make_unique<Texture>(path + line.substr(9, line.length() - 9));
                    }
                    else if(line.find("map_Ka ", 0, 7) == 0) {
                        mtl.map_Ka = std::make_unique<Texture>(path + line.substr(7, line.length() - 7));
                    }
                    getline(file, line);
                }
            }
            else {
                getline(file, line);
            }
        }
    }
}







