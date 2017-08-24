######################################
#
# eg-chris
#
######################################

# where to find the source code - locally in this case
EG_CHRIS_SITE_METHOD = local
EG_CHRIS_SITE = $($(PKG)_PKGDIR)/

# even though this is a local build, we still need a version number
# bump this number if you need to force a rebuild
EG_CHRIS_VERSION = 1

# dependencies (list of other buildroot packages, separated by space)
EG_CHRIS_DEPENDENCIES =

# LV2 bundles that this package generates (space separated list)
EG_CHRIS_BUNDLES = eg-chris.lv2

# call make with the current arguments and path. "$(@D)" is the build directory.
EG_CHRIS_TARGET_MAKE = $(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/source


# build command
define EG_CHRIS_BUILD_CMDS
	$(EG_CHRIS_TARGET_MAKE)
endef

# install command
define EG_CHRIS_INSTALL_TARGET_CMDS
	$(EG_CHRIS_TARGET_MAKE) install DESTDIR=$(TARGET_DIR)
endef


# import everything else from the buildroot generic package
$(eval $(generic-package))
