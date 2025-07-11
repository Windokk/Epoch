#include "shader.hpp"

#include <stdexcept>
#include <iostream>


namespace SHAME::Engine::Rendering {

    using namespace Filesystem;

    bool IsSamplerType(GLenum type) {
        return type == GL_SAMPLER_1D || type == GL_SAMPLER_2D ||
               type == GL_SAMPLER_3D || type == GL_SAMPLER_CUBE ||
               type == GL_SAMPLER_1D_SHADOW || type == GL_SAMPLER_2D_SHADOW ||
               type == GL_SAMPLER_1D_ARRAY || type == GL_SAMPLER_2D_ARRAY ||
               type == GL_SAMPLER_1D_ARRAY_SHADOW || type == GL_SAMPLER_2D_ARRAY_SHADOW ||
               type == GL_SAMPLER_2D_MULTISAMPLE || type == GL_SAMPLER_2D_MULTISAMPLE_ARRAY ||
               type == GL_SAMPLER_CUBE_SHADOW || type == GL_SAMPLER_BUFFER ||
               type == GL_SAMPLER_2D_RECT || type == GL_SAMPLER_2D_RECT_SHADOW;
    }

    /// @brief Constructor that build a Shader Program from 2 (vert and frag) or 3 (vert, frag and geom) different shaders path
    Shader::Shader(const Path vertexFilePath, const Path fragmentFilePath, const Path geometryFilePath)
    {
        if (vertexFilePath.full != "" && fragmentFilePath.full != "") {
		
            this->vertexFilePath = vertexFilePath.full;
            this->fragmentFilePath = fragmentFilePath.full;

            // Read vertexFile and fragmentFile and store the strings
            std::string vertexCode = vertexFilePath.ReadFile();
            std::string fragmentCode = fragmentFilePath.ReadFile();
    
            // Convert the shader source strings into character arrays
            const char* vertexSource = vertexCode.c_str();
            const char* fragmentSource = fragmentCode.c_str();
    
            // Create Vertex Shader Object and get its reference
            GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
            // Attach Vertex Shader source to the Vertex Shader Object
            glShaderSource(vertexShader, 1, &vertexSource, NULL);
            // Compile the Vertex Shader into machine code
            glCompileShader(vertexShader);
            // Checks if Shader compiled succesfully
            CompileErrors(vertexShader, "VERTEX");
    
            // Create Fragment Shader Object and get its reference
            GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
            // Attach Fragment Shader source to the Fragment Shader Object
            glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
            // Compile the Vertex Shader into machine code
            glCompileShader(fragmentShader);
            // Checks if Shader compiled succesfully
            CompileErrors(fragmentShader, "FRAGMENT");
    
            GLuint geometryShader = 0;
            bool hasGeometry = !geometryFilePath.full.empty();

            if (hasGeometry) {
                this->geometryFilePath = geometryFilePath.full;
                std::string geometryCode = geometryFilePath.ReadFile();
                const char* geometrySource = geometryCode.c_str();

                geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(geometryShader, 1, &geometrySource, NULL);
                glCompileShader(geometryShader);
                CompileErrors(geometryShader, "GEOMETRY");
            }

            ID = glCreateProgram();
            glAttachShader(ID, vertexShader);
            glAttachShader(ID, fragmentShader);
            if (hasGeometry)
                glAttachShader(ID, geometryShader);

            glLinkProgram(ID);
            CompileErrors(ID, "PROGRAM");

            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            if (hasGeometry)
                glDeleteShader(geometryShader);
        }
    }

    /// @brief Getter for the uniforms names and types of the shader
    /// @return A std::vector of uniforms
    std::vector<UniformInfo> Shader::GetActiveUniforms() {
        std::vector<UniformInfo> uniforms;
    
        GLint uniformCount;
        glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &uniformCount);
    
        GLint maxNameLength = 0;
        glGetProgramiv(ID, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);
    
        std::vector<char> nameData(maxNameLength);
    
        for (GLint i = 0; i < uniformCount; ++i) {
            GLsizei length = 0;
            GLint size = 0;
            GLenum type = 0;
    
            glGetActiveUniform(ID, i, maxNameLength, &length, &size, &type, nameData.data());
            std::string name(nameData.data(), length);
    
            GLint location = glGetUniformLocation(ID, name.c_str());
    
            if (name.find("gl_") == 0) continue;
    
            uniforms.push_back({ name, type, location });
        }
    
        return uniforms;
    }

    /// @brief Activates the Shader Program (bind)
    void Shader::Activate()
    {
        glUseProgram(ID);
    }

    /// @brief Deactivates the Shader Program (unbind)
    void Shader::Deactivate()
    {
        glUseProgram(0);
    }

    /// @brief Deletes the Shader Program
    void Shader::Cleanup()
    {
        glDeleteProgram(ID);
    }

    /// @brief Checks if the different Shaders have compiled properly
    void Shader::CompileErrors(unsigned int shader, const char* type)
    {
        // Stores status of compilation
        GLint hasCompiled;
        // Character array to store error message in
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
            if (hasCompiled == GL_FALSE)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                throw std::runtime_error("[ERROR] [RENDERING/SHADER] Couldn't compile shader : " + std::string(type) + "\n" + infoLog);
                return;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
            if (hasCompiled == GL_FALSE)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                throw std::runtime_error("[ERROR] [RENDERING/SHADER] Couldn't link shader : " + std::string(type) + "\n" + infoLog);
                return;
            }
        }
    }
}


