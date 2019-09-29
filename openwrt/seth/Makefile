# 
# copyright 2019 glider0
# License: MIT
# This is free software with ABSOLUTELY NO WARRANTY.
# 

include $(TOPDIR)/rules.mk

PKG_NAME:=seth
PKG_VERSION:=1.0
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/seth-project/sdk.git
PKG_SOURCE_VERSION:=cf391b2f25eeb0039ced75319ca10859626baf6a
PKG_MIRROR_HASH:=skip

include $(INCLUDE_DIR)/package.mk

define Package/seth
  SECTION:=extra
  CATEGORY:=Extra packages
  DEPENDS:=+pppd
  TITLE:=Seth pppd plugin
endef

define Package/seth/description
  Seth pppd plugin. It can override username once or read user name from seth data file store in flash or tmpfs.
endef

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) -fPIC -shared -o "$(PKG_BUILD_DIR)/seth.so" $(TARGET_LDFLAGS) "$(PKG_BUILD_DIR)/main.c"
endef

define Package/seth/install
	$(INSTALL_DIR) $(1)/usr/lib/pppd/2.4.7/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/seth.so $(1)/usr/lib/pppd/2.4.7/
	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) sethutil $(1)/usr/bin/
endef

$(eval $(call BuildPackage,seth))
