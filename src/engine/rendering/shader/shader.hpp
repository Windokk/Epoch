#pragma once

#include "engine/rendering/utils.hpp"

#include <string>

namespace SHAME::Engine::Rendering {

    std::string GetFileContent(const char* filename);

    struct UniformInfo {
        std::string name;
        GLenum type;
        GLint location;
    };

    class Shader{
        
        const char* vertexFile;
        const char* fragmentFile;
        
        public:
        // Reference ID of the Shader Program
        GLuint ID;
        // Constructor that build the Shader Program from 2 different shaders
        Shader(const char* vertexFile = "", const char* fragmentFile = "");

        std::vector<UniformInfo> GetActiveUniforms();

        // Activates the Shader Program (bind)
        void Activate();
        // Deactivates the Shader Program (unbind)
        void Deactivate();
        // Deletes the Shader Program
        void Cleanup();

        void setBool(const std::string& name, bool value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }
        // ------------------------------------------------------------------------
        void setInt(const std::string& name, int value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        void setFloat(const std::string& name, float value) const
        {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }
        // ------------------------------------------------------------------------
        void setVec2(const std::string& name, const glm::vec2& value) const
        {
            glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec2(const std::string& name, float x, float y) const
        {
            glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
        }
        // ------------------------------------------------------------------------
        void setVec3(const std::string& name, const glm::vec3& value) const
        {
            glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec3(const std::string& name, float x, float y, float z) const
        {
            glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
        }
        // ------------------------------------------------------------------------
        void setVec4(const std::string& name, const glm::vec4& value) const
        {
            glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
        }
        void setVec4(const std::string& name, float x, float y, float z, float w) const
        {
            glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
        }
        // ------------------------------------------------------------------------
        void setMat2(const std::string& name, const glm::mat2& mat) const
        {
            glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat3(const std::string& name, const glm::mat3& mat) const
        {
            glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        // ------------------------------------------------------------------------
        void setMat4(const std::string& name, const glm::mat4& mat) const
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
        }
        private:
        // Checks if the different Shaders have compiled properly
        void CompileErrors(unsigned int shader, const char* type);
    };

}