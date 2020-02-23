/*
 ***********************************************************************************************************************
 *
 *  Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All Rights Reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *
 **********************************************************************************************************************/
/**
 ***********************************************************************************************************************
 * @file  llpc.h
 * @brief LLPC header file: contains LLPC basic definitions (including interfaces and data types).
 ***********************************************************************************************************************
 */
#pragma once

#include "../../include/vkgcDefs.h"

namespace Llpc
{

static const uint32_t  MaxViewports = 16;
static const char      VkIcdName[]     = "amdvlk";

/// Represents per shader module options.
struct ShaderModuleOptions
{
    PipelineOptions     pipelineOptions;   ///< Pipeline options related with this shader module
    bool                enableOpt;         ///< Enable translate & lower phase in build shader module
};

/// Represents info to build a shader module.
struct ShaderModuleBuildInfo
{
    void*                pInstance;         ///< Vulkan instance object
    void*                pUserData;         ///< User data
    OutputAllocFunc      pfnOutputAlloc;    ///< Output buffer allocator
    BinaryData           shaderBin;         ///< Shader binary data (SPIR-V binary)
    ShaderModuleOptions  options;           ///< Per shader module options
};

/// Represents the base data type
enum class BasicType : uint32_t
{
    Unknown = 0,          ///< Unknown
    Float,                ///< Float
    Double,               ///< Double
    Int,                  ///< Signed integer
    Uint,                 ///< Unsigned integer
    Int64,                ///< 64-bit signed integer
    Uint64,               ///< 64-bit unsigned integer
    Float16,              ///< 16-bit floating-point
    Int16,                ///< 16-bit signed integer
    Uint16,               ///< 16-bit unsigned integer
    Int8,                 ///< 8-bit signed integer
    Uint8,                ///< 8-bit unsigned integer
};

/// Enumerates types of shader binary.
enum class BinaryType : uint32_t
{
    Unknown = 0,  ///< Invalid type
    Spirv,        ///< SPIR-V binary
    LlvmBc,       ///< LLVM bitcode
    MultiLlvmBc,  ///< Multiple LLVM bitcode
    Elf,          ///< ELF
};

/// Represents usage info of a shader module
struct ShaderModuleUsage
{
    bool                  enableVarPtrStorageBuf;  ///< Whether to enable "VariablePointerStorageBuffer" capability
    bool                  enableVarPtr;            ///< Whether to enable "VariablePointer" capability
    bool                  useSubgroupSize;         ///< Whether gl_SubgroupSize is used
    bool                  useHelpInvocation;       ///< Whether fragment shader has helper-invocation for subgroup
    bool                  useSpecConstant;         ///< Whether specializaton constant is used
    bool                  keepUnusedFunctions;     ///< Whether to keep unused function
};

/// Represents common part of shader module data
struct ShaderModuleData
{
    uint32_t         hash[4];       ///< Shader hash code
    BinaryType       binType;       ///< Shader binary type
    BinaryData       binCode;       ///< Shader binary data
    uint32_t         cacheHash[4];  ///< Hash code for calculate pipeline cache key
    ShaderModuleUsage usage;        ///< Usage info of a shader module
};

/// Represents fragment shader output info
struct FsOutInfo
{
    uint32_t    location;       ///< Output location in resource layout
    uint32_t    index;          ///< Output index in resource layout
    BasicType   basicType;      ///< Output data type
    uint32_t    componentCount; ///< Count of components of output data
};

/// Represents extended output of building a shader module (taking extra data info)
struct ShaderModuleDataEx
{
    ShaderModuleData        common;         ///< Shader module common data
    uint32_t                codeOffset;     ///< Binary offset of binCode in ShaderModuleDataEx
    uint32_t                entryOffset;    ///< Shader entry offset in ShaderModuleDataEx
    uint32_t                resNodeOffset;  ///< Resource node offset in ShaderModuleDataEX
    uint32_t                fsOutInfoOffset;///< FsOutInfo offset in ShaderModuleDataEX
    struct
    {
        uint32_t              fsOutInfoCount;           ///< Count of fragment shader output
        const FsOutInfo*      pFsOutInfos;              ///< Fragment output info array
        uint32_t              entryCount;              ///< Shader entry count in the module
        ShaderModuleEntryData entryDatas[1];           ///< Array of all shader entries in this module
    } extra;                              ///< Represents extra part of shader module data
};

/// Represents output of building a shader module.
struct ShaderModuleBuildOut
{
    ShaderModuleData*   pModuleData;       ///< Output shader module data (opaque)
};

/// Represents output of building a graphics pipeline.
struct GraphicsPipelineBuildOut
{
    BinaryData          pipelineBin;        ///< Output pipeline binary data
};

/// Represents output of building a compute pipeline.
struct ComputePipelineBuildOut
{
    BinaryData          pipelineBin;        ///< Output pipeline binary data
};

/// Defines callback function used to lookup shader cache info in an external cache
typedef Result (*ShaderCacheGetValue)(const void* pClientData, uint64_t hash, void* pValue, size_t* pValueLen);

/// Defines callback function used to store shader cache info in an external cache
typedef Result (*ShaderCacheStoreValue)(const void* pClientData, uint64_t hash, const void* pValue, size_t valueLen);

/// Specifies all information necessary to create a shader cache object.
struct ShaderCacheCreateInfo
{
    const void*  pInitialData;      ///< Pointer to a data buffer whose contents should be used to seed the shader
                                    ///  cache. This may be null if no initial data is present.
    size_t       initialDataSize;   ///< Size of the initial data buffer, in bytes.

