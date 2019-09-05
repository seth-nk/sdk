# Seth Remote Dialup API (RD-API (TM))（草案）

RD-API 是设计用于控制远程系统拨号的 Web API。远程系统存储空间可能不足以存储 Seth 数据文件，因此此 API 应运而生，并设计为通用目的。

**这是一个预先部署到生产环境的草案，在未来可能发生破坏性改变**

## 获取 Seth RD-API (TM) Compat 认证

通过我们的 Seth RD-API Compat 认证的产品可以在包装、软件界面和广告宣传中声称“与 Seth 兼容”，并使用 RD-API (TM) 兼容性标签。

![](rdapi-compat-img.png)

要提交认证，请打开 issues 与我们联系。

## 接口

### GET /rdapi-server-profile

返回如下数据

```
RDAPI-Server-Version=1
RDAPI-X-Platforms-Impl=Standard
RDAPI-VND-Impl=
RDAPI-GI=/cgi-bin/rdapi
X-UNIX-Iface=False
```

RDAPI-Server-Version 指定 RD-API 服务器的版本号，目前为 1

RDAPI-X-Platforms-Impl 指定可以使用的扩展字段，如 `Standard;UNIX;Linux` 表示可以使用 `X-UNIX-*`, `X-Linux-*`。

RDAPI-VND-Impl 指定可以使用的 VND 字段，如 `ABC;EFG` 表示可以使用 `VND-ABC-*` 和 `VND-EFG-*`

RDAPI-GI 指定之后接口的 `${GI_PREFIX}`

X-UNIX-Iface 为 `True` 表示只能使用 `X-UNIX-*` 扩展而不是 `Management-Entry` 等。客户端实现若忽略这一点将导致服务器连接失败。

### POST ${GI_PREFIX}/stat

### POST ${GI_PREFIX}/dialup

### POST ${GI_PREFIX}/hangup

## 参数

### Management-Entry

必须指定：stat dialup hangup

可选指定：

受网络管理器记录的 Entry，往往是一个“连接”，例如受 NetworkManager 的 `connection`，OpenWrt netifd `/etc/config/network` 中的一个 interface，Windows 的一个网络连接，等等。他往往和低层次 Iface 接口名字上有关系，但并不是绝对的。后者请参考 `X-UNIX-Link-Iface` 和 `X-UNIX-PPP-Iface`

### Auth-Username

必须指定：dialup

可选指定：stat hangup

用于认证的用户名，如 PAP 用户名。

### Auth-Password

必须指定：dialup

可选指定：stat hangup

用于认证的密码，如 PAP 密码。

### Service-Name

目前未实现，字段保留

### LCP-Options

目前未实现，字段保留

### X-UNIX-Link-Iface

必须指定：

可选指定：dialup stat hangup

必须与谁同时指定：X-UNIX-PPP-Iface

将会覆盖谁的意义：Management-Entry

指定要在哪个 Iface （Netdev） 上创建 PPPoE，如 `eth0`

### X-UNIX-PPP-Iface

必须指定：

可选指定：dialup stat hangup

必须与谁同时指定：X-UNIX-Link-Iface

将会覆盖谁的意义：Management-Entry

指定要创建的点对点 Iface （Netdev） 的名字，如 `ppp0`

### X-Linux-RPPPPoE-Kernel-Mode

目前未实现，字段保留

### X-UNIX-EXTRA-PPPD-Option

目前未实现，字段保留

### X-MSWin-RASAPI-Option-Mask

目前未实现，字段保留

### X-MSWin-Encryption

目前未实现，字段保留

### X-MSWin-Enable-IPv6

目前未实现，字段保留

### X-OpenWrt-Netifd-Retry-Method

目前未实现，字段保留

### VND-{VND_NAME}-*

实现特定扩展，如一家叫 “ABC” 的厂商可以实现 `VND-ABC-Eat-Food` 字段

## POST 方法接口的返回

### Connection-Stat

类似于这种格式 `EntryName=True;EntryName=True;EntryName=False;`，True 表示指定的 EntryName 是连接状态。

### X-UNIX-Iface-Stat

将会覆盖谁的意义：Connection-Stat

类似于这种格式 `PPPIface(LinkIface)=True;PPPIface(LinkIface)=True;PPPIface(LinkIface)=False;`，True 表示指定的 PPP 接口是连接状态

实现必须同时提供 `Connection-Stat`，客户端可以不支持 `X-UNIX-*` 扩展而转而使用 `Connection-Stat`，`EntryName` 应为 `PPPIface`

### Iface-TX-Bytes

目前未实现，字段保留

### Iface-RX-Bytes

目前未实现，字段保留

### Success

操作是否成功，`True` 表示成功，否则表示失败

### VND-{VND_NAME}-*

实现特定扩展，如一家叫 “ABC” 的厂商可以实现 `VND-ABC-I-Am-A-Teapot` 字段
