cache()

TEMPLATE = subdirs

SUBDIRS +=  \
    util \
    derfla \
    everything \
    lfs \ 
    hashdigest \
    websearch \
    webdict \
    calculator \
    weather \
    donate

derfla.depends = util
lfs.depends = util
everything.depends = util
hashdigest.depends = util
websearch.depends = util
webdict.depends = util
calculator.depends = util
weather.depends = util

RESOURCES += \
    donate/derfla.qrc