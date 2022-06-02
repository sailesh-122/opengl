#pragma once
#include <OGL3D/OPrerequisities.h>
#include <OGL3D/Math/OVec4.h> 
#include <OGL3D/Math/ORect.h> 
    

class OGraphicsEngine
{
public:
    OGraphicsEngine();
    ~OGraphicsEngine();
public:
    OVertexArrayObjectPtr createVertexArrayObject(const OVertexBufferDesc& data);
    OUniformBufferPtr createUniformBuffer(const OUniformBufferDesc& desc);
    OShaderProgramPtr createShaderProgram(const OShaderProgramDesc& desc);
    OTexturePtr createTexture(const OTextureDesc& desc);
    ShaderPtr initializeShader(const ShaderDesc& desc);
    //OCameraPtr createCamera(const CameraDesc& desc);
    OCameraPtr createCamera();
    OModelPtr createModel(const char*filename);

public:
    void drawModel(OModelPtr m, ShaderPtr shader_ptr, OCameraPtr camera_ptr);

public:
    void clear(const OVec4& color);
    void enable_depth_test();
    void setViewport(const ORect& size);
    void setVertexArrayObject(const OVertexArrayObjectPtr& vao);
    void setLightVertexArrayObject(const OVertexArrayObjectPtr& vao);
    void setUniformBuffer(const OUniformBufferPtr& buffer, ui32 slot);
    void bindTextureUnits(const OTexturePtr& texture);
    void setShaderProgram(const OShaderProgramPtr& program);
    void drawTriangles(const OTriangleType& triangleType, ui32 vertexCount, ui32 offset);
};