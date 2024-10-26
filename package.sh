#!/bin/bash

PKG_TITLE="$1"
PKG_VERSION="$2"
PKG_TITLE_ID="$3"
PKG_CONTENT_ID="$4"

$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_new sce_sys/param.sfo
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo APP_TYPE --type Integer --maxsize 4 --value 1
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo APP_VER --type Utf8 --maxsize 8 --value "$PKG_VERSION"
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo ATTRIBUTE --type Integer --maxsize 4 --value 0
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo CATEGORY --type Utf8 --maxsize 4 --value "gd"
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo CONTENT_ID --type Utf8 --maxsize 48 --value "$PKG_CONTENT_ID"
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo DOWNLOAD_DATA_SIZE --type Integer --maxsize 4 --value 0
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo SYSTEM_VER --type Integer --maxsize 4 --value 0
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo TITLE --type Utf8 --maxsize 128 --value "$PKG_TITLE"
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo TITLE_ID --type Utf8 --maxsize 12 --value "$PKG_TITLE_ID"
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core sfo_setentry sce_sys/param.sfo VERSION --type Utf8 --maxsize 8 --value "$PKG_VERSION"

FILES=$(find assets/ -type f -printf '%p ')
FILES="$FILES$(find sce_module/ -type f -name '*.prx' -printf '%p ')"

# Create gp4
$OO_PS4_TOOLCHAIN/bin/linux/create-gp4 -out pkg.gp4 --content-id="$PKG_CONTENT_ID" --files "eboot.bin sce_sys/about/right.sprx sce_sys/param.sfo sce_sys/icon0.png $FILES"

# Create pkg
$OO_PS4_TOOLCHAIN/bin/linux/PkgTool.Core pkg_build pkg.gp4 .
