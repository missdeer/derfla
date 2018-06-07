greaterThan(QT_MAJOR_VERSION, 4) : {
    cache()
}

TEMPLATE = subdirs

SUBDIRS += \
    derfla \
    lfs \
    everything
