//
// Created by morten on 18/05/14.
//


#pragma once
#include "kick/core/kickgl.h"
#include "kick/mesh/mesh_data.h"
#include <string>

namespace kick {
    enum class ShaderType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER,
        GeometryShader = GL_GEOMETRY_SHADER
    };

    enum class ShaderErrorType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER,
        GeometryShader = GL_GEOMETRY_SHADER,
        Linker,
        IncompleteShader
    };

    enum class BlendType {
        Zero = GL_ZERO,
        One = GL_ONE,
        SrcColor = GL_SRC_COLOR,
        OneMinusSrcColor = GL_ONE_MINUS_SRC_COLOR,
        DstColor = GL_DST_COLOR,
        OneMinusDstColor = GL_ONE_MINUS_DST_COLOR,
        SrcAlpha = GL_SRC_ALPHA,
        OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
        DstAlpha = GL_DST_ALPHA,
        OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
        ConstantColor = GL_CONSTANT_COLOR,
        OneMinusConstantColor = GL_ONE_MINUS_CONSTANT_COLOR,
        ConstantAlpha = GL_CONSTANT_ALPHA,
        OneMinusConstantAlpha = GL_ONE_MINUS_CONSTANT_ALPHA
    };

    enum class FaceCullingType {
        Front = GL_FRONT,
        Back = GL_BACK,
        FrontAndBack = GL_FRONT_AND_BACK,
        None = GL_NONE
    };

    enum class ZTestType {
        Never = GL_NEVER,
        Less = GL_LESS,
        Equal = GL_EQUAL,
        LEqual = GL_LEQUAL,
        Greater = GL_GREATER,
        NotEqual = GL_NOTEQUAL,
        GEqual = GL_GEQUAL,
        Always = GL_ALWAYS
    };

    bool toEnum(int value, ShaderType& enumValue);
    bool toEnum(int value, ShaderErrorType& enumValue);
    bool toEnum(int value, BlendType& enumValue);
    bool toEnum(int value, FaceCullingType& enumValue);
    bool toEnum(int value, ZTestType& enumValue);
    bool toEnum(int value, FaceCullingType& enumValue);

    class ShaderObj {
    public:
        ShaderObj() {}
        explicit ShaderObj(ShaderType type);
        ~ShaderObj();
        ShaderObj(ShaderObj&&);
        ShaderObj(const ShaderObj&) = delete;
        ShaderObj& operator=(ShaderObj&& other);
        void detach(GLuint shaderProgram);
        operator GLuint() { return shader; }
    private:
        GLuint shader = 0;
    };


    struct ShaderBuildException {
        std::string errorMessage;
        ShaderErrorType errorType;
        std::string codeSnippet;
        static void throwCurrentCompileException(GLuint shader, ShaderErrorType type, std::string source);
        static std::string extractLines(std::string errorMessage, std::string source, int extraLines = 2);
    };

    struct AttributeDescriptor  {
        GLuint index;
        GLint size;
        GLenum type;
        std::string name;
        VertexAttributeSemantic semantic;
    };

    struct UniformDescriptor  {
        GLuint index;
        GLint size;
        GLenum type;
        std::string name;
    };

    namespace UniformNames {
        const std::string modelMatrix{"_modelMatrix"};
        const std::string mv{"_mv"};
        const std::string norm{"_norm"};
        const std::string v{"_v"};
        const std::string worldCamPos{"_worldCamPos"};
        const std::string world2object{"_world2object"};
        const std::string mvProj{"_mvProj"};
        const std::string proj{"_proj"};
        const std::string gameObjectUID{"_gameObjectUID"};
        const std::string shadowMapTexture{"_shadowMapTexture"};
        const std::string lightMat{"_lightMat"};
        const std::string ambient{"_ambient"};
        const std::string pointLight{"_pLights[0]"};
        const std::string directionalLight{"_dLight"};
        const std::string directionalLightWorld{"_dLightWorldDir"};
        const std::string time{"_time"};
        const std::string viewport{"_viewport"};

        const static std::string list[] = {
                modelMatrix,
                mv,
                norm,
                v,
                worldCamPos,
                world2object,
                mvProj,
                proj,
                gameObjectUID,
                shadowMapTexture,
                lightMat,
                ambient,
                pointLight,
                directionalLight,
                directionalLightWorld,
                time,
                viewport
        };
    };

    std::string to_string(AttributeDescriptor attDesc);

    std::string to_string(UniformDescriptor uniformDesc);
}


