# SaveCountManagerF4
This is a plugin that manages the number of save files for Fallout 4.

# Requirements

* Building
    * Microsoft Visual Studio 2019 or later
    * CMake 3.20 or later
    * vcpkg
* Installation
    * [F4SE](http://f4se.silverlock.org/)
    * [Address Library](https://www.nexusmods.com/fallout4/mods/47327)
    * [64-bit Visual C++ 2019/2022 Redistributable](https://aka.ms/vs/17/release/vc_redist.x64.exe)

# Building
```
git clone https://github.com/Ryan-rsm-McKenzie/CommonLibF4
cd CommonLibF4
git clone https://github.com/WirelessLan/SaveCountManagerF4
cd SaveCountManagerF4
cmake --preset vs2022-windows-vcpkg
cmake --build build --config Release
```
