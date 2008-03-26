@echo off
set dirPath=%1

echo Creating Resources Directory...
rmdir /q /s %dirPath%
mkdir %dirPath%

echo Copying Manifests...
copy ..\..\Source\Resources\*.xml %dirPath%

echo Copying Textures...
copy ..\..\Source\Resources\*.tiff %dirPath%

echo Copying Shader Resources...
copy ..\..\Source\Shaders\*.vsh %dirPath%
copy ..\..\Source\Shaders\*.gsh %dirPath%
copy ..\..\Source\Shaders\*.fsh %dirPath%

