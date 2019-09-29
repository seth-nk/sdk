# Seth pppd plugin for OpenWrt

## 安装

### 预购建版本

glider0 提供了预先构建的二进制版本，你只需要下载二进制文件并上传到 OpenWrt。

CPU|示例芯片组|下载
---|---------|----
aarch64_cortex-a53|MediaTek Ralink ARM MT7622|[下载](https://raw.githubusercontent.com/seth-project/sdk/master/openwrt/build/aarch64_cortex-a53/seth-plugin.so)
arm_cortex-a15+neon-vfpv4_eabi|Qualcomm Atheros IPQ806X|[下载](https://raw.githubusercontent.com/seth-project/sdkmaster/openwrt/build/arm_cortex-a15+neon-vfpv4_eabi/seth-plugin.so)
arm_cortex-a7+neon-vfpv4_eabi|Qualcomm Atheros IPQ40XX|[下载](https://raw.githubusercontent.com/seth-project/sdkmaster/openwrt/build/arm_cortex-a7+neon-vfpv4_eabi/seth-plugin.so)
arm_cortex-a9+vfpv3_eabi|Marvell Armada 37x/38x/XP|[下载](https://raw.githubusercontent.com/seth-project/sdkmaster/openwrt/build/arm_cortex-a9+vfpv3_eabi/seth-plugin.so)
mipsel_24kc|MediaTek Ralink MIPS MT7620A/N<br>MediaTek Ralink MIPS MT7621|[下载](https://raw.githubusercontent.com/seth-project/sdkmaster/openwrt/build/mipsel_24kc/seth-plugin.so)
mips_24kc|Atheros AR7xxx/AR9xxx<br>Qualcomm Atheros QCA95XX|[下载](https://raw.githubusercontent.com/seth-project/sdkmaster/openwrt/build/mips_24kc/seth-plugin.so)

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


