cache()

TEMPLATE = subdirs

SUBDIRS +=  \
    util \
    derfla \
    lfs 

derfla.depends = util
lfs.depends = util

win32: {
	SUBDIRS += everything
    everything.depends = util
}
