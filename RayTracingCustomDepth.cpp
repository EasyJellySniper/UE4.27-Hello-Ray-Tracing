#include "RayTracingCustomDepth.h"
#include "RayTracingCustom.h"

#if RHI_RAYTRACING

#include "RayTracingMaterialHitShaders.h"
IMPLEMENT_GLOBAL_SHADER(FCustomDepthRGS, "/Engine/Private/RayTracing/RayTracingCustomDepth.usf", "CustomDepthRG", SF_RayGen);
IMPLEMENT_GLOBAL_SHADER(FCopyDepthPS, "/Engine/Private/Custom/CopyDepth.usf", "MainPS", SF_Pixel);

// toggable cvar
static TAutoConsoleVariable<bool> CVarRayTracingCustomDepth(
	TEXT("r.RayTracing.CustomDepth"),
	true,
	TEXT("Ray tracing custom depth ON/OFF.\n"),
	ECVF_RenderThreadSafe);

#endif // RHI_RAYTRACING 

void RenderRayTracingCustomDepth(FRDGBuilder& GraphBuilder,
	const FSceneTextureParameters& SceneTextures,
	const FViewInfo& View)
#if RHI_RAYTRACING
{
	if (!IsRayTracingEnabled())
	{
		return;
	}
	
	if (!CVarRayTracingCustomDepth->GetBool())
	{
		return;
	}
	
	RDG_EVENT_SCOPE(GraphBuilder, "%s", TEXT("RenderRayTracingCustomDepth"));
	
	// setup pass parameter for custom depth
	FCustomDepthRGS::FParameters* PassParameters = GraphBuilder.AllocParameters<FCustomDepthRGS::FParameters>();
	PassParameters->View = View.ViewUniformBuffer;
	PassParameters->TLAS = View.RayTracingScene.RayTracingSceneRHI->GetShaderResourceView();

	// create output texture
	FRDGTextureDesc TexDesc = FRDGTextureDesc::Create2D(SceneTextures.SceneDepthTexture->Desc.Extent, PF_R32_FLOAT, FClearValueBinding::Black, TexCreate_UAV | TexCreate_ShaderResource | TexCreate_RenderTargetable);
	FRDGTextureRef OutputDepth = GraphBuilder.CreateTexture(TexDesc, TEXT("Raytracing Output Depth"));
	PassParameters->OutputDepth = GraphBuilder.CreateUAV(OutputDepth);

	// add pass
	FIntPoint Resolution(View.ViewRect.Width(), View.ViewRect.Height());
	TShaderMapRef<FCustomDepthRGS> RayGenerationShader(View.ShaderMap);

	ClearUnusedGraphResources(RayGenerationShader, PassParameters);

	// set r.RDG.CullPasses = 0, so we can preview the pass without output being used
	GraphBuilder.AddPass(
	RDG_EVENT_NAME("RayTracedCustomDepth %dx%d", Resolution.X, Resolution.Y),
	PassParameters,
	ERDGPassFlags::Compute,
	[&View, RayGenerationShader, PassParameters, Resolution](FRHICommandList& RHICmdList)
	{
		FRayTracingShaderBindingsWriter GlobalResources;
		SetShaderParameters(GlobalResources, RayGenerationShader, *PassParameters);

		// the hit group and miss shader are bind in RayTracingMaterialPipeline, simply reuse it
		FRHIRayTracingScene* RayTracingSceneRHI = View.RayTracingScene.RayTracingSceneRHI;

		// if we want to reuse material pipeline, we need to add this pass after WaitForRayTracingScene() in DeferredShadingRenderer.cpp
		// otherwise the hit group is still invalid 
		// RHICmdList.RayTraceDispatch(View.RayTracingMaterialPipeline, RayGenerationShader.GetRayTracingShader(), RayTracingSceneRHI, GlobalResources, Resolution.X, Resolution.Y);
		
		FRayTracingPipelineStateInitializer Initializer;
		Initializer.MaxPayloadSizeInBytes = 64; // sizeof(FPackedMaterialClosestHitPayload)
		
		FRHIRayTracingShader* RayGenShaderTable[] = { RayGenerationShader.GetRayTracingShader() };
		Initializer.SetRayGenShaderTable(RayGenShaderTable);
		
		FRHIRayTracingShader* HitGroupTable[] = { View.ShaderMap->GetShader<FOpaqueShadowHitGroup>().GetRayTracingShader() };
		Initializer.SetHitGroupTable(HitGroupTable);
		Initializer.bAllowHitGroupIndexing = false; // Use the same hit shader for all geometry in the scene by disabling SBT indexing.
		
		FRayTracingPipelineState* Pipeline = PipelineStateCache::GetAndOrCreateRayTracingPipelineState(RHICmdList, Initializer);
		RHICmdList.RayTraceDispatch(Pipeline, RayGenerationShader.GetRayTracingShader(), RayTracingSceneRHI, GlobalResources, Resolution.X, Resolution.Y);
	});

	// copy depth to scene depth pass
	FCopyDepthPS::FParameters* CopyDepthParameters = GraphBuilder.AllocParameters<FCopyDepthPS::FParameters>();
	CopyDepthParameters->View = View.ViewUniformBuffer;
	CopyDepthParameters->InputDepth = OutputDepth;
	CopyDepthParameters->InputSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp>::GetRHI();
	CopyDepthParameters->RenderTargets.DepthStencil = FDepthStencilBinding(SceneTextures.SceneDepthTexture, ERenderTargetLoadAction::ELoad, FExclusiveDepthStencil::DepthWrite_StencilNop);

	TShaderMapRef<FScreenPassVS> ScreenPassVertexShader(View.ShaderMap);
	TShaderMapRef<FCopyDepthPS> CopyDepthPixelShader(View.ShaderMap);
	const FScreenPassTextureViewport Viewport(SceneTextures.SceneDepthTexture, View.ViewRect);
	FRHIDepthStencilState* CopyDepthStencilState = TStaticDepthStencilState<true,CF_Always>::GetRHI();
	
	AddDrawScreenPass(GraphBuilder, FRDGEventName(TEXT("CopyDepthToSceneDepth")), View, Viewport, Viewport, ScreenPassVertexShader, CopyDepthPixelShader, CopyDepthStencilState, CopyDepthParameters);
}
#else
{
	unimplemented();
}
#endif // RHI_RAYTRACING 