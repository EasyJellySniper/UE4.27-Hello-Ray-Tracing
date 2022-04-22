#include "RayTracingCustom.h"
#include "RayTracingCustomDepth.h"
#include "ScreenPass.h"

#if RHI_RAYTRACING

void PrepareRayTracingCustoms(const FViewInfo& View, TArray<FRHIRayTracingShader*>& OutRayGenShaders)
{
	if (!IsRayTracingEnabled())
	{
		return;
	}

	// add custom depth shader
	TShaderMapRef<FCustomDepthRGS> RayGenerationShader(View.ShaderMap);
	OutRayGenShaders.Add(RayGenerationShader.GetRayTracingShader());
}

#endif // RHI_RAYTRACING