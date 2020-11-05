# Seth pppd plugin for OpenWrt

Seth 的 OpenWrt 移植由 [glider0](https://github.com/glider0) 贡献！

## 安装

### 预购建版本

我们提供了预先构建的二进制版本，你只需要下载二进制文件并上传到 OpenWrt。

CPU|下载（与 musl libc 链接）
-|-
arm|[下载](https://seth-nk.github.io/openwrt-plugin-builds/arm/seth-plugin.so)
arm_cortex-a5|[下载](https://seth-nk.github.io/openwrt-plugin-builds/arm_cortex-a5/seth-plugin.so)
arm_cortex-a5_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/arm_cortex-a5_neon-vfpv4/seth-plugin.so)
arm_cortex-a7|[下载](https://seth-nk.github.io/openwrt-plugin-builds/arm_cortex-a7/seth-plugin.so)
arm_cortex-a7_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/arm_cortex-a7_neon-vfpv4/seth-plugin.so)
arm_cortex-a9|[下载](https://seth-nk.github.io/openwrt-plugin-builds/arm_cortex-a9/seth-plugin.so)
arm_cortex-a9_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/arm_cortex-a9_neon-vfpv4/seth-plugin.so)
arm_cortex-a15_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/arm_cortex-a15_neon-vfpv4/seth-plugin.so)
arm_cortex-a53_neon-vfpv4|[下载](https://seth-nk.github.io/openwrt-plugin-builds/arm_cortex-a53_neon-vfpv4/seth-plugin.so)
aarch64_cortex-a53|[下载](https://seth-nk.github.io/openwrt-plugin-builds/aarch64_cortex-a53/seth-plugin.so)
aarch64_cortex-a72|[下载](https://seth-nk.github.io/openwrt-plugin-builds/aarch64_cortex-a72/seth-plugin.so)
aarch64_generic|[下载](https://seth-nk.github.io/openwrt-plugin-builds/aarch64_generic/seth-plugin.so)
mips_24kc|[下载](https://seth-nk.github.io/openwrt-plugin-builds/mips_24kc/seth-plugin.so)
mips_mips32|[下载](https://seth-nk.github.io/openwrt-plugin-builds/mips_mips32/seth-plugin.so)
mipsel_24kc|[下载](https://seth-nk.github.io/openwrt-plugin-builds/mipsel_24kc/seth-plugin.so)
mipsel_4kec|[下载](https://seth-nk.github.io/openwrt-plugin-builds/mipsel_4kec/seth-plugin.so)
mipsel_74kc|[下载](https://seth-nk.github.io/openwrt-plugin-builds/mipsel_74kc/seth-plugin.so)


请选择你的设备对应的 CPU，否则可能无法正常执行、导致崩溃或损失性能。

如果你的设备不受支持，欢迎打开 issue 来要求其他 CPU 支持。

```
scp seth-plugin.so root@192.168.1.1:/usr/lib/pppd/2.4.7/
```

### 通过源代码构建

将 openwrt/seth 放入 OpenWrt 源码树 `package` 目录下

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
