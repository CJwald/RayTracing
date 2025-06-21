@echo off

pushd ..
msbuild RayTracing.sln /Flags...
popd
pause
