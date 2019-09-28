# The-Seth-Project 官方 SDK 仓库

## 关于 Seth 服务

请转到 [此处](https://seth-project.tk/)

## 构建并安装 libseth

```
cd libseth
make
sudo make install
```

编译后产生的结果和需要安装的文件有

- `libseth.so.0(.*)` - Seth 数据格式库

- `libseth.dll` - Seth 数据格式库（Windows）

- `libseth-static.a` - Seth 数据格式库（静态版本）

- `seth-dumpmeta(-sls)` 用于显示指定数据文件的元数据的命令行实现示例

- `seth-nkgetpin(-sls)` 用于从 Seth 数据文件读取指定时间的 NK PIN 的命令行实现示例

- `seth-genhdr(-sls)` 用于接受命令行参数然后生成 Seth 数据头部的命令行实现示例

- `seth-split(-sls)` 用于接受命令行参数然后从一个数据文件中抽出用户指定的时间段部分生成新的数据文件的命令行实现示例

- `seth.h` - Seth 数据格式库头文件

- `nkget.h` - Seth 数据格式库头文件

> -sls 后辍的版本表示 **S**tatic **L**inked lib**S**eth，表示与 `libseth-static.a` 而不是 `libseth.so.0` 连接。

## seth-desktop 构建

需要先构建并安装 libseth，然后参考 `desktop` 目录下的 [README 文件](desktop/README.md)进行操作。

使用方法参考 `desktop` 目录下的 [README 文件](desktop/README.md)中的说明。

## Windows 版本

我们建议你使用[预先构建的 Windows 二进制版本](https://github.com/seth-project/sdk/releases/)，如果一定要自己编译，则 Windows 构建需要在类 UNIX 系统上运行 wine 进行，并在其中安装，MinGW 和 GTK+3 for Windows，请阅读 `wine_compile.sh` 获取更多信息。在纯 Windows 环境上构建应该是可行的，请自行研究。

## 平台

- `desktop` 在桌面平台（如 Unix、Linux 和 Windows）上使用 Seth 数据文件并拨号的程序，他具有多种后端（RASAPI、RP-PPPoE、RP-PPPoE-Linux、nmcli）和多种前端（命令行、GTK+ 图形界面）。见 desktop 目录下的 README 文件。

- `openwrt` 在 OpenWrt 上使用 Seth 数据文件的包。请参见 openwrt 目录下的 README 文件。

## “Seth” 项目是谁在运作？

 The-Seth-Project 是由一群计算机技术爱好者组成的社区，其目的旨在帮助全部的校园宽带用户自由地接入他们的网络，特别是保障那些使用 GNU/Linux 和其他不被供应商支持的作业系统的用户的权益，但不论你使用什么作业系统，我们都仍然爱你。我们项目的名字 “Seth” 来自[古埃及神话中的风暴和沙漠之神 “stẖ”](https://en.wikipedia.org/wiki/Set_(deity))，“stẖ” 的一种英文翻译为“Seth”，再翻译成中文为“塞斯”。 

## 版权和免责声明

Copyright (R) 2019 [The-Seth-Project and contributors](CREDITS))

License: MIT, with additional license grant original maintainers unlimited rights for this code and the derived codes which are contributed to upstream.

This is free software; you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law.
