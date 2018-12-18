/**
 * Copyright (C) 2015-2018 Think Silicon S.A. (https://think-silicon.com/)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public v3
 * License as published by the Free Software Foundation;
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 */

/**
 *  @file       shaderProgram.h
 *  @author     Think Silicon
 *  @date       25/07/2018
 *  @version    1.0
 *
 *  @brief      Shader Program Functionality in GLOVE
 *
 */

#ifndef __SHADERPROGRAM_H__
#define __SHADERPROGRAM_H__

#include "shader.h"
#include "shaderResourceInterface.h"
#include "utils/cacheManager.h"
#include "genericVertexAttribute.h"
#include "vulkan/pipelineCache.h"
#include "refObject.h"

class Context;

class ShaderProgram : public refObject {
private:
    const vulkanAPI::vkContext_t                       *mVkContext;

    VkDescriptorSetLayout                               mVkDescSetLayout;
    VkDescriptorSetLayoutBinding                       *mVkDescSetLayoutBind;
    VkDescriptorPool                                    mVkDescPool;
    VkDescriptorSet                                     mVkDescSet;
    VkPipelineLayout                                    mVkPipelineLayout;

    vulkanAPI::PipelineCache                           *mPipelineCache;
    CacheManager                                       *mCacheManager;

    VkPipelineVertexInputStateCreateInfo                mVkPipelineVertexInput;
    VkVertexInputBindingDescription                     mVkVertexInputBinding[GLOVE_MAX_VERTEX_ATTRIBS];
    VkVertexInputAttributeDescription                   mVkVertexInputAttribute[GLOVE_MAX_VERTEX_ATTRIBS];

    uint32_t                                            mActiveVertexVkBuffersCount;
    VkBuffer                                            mActiveVertexVkBuffers[GLOVE_MAX_VERTEX_ATTRIBS];

    BufferObject                                       *mExplicitIbo;
    VkBuffer                                            mActiveIndexVkBuffer;

    bool                                                mUpdateDescriptorSets;
    bool                                                mUpdateDescriptorData;
    bool                                                mLinked;
    bool                                                mIsPrecompiled;
    bool                                                mValidated;

    float                                               mMinDepthRange;
    float                                               mMaxDepthRange;

    uint32_t                                            mStageCount;
#define MAX_SHADERS 2
    size_t                                              mShaderSPVsize[MAX_SHADERS];
    uint32_t                                           *mShaderSPVdata[MAX_SHADERS];
    VkShaderModule                                      mVkShaderModules[MAX_SHADERS];
    VkShaderStageFlagBits                               mVkShaderStages[MAX_SHADERS];
    Shader                                             *mShaders[MAX_SHADERS];
    int                                                 mStagesIDs[MAX_SHADERS];

    ShaderCompiler                                     *mShaderCompiler;
    ShaderResourceInterface                             mShaderResourceInterface;

    bool                                                ValidateProgram(void);
    void                                                ReleaseVkObjects(void);
    bool                                                AllocateVkDescriptoSet(void);
    bool                                                CreateDescriptorSetLayout(uint32_t nLiveUniformBlocks);
    bool                                                CreateDescriptorPool(uint32_t nLiveUniformBlocks);
    bool                                                CreateDescriptorSet(void);
    void                                                UpdateSamplerDescriptors(void);

    uint32_t                                            SerializeShadersSpirv(void *binary);
    uint32_t                                            DeserializeShadersSpirv(const void *binary);

    void                                                ResetVulkanVertexInput(void);
    void                                                UpdateAttributeInterface(void);
    void                                                BuildShaderResourceInterface(void);
    bool                                                UpdateVertexAttribProperties(size_t vertCount, uint32_t firstVertex, std::vector<GenericVertexAttribute>& genericVertAttribs, std::map<uint32_t, uint32_t>& vboLocationBindings, bool updatedVertexAttrib);
    void                                                GenerateVertexInputProperties(std::vector<GenericVertexAttribute>& genericVertAttribs, const std::map<uint32_t, uint32_t>& vboLocationBindings);

