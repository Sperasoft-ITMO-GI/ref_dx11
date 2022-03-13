set DebugDir=..\..\Debug\
set ref_dx11_dir=%DebugDir%ref_dx11\
set shaders_dir=%ref_dx11_dir%shaders\
set ReleaseDir=..\..\release\
set release_ref_dx11_dir=%ReleaseDir%ref_dx11\
set release_shaders_dir=%release_ref_dx11_dir%shaders\

IF NOT EXIST %ref_dx11_dir% mkdir %ref_dx11_dir%
IF NOT EXIST %shaders_dir% mkdir %shaders_dir%
IF NOT EXIST %release_ref_dx11_dir% mkdir %release_ref_dx11_dir%
IF NOT EXIST %release_shaders_dir% mkdir %release_shaders_dir%

xcopy /I/Y/E/Q *.* %shaders_dir%
xcopy /I/Y/E/Q *.* %release_shaders_dir%