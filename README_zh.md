<img width="256" height="256" align="left" style="float: left; margin: 0 10px 0 0;" src="images/modmanager.png" alt="modmanager.png"/><br />

<h1>Mod管理器</h1>

![GitHub top language](https://img.shields.io/github/languages/top/kaniol-lck/modmanager) ![C++17](https://img.shields.io/badge/C%2B%2B-17-%2300599C) ![Qt 5.15.2](https://img.shields.io/badge/Qt-5.15.2-%2341CD52) ![GitHub](https://img.shields.io/github/license/kaniol-lck/modmanager) [![CodeFactor](https://www.codefactor.io/repository/github/kaniol-lck/modmanager/badge)](https://www.codefactor.io/repository/github/kaniol-lck/modmanager) [![Codacy Badge](https://app.codacy.com/project/badge/Grade/4f9ae02df7ba432aab449f9d5bdfabeb)](https://www.codacy.com/gh/kaniol-lck/modmanager/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=kaniol-lck/modmanager&amp;utm_campaign=Badge_Grade)

[English](README.md) | **中文**

一个基于Qt的Minecraft Mod管理器。

它可以管理、更新与下载Mod。

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

### 自行构建

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

![curseforge_browser](images/curseforge_browser_zh.png)