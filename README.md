<img width="256" height="256" align="left" style="float: left; margin: 0 10px 0 0;" src="images/modmanager.png" alt="modmanager.png"/><br />

<h1>Mod Manager</h1>

![C++17](https://img.shields.io/badge/C%2B%2B-17-%2300599C) ![Qt 5.15.2](https://img.shields.io/badge/Qt-5.15.2-%2341CD52) ![GitHub](https://img.shields.io/github/license/kaniol-lck/modmanager) [![CodeFactor](https://www.codefactor.io/repository/github/kaniol-lck/modmanager/badge)](https://www.codefactor.io/repository/github/kaniol-lck/modmanager) [![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/kaniol-lck/modmanager?include_prereleases)](Changelog.md) [![GitHub all releases](https://img.shields.io/github/downloads/kaniol-lck/modmanager/total)](https://github.com/kaniol-lck/modmanager/releases) [![CI](https://github.com/kaniol-lck/modmanager/actions/workflows/ci.yml/badge.svg?event)](https://github.com/kaniol-lck/modmanager/actions/workflows/ci.yml)

**English** | [中文](README_zh.md)

A Qt-based mod manager for Minecraft, able to manage, update and download mod.

## License

Mod Manager is licensed under [GPLv3](LICENSE).

## Build

This repository has deployed [GitHub Actions](https://github.com/kaniol-lck/modmanager/actions). If you want to try my new-written bugs, welcome to try it and open some issues.

### Build From Source

Clone this repository: 

```bash
git clone git@github.com:kaniol-lck/modmanager.git
```

#### Install libaria2

Depends on your OS and package managers:

- Ubuntu (apt-get):

  ```sh
  apt-get install libaria2-0-dev
  ```

- Archlinux (aur):

  ```sh
  yay install aria2
  ```

- Windows (MSYS2):

  ```sh
  pacman -S mingw-w64-x86_64-aria2
  ```

- MacOS
  See [kaniol-lck/aria2 Releases](https://github.com/kaniol-lck/aria2/releases) , extract files to your system paths.

#### For Clang/GCC/MinGW

Build:

```bash
qmake && make
```

#### For MSVC

**\* HELP WANTED: I don't know how to use MSVC to build libaria2**

Install zlib if you don't have one. (See [zlib.install](https://github.com/horta/zlib.install))

```bash
powershell -Command "(Invoke-WebRequest -Uri https://git.io/JnHTY -OutFile install_zlib.bat)"; ./install_zlib.bat; del install_zlib.bat
```

Build:

```bash
qmake && nmake
```

## Usage

(Background in images is my wallpaper, because toolbar is semi-transparent on KDE)

### Explore - Download Mod

You can browse and download mod from:

- Curseforge
- Modrinth
- OptiFine
- ReplayMod

![curseforge_browser](images/curseforge_browser.png)

### Local - Manage Mod

You can browse and manage your local mod:

- update & rollback
- tag your mods
- batch rename
- disable mods

![local_browser](images/local_browser.png)
