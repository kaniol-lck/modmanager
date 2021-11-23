<img width="256" height="256" align="left" style="float: left; margin: 0 10px 0 0;" src="images/modmanager.png" alt="modmanager.png"/><br />

<h1>Mod管理器</h1>

![C++17](https://img.shields.io/badge/C%2B%2B-17-%2300599C) ![Qt 5.15.2](https://img.shields.io/badge/Qt-5.15.2-%2341CD52) ![GitHub](https://img.shields.io/github/license/kaniol-lck/modmanager) [![CodeFactor](https://www.codefactor.io/repository/github/kaniol-lck/modmanager/badge)](https://www.codefactor.io/repository/github/kaniol-lck/modmanager)  [![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/kaniol-lck/modmanager?include_prereleases)](Changelog.md) [![GitHub all releases](https://img.shields.io/github/downloads/kaniol-lck/modmanager/total)](https://github.com/kaniol-lck/modmanager/releases) [![CI](https://github.com/kaniol-lck/modmanager/actions/workflows/ci.yml/badge.svg?event)](https://github.com/kaniol-lck/modmanager/actions/workflows/ci.yml)

[English](README.md) | **中文**

一个基于Qt的Minecraft Mod管理器，可以管理、更新与下载Mod。

## License

Mod管理器以[GPLv3](LICENSE)许可证发布。

## 安装

见[Release](https://github.com/kaniol-lck/modmanager/releases)。

## 构建

本项目已经配置了[GitHub Actions](https://github.com/kaniol-lck/modmanager/actions)，如果你想要尝试最新鲜的bug，可以直接在其中选择下载并帮我捉虫。

### 从源码构建

克隆本仓库至本地：

```bash
git clone git@github.com:kaniol-lck/modmanager.git
```

#### 安装依赖库

该项目有以下外部依赖库：

- Qt（5.15.2或也许其他版本）
- quazip
- libaria2

具体安装方式取决于你的系统与包管理器：

- Ubuntu:

  ```sh
  apt-get install libaria2-0-dev libquazip5-dev
  ```

- Archlinux:

  ```sh
  pacman -S aria2 quazip
  ```

- Windows (MSYS2):

  ```sh
  pacman -S mingw-w64-x86_64-aria2 mingw-w64-x86_64-quazip
  ```

- MacOS
  
  ```sh
  brew install quazip
  ```
  libaria2：请在[kaniol-lck/aria2 Releases](https://github.com/kaniol-lck/aria2/releases/tag/release-1.36.0) 查看安装说明
  

#### 编译

```bash
qmake && make
```

## 使用

（图中的背景是我的桌面，因为工具栏在KDE上是半透明的）

### 探索 - 下载Mod

你可以从这些网站上浏览并下载mod：

- Curseforge
- Modrinth
- OptiFine
- ReplayMod

![curseforge_browser](images/curseforge_browser_zh.png)

### 本地 - 管理Mod

你可以浏览并管理你的本地mod：

- 更新&回退
- 为mod添加标签
- 批量重命名
- 禁用mod

![local_browser](images/local_browser_zh.png)