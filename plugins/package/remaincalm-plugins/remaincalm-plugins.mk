######################################
#
# remaincalm-plugins
#
######################################

REMAINCALM_PLUGINS_VERSION = 3d0f4656bafc86559ca2fcd840479ee3fca27bd2
REMAINCALM_PLUGINS_SITE = $(call github,remaincalm,dpfports,$(REMAINCALM_PLUGINS_VERSION))
REMAINCALM_PLUGINS_BUNDLES = floaty.lv2 mud.lv2

REMAINCALM_PLUGINS_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) NOOPT=true

define REMAINCALM_PLUGINS_BUILD_CMDS
	(cd $(@D) && \
		[ ! -d dpf ] && \
		git clone git://github.com/DISTRHO/DPF dpf --depth=1 && \
		ln -sf ../dpf floaty/dpf && \
		ln -sf ../dpf mud/dpf || true)

	$(REMAINCALM_PLUGINS_TARGET_MAKE) -C $(@D)/floaty/source
	$(REMAINCALM_PLUGINS_TARGET_MAKE) -C $(@D)/mud/source
endef

define REMAINCALM_PLUGINS_INSTALL_TARGET_CMDS
	install -d $(TARGET_DIR)/usr/lib/lv2
	cp -rL $(@D)/floaty/source/build/floaty.lv2 $(TARGET_DIR)/usr/lib/lv2/
	cp -rL $(@D)/mud/source/build/mud.lv2 $(TARGET_DIR)/usr/lib/lv2/
endef

$(eval $(generic-package))