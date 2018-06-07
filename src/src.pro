greaterThan(QT_MAJOR_VERSION, 4) : {
    cache()
}

TEMPLATE = subdirs

SUBDIRS +=  \
    util \
    derfla \
    lfs \
    everything

derfla.depends = util
lfs.depends = util
everything.depends = util
