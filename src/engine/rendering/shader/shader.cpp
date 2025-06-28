#include "shader.hpp"

#include <stdexcept>
#include <iostream>

#include "engine/filesystem/filesystem.hpp"

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

    Shader::Shader(const char* vertexFile, const char* fragmentFile)
    {

        if (vertexFile != "" && fragmentFile != "") {
		
            this->vertexFile = vertexFile;
            this->fragmentFile = fragmentFile;

            // Read vertexFile and fragmentFile and store the strings
            std::string vertexCode = FileManager::ReadFile(Path(vertexFile));
            std::string fragmentCode = FileManager::ReadFile(Path(fragmentFile));
    
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
    
            // Create Shader Program Object and get its reference
            ID = glCreateProgram();
            // Attach the Vertex and Fragment Shaders to the Shader Program
            glAttachShader(ID, vertexShader);
            glAttachShader(ID, fragmentShader);
            // Wrap-up/Link all the shaders together into the Shader Program
            glLinkProgram(ID);
            // Checks if Shaders linked succesfully
            CompileErrors(ID, "PROGRAM");
    
            // Delete the now useless Vertex and Fragment Shader objects
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        }
    }

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

    void Shader::Cleanup()
    {
        glDeleteProgram(ID);
    }

    void Shader::Activate()
    {
        glUseProgram(ID);
    }

    void Shader::Deactivate()
    {
        glUseProgram(0);
    }

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


