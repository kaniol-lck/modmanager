# Mod Manager

![GitHub top language](https://img.shields.io/github/languages/top/kaniol-lck/modmanager) ![C++17](https://img.shields.io/badge/C%2B%2B-17-%2300599C) ![Qt 5.15.2](https://img.shields.io/badge/Qt-5.15.2-%2341CD52) ![GitHub](https://img.shields.io/github/license/kaniol-lck/modmanager) [![CodeFactor](https://www.codefactor.io/repository/github/kaniol-lck/modmanager/badge)](https://www.codefactor.io/repository/github/kaniol-lck/modmanager) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/4f9ae02df7ba432aab449f9d5bdfabeb)](https://www.codacy.com/gh/kaniol-lck/modmanager/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=kaniol-lck/modmanager&amp;utm_campaign=Badge_Grade)

**English** | [中文](README_zh.md)

A Qt-based mod manager for Minecraft.

It should be able to manage, update and download mod.

## Build

This repository has deployed [GitHub Actions](https://github.com/kaniol-lck/modmanager/actions). If you want to try my new-written bugs, welcome to try it and open some issues.

### Build Status

| Platform      | Status                                                       |
| ------------- | ------------------------------------------------------------ |
| Windows MinGW | [![windows](https://github.com/kaniol-lck/modmanager/actions/workflows/windows-mingw.yml/badge.svg)](https://github.com/kaniol-lck/modmanager/blob/master/.github/workflows/windows-mingw.yml) |
| Windows MSVC  | [![windows](https://github.com/kaniol-lck/modmanager/actions/workflows/windows-msvc.yml/badge.svg)](https://github.com/kaniol-lck/modmanager/blob/master/.github/workflows/windows-msvc.yml) |
| Linux         | [![ubuntu](https://github.com/kaniol-lck/modmanager/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/kaniol-lck/modmanager/blob/master/.github/workflows/ubuntu.yml) |
| Mac OS        | *UNKNOWN\**                                                  |

\* Mac OS Build failed with c++ 17 on GitHub Actions. （HELP WANTED）

### Build Yourself

Clone this repository: 

```bash
git clone git@github.com:kaniol-lck/modmanager.git
```

#### For GCC/MinGW

Build:

```bash
qmake && make
```

#### For MSVC

Install zlib if you don't have one. (See [zlib.install](https://github.com/horta/zlib.install))

```bash
powershell -Command "(Invoke-WebRequest -Uri https://git.io/JnHTY -OutFile install_zlib.bat)"; ./install_zlib.bat; del install_zlib.bat
```

Build:

```bash
qmake && nmake
```

## Usage

![curseforge_browser](images/curseforge_browser.png)