    void                                                LineLoopConversion(void* data, uint32_t indexCount, size_t elementByteSize);
    bool                                                ConvertIndexBufferToUint16(const void* srcData, size_t elementCount, BufferObject** ibo);
    bool                                                AllocateExplicitIndexBuffer(const void* data, size_t size, BufferObject** ibo);
    uint32_t                                            GetMaxIndex(BufferObject* ibo, uint32_t indexCount, size_t actualSize, VkDeviceSize offset);

public:
    ShaderProgram(const vulkanAPI::vkContext_t *vkContext = nullptr);
    ~ShaderProgram() override;

    void                                                SetPipelineVertexInputStateInfo(void);
    bool                                                SetPipelineShaderStage(uint32_t &pipelineShaderStageCount, int *pipelineStagesIDs, VkPipelineShaderStageCreateInfo *pipelineShaderStages);
    void                                                PrepareIndexBufferObject(uint32_t* firstIndex, uint32_t* maxIndex, uint32_t indexCount, GLenum type, const void* indices, BufferObject* ibo);
    bool                                                PrepareVertexAttribBufferObjects(size_t vertCount, uint32_t firstVertex, std::vector<GenericVertexAttribute>& genericVertAttribs, bool updatedVertexAttrib);
    Shader                                             *IsShaderAttached(Shader *shader) const;
    void                                                AttachShader(Shader *shader);
    void                                                DetachShader(Shader *shader);
    int                                                 GetInfoLogLength(void) const;
    char                                               *GetInfoLog(void) const;
    bool                                                LinkProgram();

    void                                                DetachShaders(void);

    void                                                Validate(void);
    bool                                                ValidateSamplers(void);
    void                                                EnableUpdateOfDescriptorSets(void)                  { FUN_ENTRY(GL_LOG_TRACE); mUpdateDescriptorSets = true; }

    void                                                UsePrecompiledBinary(const void *binary, size_t binarySize);
    void                                                GetBinaryData(void *binary, GLsizei *binarySize);
    GLsizei                                             GetBinaryLength(void);

    uint32_t                                            GetNumberOfActiveUniforms(void)             const   { FUN_ENTRY(GL_LOG_TRACE); return mShaderResourceInterface.GetLiveUniforms(); }
    int                                                 GetUniformLocation(const char *name)        const   { FUN_ENTRY(GL_LOG_TRACE); return mShaderResourceInterface.GetUniformLocation(name); }
    const ShaderResourceInterface::uniform             *GetUniform(uint32_t index)                  const   { FUN_ENTRY(GL_LOG_TRACE); return mShaderResourceInterface.GetUniform(index); }
    const ShaderResourceInterface::uniform             *GetUniformAtLocation(uint32_t location)     const   { FUN_ENTRY(GL_LOG_TRACE); return mShaderResourceInterface.GetUniformAtLocation(location); }
    uint32_t                                            GetStageCount(void)                         const   { FUN_ENTRY(GL_LOG_TRACE); return mStageCount; }
    VkShaderStageFlagBits                               GetShaderStage(void)                        const   { FUN_ENTRY(GL_LOG_TRACE); return mVkShaderStages[0]; }
    VkShaderModule                                      GetShaderModule(void)                       const   { FUN_ENTRY(GL_LOG_TRACE); return mVkShaderModules[0]; }
    VkShaderModule                                      GetVertexShaderModule(void)                 const   { FUN_ENTRY(GL_LOG_TRACE); return mVkShaderModules[0]; }
    VkShaderModule                                      GetFragmentShaderModule(void)               const   { FUN_ENTRY(GL_LOG_TRACE); return mVkShaderModules[1]; }

