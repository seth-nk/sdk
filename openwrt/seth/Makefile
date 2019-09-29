# 
# copyright 2019 glider0
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

include $(TOPDIR)/rules.mk

PKG_NAME:=seth
PKG_VERSION:=20190928
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/seth-project/sdk.git
PKG_SOURCE_DATE:=2019-09-28
PKG_SOURCE_VERSION:=cf391b2f25eeb0039ced75319ca10859626baf6a
PKG_MIRROR_HASH:=7367b31e680e18a14f318389e810a434404b01eda88affc6ccfc71d83e4371e2

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += -fPIC -shared

define Package/seth
  SECTION:=extra
  CATEGORY:=Extra packages
  DEPENDS:=+rp-pppoe-common
  TITLE:=Seth pppd plugin
endef

define Package/seth/description
  Seth pppd plugin. It can override username once or read user name from seth data file store in flash or tmpfs.
endef

define Build/Compile
	$(MAKE) -C "$(PKG_BUILD_DIR)/libseth" CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" LDFLAGS="$(TARGET_LDFLAGS)" libseth-static.a
	$(TARGET_CC) $(TARGET_CFLAGS) -o "$(PKG_BUILD_DIR)/seth-plugin.so" $(TARGET_LDFLAGS) -I"$(PKG_BUILD_DIR)/libseth/" "$(PKG_BUILD_DIR)/main.c" "$(PKG_BUILD_DIR)/libseth/libseth-static.a"
endef

define Package/seth/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/2.4.7/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/seth-plugin.so $(1)/usr/lib/pppd/2.4.7/
endef

$(eval $(call BuildPackage,seth))
