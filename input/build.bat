SETLOCAL EnableDelayedExpansion

Rem Package information
set PKG_TITLE="PS4 Test Input System"
set PKG_VERSION="1.2"
set PKG_ASSETS="assets"
set PKG_TITLE_ID="DDRM00001"
set PKG_CONTENT_ID="IV0000-%PKG_TITLE_ID%_00-PS4INPUTSY000000"

Rem Libraries to link in
set libraries=-lc -lm -lkernel -lc++ -lSceVideoOut -lSceAudioOut -lScePad -lSceUserService

set extra_flags=-O3

Rem Read the script arguments into local vars
set intdir=%1

set outputElf=%intdir%\input_ps4.elf
set outputOelf=%intdir%\input_ps4.oelf

set compiler=clang
set compilerPP=clang++

@mkdir %intdir%

Rem Compile object files for all the source files
for %%f in (*.c) do (
    %compiler% --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -I"%OO_PS4_TOOLCHAIN%\\include" -I"%OO_PS4_TOOLCHAIN%\\include\\c++\\v1" %extra_flags% -c -o %intdir%\%%~nf.o %%~nf.c
    set failed=1
    if errorlevel 0 if not errorlevel 1 set "failed="
    if defined failed exit 1
)

for %%f in (*.cpp) do (
    %compilerPP% --target=x86_64-pc-freebsd12-elf -fPIC -funwind-tables -I"%OO_PS4_TOOLCHAIN%\\include" -I"%OO_PS4_TOOLCHAIN%\\include\\c++\\v1" %extra_flags% -c -o %intdir%\%%~nf.o %%~nf.cpp
    set failed=1
    if errorlevel 0 if not errorlevel 1 set "failed="
    if defined failed exit 1
)

Rem Get a list of object files for linking
set obj_files=
for %%f in (%1\\*.o) do set obj_files=!obj_files! .\%%f

Rem Link the input ELF
ld.lld -m elf_x86_64 -pie --script "%OO_PS4_TOOLCHAIN%\link.x" --eh-frame-hdr -o "%outputElf%" "-L%OO_PS4_TOOLCHAIN%\\lib" %libraries% --verbose "%OO_PS4_TOOLCHAIN%\lib\crt1.o" %obj_files%
if NOT ERRORLEVEL 0 exit 1

Rem Create the eboot
%OO_PS4_TOOLCHAIN%\bin\windows\create-fself.exe -in "%outputElf%" --out "%outputOelf%" --eboot "../eboot.bin" --paid 0x3800000000000011
if NOT ERRORLEVEL 0 exit 1

if NOT "%2"=="pkg" exit 0
Rem Create param.sfo
cd ..
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_new sce_sys/param.sfo
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo APP_TYPE --type Integer --maxsize 4 --value 1
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo APP_VER --type Utf8 --maxsize 8 --value %PKG_VERSION%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo ATTRIBUTE --type Integer --maxsize 4 --value 0
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo CATEGORY --type Utf8 --maxsize 4 --value "gd"
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo CONTENT_ID --type Utf8 --maxsize 48 --value %PKG_CONTENT_ID%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo SYSTEM_VER --type Integer --maxsize 4 --value 0
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo TITLE --type Utf8 --maxsize 128 --value %PKG_TITLE%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo TITLE_ID --type Utf8 --maxsize 12 --value %PKG_TITLE_ID%
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe sfo_setentry sce_sys/param.sfo VERSION --type Utf8 --maxsize 8 --value %PKG_VERSION%

Rem Get a list of assets for packaging
set module_files=
for %%f in (sce_module\\*) do set module_files=!module_files! sce_module/%%~nxf

set asset_audio_files=
for %%f in (assets\\audio\\*) do set asset_audio_files=!asset_audio_files! assets/audio/%%~nxf

set asset_fonts_files=
for %%f in (assets\\fonts\\*) do set asset_fonts_files=!asset_fonts_files! assets/fonts/%%~nxf

set asset_images_files=
for %%f in (assets\\images\\*) do set asset_images_files=!asset_images_files! assets/images/%%~nxf

set asset_misc_files=
for %%f in (assets\\misc\\*) do set asset_misc_files=!asset_misc_files! assets/misc/%%~nxf

set asset_videos_files=
for %%f in (assets\\videos\\*) do set asset_videos_files=!asset_videos_files! assets/videos/%%~nxf

Rem Create gp4
%OO_PS4_TOOLCHAIN%\bin\windows\create-gp4.exe -out pkg.gp4 --content-id=%PKG_CONTENT_ID% --files "eboot.bin sce_sys/about/right.sprx sce_sys/param.sfo sce_sys/icon0.png %module_files% %asset_audio_files% %asset_fonts_files% %asset_images_files% %asset_misc_files% %asset_videos_files%"

Rem Create pkg
%OO_PS4_TOOLCHAIN%\bin\windows\PkgTool.Core.exe pkg_build pkg.gp4 .
