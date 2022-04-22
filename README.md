# UE4.27 Hello Ray Tracing
 Simple custom ray tracing example.

Change list: <br>
Engine\Shaders\Private\Custom\CopyDepth.usf  <br>
Engine\Shaders\Private\RayTracing\RayTracingCustomDepth.usf  <br>
Engine\Source\Runtime\Renderer\Private\DeferredShadingRenderer.cpp  <br>
Engine\Source\Runtime\Renderer\Private\RayTracing\RayTracingCustom.cpp  <br>
Engine\Source\Runtime\Renderer\Private\RayTracing\RayTracingCustom.h  <br>
Engine\Source\Runtime\Renderer\Private\RayTracing\RayTracingCustomDepth.cpp  <br>
Engine\Source\Runtime\Renderer\Private\RayTracing\RayTracingCustomDepth.h  <br><br>

Simply copy them to the path to check the effect. <br>
Use r.RayTracing.CustomDepth for toggling. <br>
Implementation explained in my WordPress post. <br><br>

![alt text](https://i.imgur.com/IiflKXY.jpg)
