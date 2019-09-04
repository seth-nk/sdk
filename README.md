# The-Seth-Project 官方 SDK 仓库

**此仓库未正式完成。SDK 的内容可能发生破坏性改变。**

**此仓库正在过渡时期，软件将发生颠覆性不兼容改变，但仓库中存在新旧代码可能较为混乱。且在未来可能会发生破坏性改变。**

## 关于 Seth 服务

见 [此处](https://tienetech.tk/seth/)

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

- `desktop` 在桌面平台（如 Unix、Linux 和 Windows）上使用 Seth 服务的工具 demo。见 desktop 目录下的 README 文件。

- `openwrt` 在 OpenWrt 上使用 Seth 服务的包。请参见 openwrt 目录下的 README 文件。

## 我们与 NSWA Ranga 的关系

NSWA Ranga 的开发者 NSWA Maintainers （或 NSWA Ranga Maintainers）是我们的合作伙伴。此仓库的部分代码使用了 NSWA Maintainers 贡献的代码。

Seth 数据的计算这一难点的解决，部分来自 NSWA Maintainers 的努力。

NSWA Ranga 产品内置了修改版本的 Seth 代码以使用 Seth 服务。

## “Seth” 来源于什么

在 The-Seth-Project 开发者的童年回忆 “Yu-Gi-Oh!” 系列动画片中，角色“海马濑人”的前世 “セト”（Seto Kaiba）的原型为古埃及神话的风暴之神“stẖ”，“stẖ”的一种英文翻译为“Seth”，再翻译成中文为“塞斯”。

维基百科：https://en.wikipedia.org/wiki/List_of_Yu-Gi-Oh!_characters#Seto_Kaiba 和 https://en.wikipedia.org/wiki/Set_(deity)

## 版权和免责声明

Copyright (R) 2019 NSWA Maintainers

Copyright (R) 2019 The-Seth-Project

License: MIT, with additional license grant original maintainers unlimited rights for this code and the derived codes which are contributed to upstream.

This is free software; you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law.