    Shader                                             *GetVertexShader(void)                       const   { FUN_ENTRY(GL_LOG_TRACE); return mShaders[0]; }
    Shader                                             *GetFragmentShader(void)                     const   { FUN_ENTRY(GL_LOG_TRACE); return mShaders[1]; }
    size_t                                              GetActiveUniformMaxLen(void)                const   { FUN_ENTRY(GL_LOG_TRACE); return mShaderResourceInterface.GetActiveUniformMaxLen(); }
    size_t                                              GetActiveAttribMaxLen(void)                 const   { FUN_ENTRY(GL_LOG_TRACE); return mShaderResourceInterface.GetActiveAttribMaxLen(); }
    VkPipelineVertexInputStateCreateInfo               *GetVkPipelineVertexInput(void)                      { FUN_ENTRY(GL_LOG_TRACE); return &mVkPipelineVertexInput; }
    VkPipelineLayout                                    GetVkPipelineLayout(void)                   const   { FUN_ENTRY(GL_LOG_TRACE); return mVkPipelineLayout; }
    int                                                 GetStagesIDs(uint32_t index)                const   { FUN_ENTRY(GL_LOG_TRACE); return mStagesIDs[index]; }
    const VkDescriptorSet                              *GetVkDescSet(void)                          const   { FUN_ENTRY(GL_LOG_TRACE); return &mVkDescSet; }
    uint32_t                                            GetActiveVertexVkBuffersCount(void)         const   { FUN_ENTRY(GL_LOG_TRACE); return mActiveVertexVkBuffersCount; }
    const VkBuffer                                     *GetActiveVertexVkBuffers(void)              const   { FUN_ENTRY(GL_LOG_TRACE); return mActiveVertexVkBuffers; }
    VkBuffer                                            GetActiveIndexVkBuffer(void)                const   { FUN_ENTRY(GL_LOG_TRACE); return mActiveIndexVkBuffer; }

    void                                                SetVkContext(const vulkanAPI::vkContext_t *vkContext) { FUN_ENTRY(GL_LOG_TRACE); mVkContext = vkContext; mPipelineCache->SetContext(mVkContext);}
    void                                                SetShaderCompiler(ShaderCompiler* shaderCompiler)   { FUN_ENTRY(GL_LOG_TRACE); assert(shaderCompiler != nullptr); mShaderCompiler = shaderCompiler; }
    void                                                SetStagesIDs(uint32_t index, uint32_t id)           { FUN_ENTRY(GL_LOG_TRACE); mStagesIDs[index] = id; }

    void                                                SetCustomAttribsLayout(const char *name, int index) { FUN_ENTRY(GL_LOG_TRACE); mShaderResourceInterface.SetCustomAttribsLayout(name, index); }
    void                                                SetUniformData(uint32_t location, size_t size, const void *ptr);
    void                                                GetUniformData(uint32_t location, size_t size, void *ptr) const;
    void                                                SetSampler(uint32_t location, int count, const int *textureUnit);
    void                                                SetCacheManager(CacheManager *cacheManager);
    void                                                UpdateDescriptorSet(void);
    void                                                UpdateBuiltInUniformData(float minDepthRange, float maxDepthRange);

    uint32_t                                            GetNumberOfActiveAttributes(void) const;
    const
    ShaderResourceInterface::attribute                 *GetVertexAttribute(int index) const;
    const string &                                      GetAttributeName(int index) const;
    int                                                 GetAttributeType(int index) const;
    int                                                 GetAttributeLocation(const char *name) const;
    VkPipelineCache                                     GetVkPipelineCache(void);
    void                                                SetShaderModules(void);

    bool                                                HasVertexShader(void)                       const   { FUN_ENTRY(GL_LOG_TRACE); return (bool)mShaders[0]; }
    bool                                                HasFragmentShader(void)                     const   { FUN_ENTRY(GL_LOG_TRACE); return (bool)mShaders[1]; }
    bool                                                HasStages(void)                             const   { FUN_ENTRY(GL_LOG_TRACE); return mStageCount; }
    bool                                                HasStagesUpdated(int stageIDs[2])           const   { FUN_ENTRY(GL_LOG_TRACE); return (stageIDs[0] != GetStagesIDs(0) || stageIDs[1] != GetStagesIDs(1)) ? true : false; }
    bool                                                IsLinked(void)                              const   { FUN_ENTRY(GL_LOG_TRACE); return mLinked; }
    bool                                                IsPrecompiled(void)                         const   { FUN_ENTRY(GL_LOG_TRACE); return mIsPrecompiled; }
    bool                                                IsValidated(void)                           const   { FUN_ENTRY(GL_LOG_TRACE); return mValidated; }
};

#endif // __PROGRAM_H__
