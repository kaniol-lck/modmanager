# Mod管理器

[English](README.md) | **中文**

一个基于Qt的Minecraft Mod管理器。

它可以管理、更新与下载Mod。

## 构建

需要C++17。

```sh
qmake && make
```

### 构建状态

| 平台    | 状态                                                         |
| ------- | ------------------------------------------------------------ |
| Windows | [![windows-mingw](https://github.com/kaniol-lck/modmanager/actions/workflows/windows-mingw.yml/badge.svg)](https://github.com/kaniol-lck/modmanager/blob/master/.github/workflows/windows-mingw.yml) |
| Linux   | [![ubuntu](https://github.com/kaniol-lck/modmanager/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/kaniol-lck/modmanager/blob/master/.github/workflows/ubuntu.yml) |
| Mac OS  | *未知\**                                                     |

\* Mac OS的Github Actions构建在C++ 17的部分失败了（我也不知道是不是原先用的actions写的有问题）。

## 使用

![curseforge_browser](images/curseforge_browser_zh.png)