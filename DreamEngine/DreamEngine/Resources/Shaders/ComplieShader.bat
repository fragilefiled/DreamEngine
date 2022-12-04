@echo off
for /r %%a in (*.vert) do F:/VKSDK/Bin/glslc.exe  -o %%a.spv %%a
for /r %%a in (*.frag) do F:/VKSDK/Bin/glslc.exe  -o %%a.spv %%a
echo Compile Success!
pause
