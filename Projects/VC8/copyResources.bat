@echo off
set recPath=%1\Resources

echo Creating Resources Directory...
rmdir /q /s %recPath%
mkdir %recPath%

echo Copying Manifests...
copy ..\..\Source\Resources\*.xml %recPath%

echo Copying Textures...
copy ..\..\Source\Resources\*.tiff %recPath%

echo Copying Shader Resources...
copy ..\..\Source\Shaders\*.vsh %recPath%
copy ..\..\Source\Shaders\*.gsh %recPath%
copy ..\..\Source\Shaders\*.fsh %recPath%

echo Copying Dependency Libraries
copy ..\..\Dependencies\libtiff\lib\zlib1.dll %1\zlib1.dll
copy ..\..\Dependencies\libtiff\lib\jpeg62.dll %1\jpeg62.dll
copy ..\..\Dependencies\libtiff\lib\libtiff3.dll %1\libtiff3.dll
copy ..\..\Dependencies\freetype\lib\freetype6.dll %1\freetype6.dll
copy ..\..\Dependencies\pthreads\lib\pthreadVC2.dll %1\pthreadVC2.dll
