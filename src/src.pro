cache()

TEMPLATE = subdirs

SUBDIRS +=  \
    util \
    derfla \
    everything \
    lfs 

derfla.depends = util
lfs.depends = util
everything.depends = util
