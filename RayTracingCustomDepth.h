#pragma once
#include "GlobalShader.h"
#include "SceneRendering.h"

#if RHI_RAYTRACING

// custom depth ray generation shader class
class FCustomDepthRGS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FCustomDepthRGS)
	SHADER_USE_ROOT_PARAMETER_STRUCT(FCustomDepthRGS, FGlobalShader)

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return ShouldCompileRayTracingShadersForProject(Parameters.Platform);
	}

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_SRV(RaytracingAccelerationStructure, TLAS)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float>, OutputDepth)
	END_SHADER_PARAMETER_STRUCT()
};

// copy depth shader
class FCopyDepthPS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FCopyDepthPS);
	SHADER_USE_PARAMETER_STRUCT(FCopyDepthPS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, InputDepth)
		SHADER_PARAMETER_SAMPLER(SamplerState, InputSampler)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{ 
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
};

#endif // RHI_RAYTRACING