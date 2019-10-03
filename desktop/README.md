# Seth Desktop-CLI

## 使用说明

### GTK 模式

运行 `sethdkt-gtk`（UNIX）或 `sethdkt-gtkWL.exe`（Windows）（由于 [Windows 的技术缺陷](https://stackoverflow.com/a/230425/10299279)，无法直接运行 `sethdkt-gtk.exe`, `sethdkt-gtkWL.exe` 是一个简单的包装器，会预先准备环境后执行 `sethdkt-gtk.exe`），将 Seth 数据文件放入提示的目录，然后输入密码点击“连接”。在高级选项中可以设置部分参数。见 [配置文件](sethcli.conf.in)。常用的是 UNIX 平台的 `pppoe.interface=XXX` 配置用于指定接口。

### CLI 交互模式

直接运行 `sethdkt-cli`（UNIX）或 `sethdkt-cli.exe`（Windows）

```
$ sethdkt-cli
```

第一次运行时，会提示已在某个目录生成了 [配置文件](sethcli.conf.in)，你需要编辑它，通常情况下你需要修改 `seth.filename` 为数据文件路径（绝对或相对），`pap.password` 为密码。在 UNIX 上，你还需要注意 `pppoe.interface` 应为你的拨号接口，默认为 `eth0`，此外，对于桌面 UNIX 用户，我们强烈建议将 `pppoe.backend` 改为 `nmcli` 从而可以无需 root 运行且可以和桌面环境完美集成，非桌面用户（无头计算机，如软路由）保持 `rp-pppoe` 后端即可，一个例外是 Linux 用户，Linux 上有一个内核模式的 PPPoE 实现，性能比典型的 UNIX 用户模式 `rp-pppoe` 更好，如需使用，将 `pppoe.backend` 改为 `rp-pppoe-linux` 。

### CLI 脚本模式

sethdkt-cli 可以以脚本模式工作

- -NoConfigFileCreation 抑制配置文件创建

- -LoadPlatformDefaultConfig 不要从文件中读取配置，使用当前平台的默认配置

- -OverrideConfig 无条件覆盖某一个配置

例如

```
$ sethdkt-cli -NoConfigFileCreation -LoadPlatformDefaultConfig -OverrideConfig pap.password 233333 -OverrideConfig seth.filename /path/to/datafile.sth3 
```

## 从源码构建

### 依赖

```
libseth（必须，来自 Seth SDK，请自行构建并安装）
libglib2-0（必须）
libnm-dev （如果启用了 nmcli 后端）
libgtk-3-dev（如果构建 gtk 前端）
WSL（如果构建 Windows 目标，由 MinGW 提供）
```

### 构建与安装

只需要运行 `make` 进行构建，但你需要 GNU Make，因为 Makefile 使用了 GNU Makefile 扩展。因此在 BSD 系统上应该使用 `devel/gmake` 包

```
$ make
$ sudo make install
```

## Windows 版本

我们建议你使用[预先构建的 Windows 二进制版本](https://github.com/seth-project/sdk/releases/)，如果一定要自己编译，则 Windows 构建需要在类 UNIX 系统上运行 wine 进行，并在其中安装，MinGW 和 GTK+3 for Windows，请阅读 `wine_compile.sh` 获取更多信息。在纯 Windows 环境上构建应该是可行的，请自行研究。

Windows 构建还需要依赖外部的代码，请使用以下命令完成同步或使用 `--recursive` 选项克隆存储库。（如果你不需要构建 Windows 版本，则不需要进行）

```
git submodule init
git submodule update
```

## 法律信息备注

Seth Desktop 可以与某些 GPL 库进行连接，当你链接到这些 GPL 库时，你将受到 GPL 限制！

GPL 库：

- **libnm**：如果不希望链接到 libnm，你可以不编译 nmcli 后端，这会使 Seth Desktop 失去部分功能。编辑 `configs/posix.mk` 文件，并删除 `CONFIG_BACKEND_NMCLI := y`