    // NOTE: The following parameters are all optional, and are only used when the IShaderCache will be used in
    // tandem with an external cache which serves as a backing store for the cached shader data.

    // [optional] Private client-opaque data which will be passed to the pClientData parameters of the Get and
    // Store callback functions.
    const void*            pClientData;
    ShaderCacheGetValue    pfnGetValueFunc;    ///< [Optional] Function to lookup shader cache data in an external cache
    ShaderCacheStoreValue  pfnStoreValueFunc;  ///< [Optional] Function to store shader cache data in an external cache
};

// =====================================================================================================================
/// Represents the interface of a cache for compiled shaders. The shader cache is designed to be optionally passed in at
/// pipeline create time. The compiled binary for the shaders is stored in the cache object to avoid compiling the same
/// shader multiple times. The shader cache also provides a method to serialize its data to be stored to disk.
class IShaderCache
{
public:
    /// Serializes the shader cache data or queries the size required for serialization.
    ///
    /// @param [in]      pBlob  System memory pointer where the serialized data should be placed. This parameter can
    ///                         be null when querying the size of the serialized data. When non-null (and the size is
    ///                         correct/sufficient) then the contents of the shader cache will be placed in this
    ///                         location. The data is an opaque blob which is not intended to be parsed by clients.
    /// @param [in,out]  pSize  Size of the memory pointed to by pBlob. If the value stored in pSize is zero then no
    ///                         data will be copied and instead the size required for serialization will be returned
    ///                         in pSize.
    ///
    /// @returns Success if data was serialized successfully, Unknown if fail to do serialize.
    virtual Result Serialize(
        void*   pBlob,
        size_t* pSize) = 0;

    /// Merges the provided source shader caches' content into this shader cache.
    ///
    /// @param [in]  srcCacheCount  Count of source shader caches to be merged.
    /// @param [in]  ppSrcCaches    Pointer to an array of pointers to shader cache objects.
    ///
    /// @returns Success if data of source shader caches was merged successfully, OutOfMemory if the internal allocator
    ///          memory cannot be allocated.
    virtual Result Merge(
        uint32_t             srcCacheCount,
        const IShaderCache** ppSrcCaches) = 0;

