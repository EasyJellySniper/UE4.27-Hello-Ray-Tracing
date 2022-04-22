#pragma once
#include "GlobalShader.h"
#include "SceneRendering.h"
#include "SceneTextureParameters.h"

#if RHI_RAYTRACING

// should rendering custom ray tracing?
inline bool ShouldRenderRayTracingCustom()
{
	return IsRayTracingEnabled();
}

// prepare all custom ray tracing stuff here
void PrepareRayTracingCustoms(const FViewInfo& View, TArray<FRHIRayTracingShader*>& OutRayGenShaders);

// Render raytracing pass
void RenderRayTracingCustomDepth(FRDGBuilder& GraphBuilder,
	const FSceneTextureParameters& SceneTextures,
	const FViewInfo& View);

#endif // RHI_RAYTRACING