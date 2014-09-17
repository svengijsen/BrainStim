TEMPLATE = lib
CONFIG += qt plugin
QT += declarative

DESTDIR = ../../../../TimeExample
TARGET  = DefaultPlugin

SOURCES += \
    DefaultPlugin.cpp

qdeclarativesources.files += \
    TimeExample/qmldir \
    TimeExample/center.png \
    TimeExample/clock.png \
    TimeExample/Clock.qml \
    TimeExample/hour.png \
    TimeExample/minute.png

qdeclarativesources.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins/com/nokia/TimeExample

sources.files += plugins.pro DefaultPlugin.cpp DefaultPlugin.qml README
sources.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins
target.path += $$[QT_INSTALL_EXAMPLES]/declarative/cppextensions/plugins/com/nokia/TimeExample

INSTALLS += qdeclarativesources sources target

symbian {
    include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
    TARGET.EPOCALLOWDLLDATA = 1
}
maemo5: include($$QT_SOURCE_TREE/examples/maemo5pkgrules.pri)
