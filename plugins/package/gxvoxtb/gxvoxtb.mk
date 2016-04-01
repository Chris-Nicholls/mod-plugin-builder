######################################
#
# gxvoxtb
#
######################################

GXVOXTB_VERSION = 9ff36d0daff8fce475074ff50167a9d4205adb37
GXVOXTB_SITE = $(call github,moddevices,gx_vobtb.lv2,$(GXVOXTB_VERSION))
GXVOXTB_BUNDLES = 

GXVOXTB_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) SSE_CFLAGS="" -C $(@D)

define GXVOXTB_BUILD_CMDS
	$(GXVOXTB_TARGET_MAKE)
endef

define GXVOXTB_INSTALL_TARGET_CMDS
	$(GXVOXTB_TARGET_MAKE) install DESTDIR=$(TARGET_DIR) INSTALL_DIR=/usr/lib/lv2
	cp -rL $($(PKG)_PKGDIR)/gx_voxtb.lv2/* $(TARGET_DIR)/usr/lib/lv2/gx_voxtb.lv2/
endef

$(eval $(generic-package))
