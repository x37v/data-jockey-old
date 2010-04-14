#edit this for your system!
PREFIX = /usr/local/

#change to release if you want to be optimized
CONFIG += qt debug
QT += sql

RESOURCES = ../datajockey.qrc

TEMPLATE = app
TARGET = datajockey_annotator
DEPENDPATH += ../
INCLUDEPATH += ../include/
INCLUDEPATH += /usr/local/include/

LIBS += -lyamlcpp -lsyck -lboost_program_options-mt

target.path = $$PREFIX/bin
INSTALLS += target

MOC_DIR = ../moc
OBJECTS_DIR = ../objects

# Input
HEADERS +=  \
				../include/annotator.hpp \
				../include/config.hpp \
				../include/object.hpp \
				../include/tageditor.hpp \
				../include/tagmodel.hpp \
				../include/tagview.hpp \
				../include/treeitem.h \
				../include/treemodel.h \
				../include/workdbview.hpp \
				../include/workdetailview.hpp \
				../include/workfiltermodel.hpp \
				../include/worktablemodel.hpp \
				../include/worktagmodelfilter.hpp

SOURCES +=  \
				../src/config.cpp \
				../src/object.cpp \
				../src/tageditor.cpp \
				../src/tagmodel.cpp \
				../src/tagview.cpp \
				../src/treeitem.cpp \
				../src/treemodel.cpp \
				../src/workdbview.cpp \
				../src/workdetailview.cpp \
				../src/workfiltermodel.cpp \
				../src/worktablemodel.cpp \
				../src/worktagmodelfilter.cpp \
				../src/annotator.cpp

