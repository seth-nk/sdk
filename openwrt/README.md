# Seth pppd plugin for OpenWrt

Seth 的 OpenWrt 移植由 [glider0](https://github.com/glider0) 贡献！

## 安装

### 预购建版本

我们提供了预先构建的二进制版本，你只需要下载二进制文件并上传到 OpenWrt。

请注意，预构建版本与 musl libc 链接，不适用于其他 libc（如 glibc, uClibc），OpenWrt 默认使用 musl libc。

CPU|pppd 版本 2.4.7|pppd 版本 2.4.8
-|-|-
arm|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/arm/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/arm/seth-plugin.so)
arm_cortex-a5|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/arm_cortex-a5/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/arm_cortex-a5/seth-plugin.so)
arm_cortex-a5_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/arm_cortex-a5_neon-vfpv4/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/arm_cortex-a5_neon-vfpv4/seth-plugin.so)
arm_cortex-a7|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/arm_cortex-a7/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/arm_cortex-a7/seth-plugin.so)
arm_cortex-a7_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/arm_cortex-a7_neon-vfpv4/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/arm_cortex-a7_neon-vfpv4/seth-plugin.so)
arm_cortex-a9|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/arm_cortex-a9/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/arm_cortex-a9/seth-plugin.so)
arm_cortex-a9_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/arm_cortex-a9_neon-vfpv4/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/arm_cortex-a9_neon-vfpv4/seth-plugin.so)
arm_cortex-a15_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/arm_cortex-a15_neon-vfpv4/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/arm_cortex-a15_neon-vfpv4/seth-plugin.so)
arm_cortex-a53_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/arm_cortex-a53_neon-vfpv4/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/arm_cortex-a53_neon-vfpv4/seth-plugin.so)
aarch64_generic|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/aarch64_generic/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/aarch64_generic/seth-plugin.so)
aarch64_cortex-a53|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/aarch64_cortex-a53/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/aarch64_cortex-a53/seth-plugin.so)
aarch64_cortex-a72|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/aarch64_cortex-a72/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/aarch64_cortex-a72/seth-plugin.so)
mips_24kc|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/mips_24kc/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/mips_24kc/seth-plugin.so)
mips_mips32|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/mips_mips32/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/mips_mips32/seth-plugin.so)
mipsel_24kc|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/mipsel_24kc/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/mipsel_24kc/seth-plugin.so)
mipsel_4kec|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/mipsel_4kec/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/mipsel_4kec/seth-plugin.so)
mipsel_74kc|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/mipsel_74kc/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/mipsel_74kc/seth-plugin.so)
mipsel_mips32|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.7/mipsel_mips32/seth-plugin.so)|[下载](https://seth-nk.github.io/openwrt-plugin-builds/pppd2.4.8/mipsel_mips32/seth-plugin.so)


请选择你的设备对应的 CPU，否则可能无法正常执行、导致崩溃或损失性能。

如果你的设备不受支持，欢迎打开 issue 来要求其他 CPU 支持。

OpenWrt 19.07 及更低版本，请使用 pppd 2.4.7 版本

```
scp seth-plugin.so root@192.168.1.1:/usr/lib/pppd/2.4.7/
```

OpenWrt 当前 snapshot 及 19.07 的下一个版本（未发布），请使用 pppd 2.4.8 版本

```
scp seth-plugin.so root@192.168.1.1:/usr/lib/pppd/2.4.8/
```

### 通过源代码构建

将 openwrt/seth 放入 OpenWrt 源码树 `package` 目录下

如果源码树是 OpenWrt 当前 snapshot 及 19.07 的下一个版本（未发布），编辑 `package/seth/Makefile` 文件，将 `_PPPD_VERSION` 改为 2.4.8。如果是 19.07 及以下版本则无需修改。

```
make package/seth/download V=s
make package/seth/compile
```

你可以使用 `-j` 选项以获得更快的构建速度

## 如何使用

前提条件，完成安装

创建 `/etc/seth` 目录

```
mkdir -p /etc/seth
```

将你的数据文件重命名为“你的上网用户名.sth3”形式，放入 `/etc/seth` 目录

设置接口

```
uci set network.wan.proto='pppoe'
uci set network.wan.pppd_options='plugin seth-plugin.so'
uci set network.wan.metric='0'
uci set network.wan.auto='0'
uci set network.wan.username='你的上网用户名'
uci set network.wan.password='你的上网密码'
uci commit network
```

将系统时钟设置为正确的值。使用 ntp 同步时间或者通过 LuCI 界面同步。

使用 LuCI 连接或者使用 `ifup` 命令连接。

## 提示

插件在查找 `/etc/seth` 之前还会先查找 `/tmp/seth`，即 `/tmp/seth` 可以覆盖 `/etc/seth`。`/tmp/seth` 是一个虚拟的目录，会在关机时自动清除。

插件在无法找到数据文件时还可以直接查找 `/tmp/nk-override/用户名` 文件，这是一个一次性覆盖本次拨号的文件，这是对未来 Seth 设想的 RD-API（远程拨号 API）所做的一种准备，目前没有什么用。

在以上多个地方数据文件全部没有、过期，也找不到一次性覆盖时。插件会终结拨号过程。

OpenWrt 可能无法正确识别部分 ISP 的认证失败。此插件做了保护处理，以避免在连接出错时不断重试导致帐号被封禁。
