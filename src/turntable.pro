# Copyright (c) 2011-2012 Nokia Corporation.

QT += core gui quick opengl sensors
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
TARGET = turntable
VERSION = 1.4.1

SOURCES += \
    main.cpp \
    drummachine.cpp \
    #mainwindow.cpp \
    turntable.cpp

OTHER_FILES += \
    qml/*.qml \
    qml/Common/*.qml \
    qml/DrumMachine/*.qml \
    qml/HelpScreen/*.qml \
    qml/SampleSelector/*.qml \
    qml/MixerPanel/*.qml \
    qml/Turntable/*.qml

RESOURCES += turntable.qrc

HEADERS += \
    drummachine.h \
    #mainwindow.h \
    turntable.h


win32:!maemo5 {
    TARGET = DjTurntable
    QT += multimedia
    RC_FILE = turntable.rc
}


# Maemo 5 and Harmattan
unix:!symbian {
    # Common
    BINDIR    = /opt/usr/bin
    DATADIR   = /usr/share

    DEFINES  += DATADIR=\\\"$$DATADIR\\\" \
                PKGDATADIR=\\\"$$PKGDATADIR\\\"

    target.path = $$BINDIR

    icon.path = $$DATADIR/icons/hicolor/80x80/apps
    icon.files += icons/80x80/turntable.png

    maemo5 {
        # Maemo 5 specific
        QT        += multimedia
        CONFIG    += mobility
        MOBILITY  += sensors systeminfo

        HEADERS   += accelerometerfilter.h
        SOURCES   += accelerometerfilter.cpp

        OTHER_FILES += qtc_packaging/debian_fremantle/*

        desktop.path = $$DATADIR/applications/hildon
        desktop.files += qtc_packaging/debian_fremantle/$${TARGET}.desktop
    }

    Harmattan {
        # Harmattan specific
        DEFINES += Q_WS_MAEMO_6

        CONFIG += mobility
        MOBILITY += sensors systeminfo

        HEADERS += accelerometerfilter.h
        SOURCES += accelerometerfilter.cpp

        OTHER_FILES += qtc_packaging/debian_harmattan/*

        desktop.path = $$DATADIR/applications
        desktop.files += qtc_packaging/debian_harmattan/$${TARGET}.desktop

        gameclassify.path = /usr/share/policy/etc/syspart.conf.d
        gameclassify.files += qtc_packaging/debian_harmattan/$${TARGET}.conf

        INSTALLS += gameclassify
    }

    INSTALLS += target \
                desktop \
                icon
}


symbian {
    TARGET = DjTurntable
    CONFIG += mobility
    MOBILITY += sensors systeminfo

    HEADERS += accelerometerfilter.h
    SOURCES += accelerometerfilter.cpp

    !contains(SYMBIAN_VERSION, Symbian3) {
        message(Symbian^1)

        DEFINES += Q_OS_SYMBIAN_1

        # In Symbian^1 we don't have OpenGL available
        QT -= opengl
        DEFINES += QT_NO_OPENGL
    }
    else {
        message(Symbian^3)

        # To handle volume up / down keys on Symbian
        LIBS += -lremconcoreapi
        LIBS += -lremconinterfacebase

        # Make the volume louder
        DEFINES += QTGAMEENABLER_USE_VOLUME_HACK

        # Enable hardware floats (speeds up stb vorbis considerably)
        MMP_RULES += "OPTION gcce -march=armv6"
        MMP_RULES += "OPTION gcce -mfpu=vfp"
        MMP_RULES += "OPTION gcce -mfloat-abi=softfp"
        MMP_RULES += "OPTION gcce -marm"
    }

    # For the icon
    ICON = icons/turntable.svg

    # To lock the application to landscape orientation
    LIBS += -lcone -leikcore -lavkon

    TARGET.EPOCHEAPSIZE = 0x100000 0x4000000
    TARGET.EPOCSTACKSIZE = 0x14000
}

UBUNTU_TOUCH {
    message("building for Ubuntu Touch")

    DEFINES += Q_WS_MAEMO_6 #reuse rules for MAEMO OS

    HEADERS += accelerometerfilter.h
    SOURCES += accelerometerfilter.cpp

    target.path = /
    click_files.path = /
    click_files.files = $$PWD/qtc_packaging/ubuntu_touch/*

    INSTALLS+=click_files
}

include(qtgameenabler/qtgameenableraudio.pri)
