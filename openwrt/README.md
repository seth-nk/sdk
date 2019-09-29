# Seth pppd plugin for OpenWrt

## 安装

### 预购建版本

glider0 提供了预先构建的二进制版本，你只需要下载二进制文件并上传到 OpenWrt。

CPU|示例芯片组|下载
---|---------|----
aarch64_cortex-a53|MediaTek Ralink ARM MT7622|[下载](https://raw.githubusercontent.com/seth-project/sdk/master/openwrt/build/aarch64_cortex-a53/seth-plugin.so)
arm_cortex-a15+neon-vfpv4_eabi|Qualcomm Atheros IPQ806X|[下载](https://seth-project/openwrt-plugin/build/arm_cortex-a15+neon-vfpv4_eabi/seth-plugin.so)
arm_cortex-a7+neon-vfpv4_eabi|Qualcomm Atheros IPQ40XX|[下载](https://seth-project/openwrt-plugin/build/arm_cortex-a7+neon-vfpv4_eabi/seth-plugin.so)
arm_cortex-a9+vfpv3_eabi|Marvell Armada 37x/38x/XP|[下载](https://seth-project/openwrt-plugin/build/arm_cortex-a9+vfpv3_eabi/seth-plugin.so)
mipsel_24kc|MediaTek Ralink MIPS MT7620A/N<br>MediaTek Ralink MIPS MT7621|[下载](https://seth-project/openwrt-plugin/build/mipsel_24kc/seth-plugin.so)
mips_24kc|Atheros AR7xxx/AR9xxx<br>Qualcomm Atheros QCA95XX|[下载](https://seth-project/openwrt-plugin/build/mips_24kc/seth-plugin.so)

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
