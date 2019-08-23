# Seth Desktop-CLI

## 从源码构建

### 依赖

```
libseth（请自行构建并安装）
libglib2-0
libnm-dev （如果启用了 nmcli 后端）
```

### 构建与安装

只需要运行 `make` 进行构建，但你需要 GNU Make，因为 Makefile 使用了 GNU Makefile 扩展。因此在 BSD 系统上应该使用 `devel/gmake` 包

```
$ make
$ make install
```

## Windows 构建

Windows 构建需要在类 UNIX 系统上运行 wine 进行，并在其中安装，Mingw 和 GTK+3 for Windows，请阅读 `wine_compile.sh` 获取更多信息。在纯 Windows 环境上构建应该是可行的，请自行研究。

## 法律信息备注

Seth Desktop 可以与某些 GPL 库进行连接，当你链接到这些 GPL 库时，你将受到 GPL 限制！

GPL 库：

- **libnm**：如果不希望链接到 libnm，你可以不编译 nmcli 后端，这会使 Seth Desktop 失去部分功能。编辑 `configs/posix.mk` 文件，并删除 `CONFIG_BACKEND_NMCLI := y`