    /// Frees all resources associated with this object.
    virtual void Destroy() = 0;

protected:
    /// @internal Constructor. Prevent use of new operator on this interface.
    IShaderCache() {}

    /// @internal Destructor. Prevent use of delete operator on this interface.
    virtual ~IShaderCache() {}
};

// =====================================================================================================================
/// Represents the interfaces of a pipeline compiler.
class ICompiler
{
public:
    /// Creates pipeline compiler from the specified info.
    ///
    /// @param [in]  optionCount    Count of compilation-option strings
    /// @param [in]  options        An array of compilation-option strings
    /// @param [out] ppCompiler     Pointer to the created pipeline compiler object
    ///
    /// @returns Result::Success if successful. Other return codes indicate failure.
    static Result VKAPI_CALL Create(GfxIpVersion      gfxIp,
                                    uint32_t          optionCount,
                                    const char*const* options,
                                    ICompiler**       ppCompiler);

    /// Checks whether a vertex attribute format is supported by fetch shader.
    ///
    /// @parame [in] format  Vertex attribute format
    ///
    /// @return TRUE if the specified format is supported by fetch shader. Otherwise, FALSE is returned.
    static bool VKAPI_CALL IsVertexFormatSupported(VkFormat format);

    /// Destroys the pipeline compiler.
    virtual void VKAPI_CALL Destroy() = 0;

    /// Convert ColorBufferFormat to fragment shader export format
    ///
    /// param [in] pTarget                  Color target including color buffer format
    /// param [in] enableAlphaToCoverage    Whether enable AlphaToCoverage
    ///
    /// @return uint32_t type casted from fragment shader export format.
    virtual uint32_t ConvertColorBufferFormatToExportFormat(const ColorTarget*  pTarget,
                                                            const bool          enableAlphaToCoverage) const = 0;

    /// Build shader module from the specified info.
    ///
    /// @param [in]  pShaderInfo    Info to build this shader module
    /// @param [out] pShaderOut     Output of building this shader module
    ///
    /// @returns Result::Success if successful. Other return codes indicate failure.
    virtual Result BuildShaderModule(const ShaderModuleBuildInfo* pShaderInfo,
                                    ShaderModuleBuildOut*        pShaderOut) const = 0;

    /// Build graphics pipeline from the specified info.
    ///
    /// @param [in]  pPipelineInfo  Info to build this graphics pipeline
    /// @param [out] pPipelineOut   Output of building this graphics pipeline
    ///
    /// @returns Result::Success if successful. Other return codes indicate failure.
    virtual Result BuildGraphicsPipeline(const GraphicsPipelineBuildInfo* pPipelineInfo,
                                         GraphicsPipelineBuildOut*        pPipelineOut,
                                         void*                            pPipelineDumpFile = nullptr) = 0;

    /// Build compute pipeline from the specified info.
    ///
    /// @param [in]  pPipelineInfo  Info to build this compute pipeline
    /// @param [out] pPipelineOut   Output of building this compute pipeline
    ///
    /// @returns Result::Success if successful. Other return codes indicate failure.
    virtual Result BuildComputePipeline(const ComputePipelineBuildInfo* pPipelineInfo,
                                        ComputePipelineBuildOut*        pPipelineOut,
                                        void*                           pPipelineDumpFile = nullptr) = 0;

#if LLPC_CLIENT_INTERFACE_MAJOR_VERSION < 38
    /// Creates a shader cache object with the requested properties.
    ///
    /// @param [in]  pCreateInfo    Create info of the shader cache.
    /// @param [out] ppShaderCache  Constructed shader cache object.
    ///
    /// @returns Success if the shader cache was successfully created. Otherwise, ErrorOutOfMemory is returned.
    virtual Result CreateShaderCache(
        const ShaderCacheCreateInfo* pCreateInfo,
        IShaderCache**               ppShaderCache) = 0;
#endif

protected:
    ICompiler() {}
    /// Destructor
    virtual ~ICompiler() {}
};

} // Llpc