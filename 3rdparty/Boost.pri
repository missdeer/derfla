# Find Boost library.

win32-*msvc* : {
    # Try to use qmake variable's value.
    _BOOST_ROOT = $$BOOST_ROOT
    isEmpty(_BOOST_ROOT) {
        # Try to use the system environment value.
        _BOOST_ROOT = $$(BOOST_ROOT)
    }

    isEmpty(_BOOST_ROOT) {
        message(BOOST_ROOT not detected. You may set the environment variable BOOST_ROOT. For example, BOOST_ROOT=/path/to/boost_root, or run qmake with argument: qmake BOOST_ROOT=/path/to/boost_root)
        !build_pass:error(Please set the environment variable `BOOST_ROOT`. For example, BOOST_ROOT=/path/to/boost_root, or run qmake with argument: qmake BOOST_ROOT=/path/to/boost_root)
    } else {
        message(Boost Library detected in $$_BOOST_ROOT)
        INCLUDEPATH += $$_BOOST_ROOT
        TR_EXCLUDE += $$_BOOST_ROOT/*
    }
} else: {
    INCLUDEPATH += /usr/local/include
    TR_EXCLUDE += /usr/local/include/boost/*
    LIBS+=-L/usr/local/lib
}
