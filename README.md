# Ray Tracing

Leveraging the code from "The Cherno" youtube series "Ray Tracing", added recursive ray tracing to loop rays back when they hit a sphere boundary

Adding shaders using opengl

## Building and running
Officially supports Windows 10/11 and Visual Studio 2022, with more platforms coming soon. You'll need to have the [Vulkan SDK](https://vulkan.lunarg.com/) installed.

1. Clone recursively: `git clone --recursive https://github.com/CJwald/RayTracing`
2. Run `scripts/Setup.bat`
3. Open `RayTracing.sln` and hit F5 (preferably change configuration to Release or Dist first, Debug is slow)