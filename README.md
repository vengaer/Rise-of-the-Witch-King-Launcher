# Rise of the Witch-King Launcher

A custom launcher for [The Battle for Middle Earth II The Rise of the Witch-King](https://bfme.fandom.com/wiki/The_Rise_of_the_Witch-king) version 2.02 supporting the [Edain](https://www.moddb.com/mods/edain-mod) and [Battles of the Third Age](https://moddb.com/mods/battles-of-the-third-age) mods.

[![Build Status](https://gitlab.com/vilhelmengstrom/rotwkl/badges/master/pipeline.svg)](https://gitlab.com/vilhelmengstrom/rotwkl/commits/master)
[![Build Status](https://ci.appveyor.com/api/projects/status/csbvmnmcfs5xvc33?svg=true)](https://ci.appveyor.com/project/Vilhelmengstrom/rotwkl)

_Use at your own risk_. I _will not_ be held responsible for broken installations caused from (mis)use of this piece of software. See [the license](LICENSE). **Always make a backup before trying this with a new installation.**

## Install

First of all:
 - Patch your game to version 2.02.
 - If you have Edain installed, make sure it is disabled.

### Windows


#### Install dependencies

 - Install [Qt](https://www.qt.io) (the open source version is enough)
    - Assumed to be installed to `C:\Qt`. If installed elsewhere, just use that path instead.
 - Install [msys](https://www.msys2.org) (follow the instructions on the page)
    - Assumed to be installed to `C:\msys64`.
 - Launch `C:\msys64\mingw64.exe` and run `pacman --needed -S make git gcc mingw-w64-x86_64-toolchain mingw-w64-x86_64-openssl`

#### Build and package

In the MSYS MinGW64 shell:
 - `cd && git clone https://gitlab.com/vilhelmengstrom/rotwkl.git && cd rotwkl`
 - `make release QT_PATH=C:/Qt/<version>/mingw73_64`
 - `mkdir rotwk_launcher`
 - `cp -r {images,toml,xml,rotwkl} rotwk_launcher`
 - `cp C:/msys64/mingw64/bin/{libgcc_s_seh-1,libstdc++-6,libwinpthread-1}.dll rotwk_launcher`
 - `cp C:/Qt/<version>/mingw73_64/bin/{Qt5Core,Qt5Gui,Qt5Svg,Qt5Widgets}.dll rotwk_launcher`

After doing this, `rotwk_launcher` will contain everything needed to run the launcher.

## Disclaimer

This piece of software is not affiliated with either the Edain or BotTA mods. All credit for the mods go to the respective authors.
