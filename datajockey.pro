#datajockey!

#edit this for your system!
PREFIX = /usr/local/


TEMPLATE = app
TARGET = datajockey
DEPENDPATH += .
INCLUDEPATH += include/
INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /usr/lib/ruby/1.8/i486-linux/
INCLUDEPATH += /usr/local/include/oscpack/

target.path = $$PREFIX/bin
INSTALLS += target

QT += sql
LIBS += -ljackcpp `pkg-config --libs jack` -lsndfile -lslv2 -lmad -lvorbisfile -lruby1.8 -loscpack -lboost_regex -lyamlcpp -lsyck
QMAKE_LFLAGS += -rdynamic

MOC_DIR = moc
OBJECTS_DIR = objects

swigtarget.target = swig/datajockey.so
swigtarget.commands = cd swig && ruby extconf.rb && make
swigtarget.depends = swig/*.i swig/scriptcallbackfilter.cpp

swigutilstarget.target = utils-swig/datajockey_utils.so
swigutilstarget.commands = cd utils-swig/ && ruby extconf.rb && make
swigutilstarget.depends = utils-swig/*.i

install_ruby.target = install_ruby
install_ruby.depends = $$swigutilstarget.target $$swigtarget.target
install_ruby.commands = cd utils-swig/ && make install && cd ../swig/ && make install

install_all.target = install_all
install_all.depends = $$install_ruby install

QMAKE_EXTRA_TARGETS += swigtarget 
QMAKE_EXTRA_TARGETS += swigutilstarget 
QMAKE_EXTRA_TARGETS += install_ruby 
QMAKE_EXTRA_TARGETS += install_all 

POST_TARGETDEPS += $$swigtarget.target
POST_TARGETDEPS += $$swigutilstarget.target

CONFIG += debug qt

# Input
HEADERS +=  include/application.hpp \
				include/applicationmodel.hpp \
				include/applicationview.hpp \
				include/audiodriver.hpp \
				include/audioio.hpp \
				include/buffer.hpp \
				include/bufferplayer.hpp \
				include/command.hpp \
				include/crossfademodel.hpp \
				include/crossfadeview.hpp \
				include/defaultworkfilters.hpp \
				include/djmixerchannelmodel.hpp \
				include/djmixerchannelview.hpp \
				include/djmixercontrolmodel.hpp \
				include/djmixercontrolview.hpp \
				include/djmixerworkinfoview.hpp \
				include/eqmodel.hpp \
				include/eqview.hpp \
				include/mastermodel.hpp \
				include/masterview.hpp \
				include/mixerchannelmodel.hpp \
				include/mixerchannelview.hpp \
				include/mixerpanelmodel.hpp \
				include/mixerpanelview.hpp \
				include/object.hpp \
				include/oscreceiver.hpp \
				include/interpretermodel.hpp \
				include/interpreterioproxy.hpp \
				include/interpreterview.hpp \
				include/remoteworkfiltermodel.hpp \
				include/soundfile.hpp \
				include/tageditor.hpp \
				include/tagmodel.hpp \
				include/tagview.hpp \
				include/tempo.hpp \
				include/treeitem.h \
				include/treemodel.h \
				include/utilities.hpp \
				include/workdbview.hpp \
				include/workdetailview.hpp \
				include/workfilterlist.hpp \
				include/workfilterlistview.hpp \
				include/workfiltermodel.hpp \
				include/workloader.hpp \
				include/worktablemodel.hpp \
				include/worktagmodelfilter.hpp 

SOURCES +=  src/application.cpp \
				src/main.cpp \
				src/applicationmodel.cpp \
				src/applicationview.cpp \
				src/audiodriver.cpp \
				src/audioio.cpp \
				src/buffer.cpp \
				src/bufferplayer.cpp \
				src/command.cpp \
				src/crossfademodel.cpp \
				src/crossfadeview.cpp \
				src/defaultworkfilters.cpp \
				src/djmixerchannelmodel.cpp \
				src/djmixerchannelview.cpp \
				src/djmixercontrolmodel.cpp \
				src/djmixercontrolview.cpp \
				src/djmixerworkinfoview.cpp \
				src/eqmodel.cpp \
				src/eqview.cpp \
				src/mastermodel.cpp \
				src/masterview.cpp \
				src/mixerchannelmodel.cpp \
				src/mixerchannelview.cpp \
				src/mixerpanelmodel.cpp \
				src/mixerpanelview.cpp \
				src/object.cpp \
				src/oscreceiver.cpp \
				src/interpretermodel.cpp \
				src/interpreterioproxy.cpp \
				src/interpreterview.cpp \
				src/remoteworkfiltermodel.cpp \
				src/soundfile.cpp \
				src/tageditor.cpp \
				src/tagmodel.cpp \
				src/tagview.cpp \
				src/tempo.cpp \
				src/treeitem.cpp \
				src/treemodel.cpp \
				src/utilities.cpp \
				src/workdbview.cpp \
				src/workdetailview.cpp \
				src/workfilterlist.cpp \
				src/workfilterlistview.cpp \
				src/workfiltermodel.cpp \
				src/workloader.cpp \
				src/worktablemodel.cpp \
				src/worktagmodelfilter.cpp 

