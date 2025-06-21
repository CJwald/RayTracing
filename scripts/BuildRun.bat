@echo off

pushd ..
msbuild RayTracing.sln
popd

pushd ..\bin\Dist-windows-x86_64\RayTracing\
RayTracing.exe
popd
