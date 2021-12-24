set DebugDir=..\..\Debug\
set ref_dx11_dir=%DebugDir%ref_dx11\
set shaders_dir=%ref_dx11_dir%shaders\

IF NOT EXIST %ref_dx11_dir% mkdir %ref_dx11_dir%
IF NOT EXIST %shaders_dir% mkdir %shaders_dir%

xcopy /I/Y/E/Q *.hlsl %shaders_dir%