<img width="256" height="256" align="left" style="float: left; margin: 0 10px 0 0;" src="images/modmanager.png" alt="modmanager.png"/><br />

<h1>Mod管理器</h1>

![GitHub top language](https://img.shields.io/github/languages/top/kaniol-lck/modmanager) ![C++17](https://img.shields.io/badge/C%2B%2B-17-%2300599C) ![Qt 5.15.2](https://img.shields.io/badge/Qt-5.15.2-%2341CD52) ![GitHub](https://img.shields.io/github/license/kaniol-lck/modmanager) [![CodeFactor](https://www.codefactor.io/repository/github/kaniol-lck/modmanager/badge)](https://www.codefactor.io/repository/github/kaniol-lck/modmanager)  [![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/kaniol-lck/modmanager?include_prereleases)](Changelog.md) [![GitHub all releases](https://img.shields.io/github/downloads/kaniol-lck/modmanager/total)](https://github.com/kaniol-lck/modmanager/releases)

[English](README.md) | **中文**

一个基于Qt的Minecraft Mod管理器，可以管理、更新与下载Mod。

## License

Mod管理器以[GPLv3](LICENSE)许可证发布。

## 构建

本项目已经配置了[GitHub Actions](https://github.com/kaniol-lck/modmanager/actions)，如果你想要尝试最新鲜的bug，可以直接在其中选择下载并帮我捉虫。

### 构建状态

| 平台          | 状态                                                         |
| ------------- | ------------------------------------------------------------ |
| Windows MinGW | [![windows](https://github.com/kaniol-lck/modmanager/actions/workflows/windows-mingw.yml/badge.svg)](https://github.com/kaniol-lck/modmanager/blob/master/.github/workflows/windows-mingw.yml) |
| Windows MSVC  | [![windows](https://github.com/kaniol-lck/modmanager/actions/workflows/windows-msvc.yml/badge.svg)](https://github.com/kaniol-lck/modmanager/blob/master/.github/workflows/windows-msvc.yml) |
| Linux         | [![ubuntu](https://github.com/kaniol-lck/modmanager/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/kaniol-lck/modmanager/blob/master/.github/workflows/ubuntu.yml) |
| Mac OS        | *未知\**                                                     |

\* Mac OS的Github Actions构建在C++ 17的部分失败了（我也不知道是不是原先用的actions写的有问题）。

### 从源码构建

克隆本仓库至本地：

```bash
git clone git@github.com:kaniol-lck/modmanager.git
```

#### 对于GCC/MinGW

构建：

```bash
qmake && make
```

#### 对于MSVC

安装zlib（详见[zlib.install](https://github.com/horta/zlib.install)）：

```bash
powershell -Command "(Invoke-WebRequest -Uri https://git.io/JnHTY -OutFile install_zlib.bat)"; ./install_zlib.bat; del install_zlib.bat
```

构建：

```bash
qmake && nmake
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