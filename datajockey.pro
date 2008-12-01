#datajockey!

#edit this for your system!
PREFIX = /usr/local/
RUBYLIBDEST = $$system("ruby -e 'require \"rbconfig\"; puts File.join(Config::CONFIG[\"sitelibdir\"].chomp, \"datajockey/\")'")

#change to release if you want to be optimized
CONFIG += qt debug
#CONFIG += qt release

TEMPLATE = app
TARGET = datajockey
DEPENDPATH += .
INCLUDEPATH += include/
INCLUDEPATH += /usr/local/include/
INCLUDEPATH += /usr/lib/ruby/1.8/i486-linux/
INCLUDEPATH += /usr/local/include/oscpack/
macx {
	INCLUDEPATH += /opt/local/include/
	INCLUDEPATH += /opt/local/lib/ruby/1.8/i686-darwin9/
}

#fast math should get rid of denormal problems [with SSE enabled processors]
QMAKE_CFLAGS += -ffast-math
QMAKE_CXXFLAGS += -ffast-math
RESOURCES = datajockey.qrc

target.path = $$PREFIX/bin
INSTALLS += target

QT += sql

#for some reason linking order matters here, ruby won't require 'rubygems' otherwise
#i haven't figured out the culprit yet, but this works so i'm doing it this way! 

LIBS += -ljackcpp -lsndfile -lslv2 -lmad -lvorbisfile -loscpack 

unix:!macx {
	LIBS += -lruby1.8 -lboost_regex 
}
macx {
	LIBS += -lruby.1.8 -lboost_regex-mt 
}

LIBS += -lyamlcpp -lsyck $$system("pkg-config --libs jack") -lboost_program_options

unix:!macx { QMAKE_LFLAGS += -rdynamic }
macx { 
	QMAKE_LFLAGS += -L/opt/local/lib/ 
#QMAKE_LFLAGS += -Wl,-dylinker
}

MOC_DIR = moc
OBJECTS_DIR = objects

unix:!macx { swigtarget.target = swig/datajockey.so }
macx { swigtarget.target = swig/datajockey.bundle }
swigtarget.commands = cd swig && ruby extconf.rb && make
swigtarget.depends = swig/*.i swig/scriptcallbackfilter.cpp

unix:!macx { swigutilstarget.target = utils-swig/datajockey_utils.so }
macx { swigutilstarget.target = utils-swig/datajockey_utils.bundle }
swigutilstarget.commands = cd utils-swig/ && ruby extconf.rb && make
swigutilstarget.depends = utils-swig/*.i

buildswig.target = swig
buildswig.depends = $$swigtarget.target $$swigutilstarget.target

ruby_annotation_files.target = ruby_annotation_files
ruby_annotation_files.files = ruby/datajockey/annotation/*.rb
ruby_annotation_files.path = $$RUBYLIBDEST/annotation/

ruby_db_files.target = ruby_db_files
ruby_db_files.files = ruby/datajockey/database/*.rb
ruby_db_files.path = $$RUBYLIBDEST/database/

ruby_files.target = ruby_files
ruby_files.files = ruby/datajockey/*.rb
ruby_files.path = $$RUBYLIBDEST
#install_ruby_files.commands = rsync -r --cvs-exclude ruby/datajockey/ $$RUBYLIBDEST

install_swig.target = install_swig
install_swig.depends = $$swigutilstarget.target $$swigtarget.target
install_swig.commands = cd utils-swig/ && make install && cd ../swig/ && make install

beatroot.target = beatroot
beatroot.files = lib/beatroot-0.5.3.jar
beatroot.path = $$PREFIX/share/datajockey/

importer.target = importer
importer.depends = install_swig
importer.files = ruby/datajockey_importer
importer.path = $$PREFIX/bin

really_clean.target = really_clean
really_clean.depends = clean
really_clean.commands = rm -fr datajockey moc/ objects/ && \
	cd swig/ && make clean && rm -f datajockey_wrap.cxx && cd .. && \
	cd utils-swig/ && make clean && rm -r utilities_wrap.cxx && cd .. && \
	cd annotator/ && make clean && rm -f datajockey_annotator

post_docs.target = post_docs
post_docs.commands = rsync -vPr --cvs-exclude doc/ x37v.info:x37v.info/datajockey/doc/

QMAKE_EXTRA_TARGETS += swigtarget 
QMAKE_EXTRA_TARGETS += swigutilstarget 
QMAKE_EXTRA_TARGETS += buildswig 
QMAKE_EXTRA_TARGETS += install_swig 
QMAKE_EXTRA_TARGETS += really_clean
QMAKE_EXTRA_TARGETS += post_docs 

POST_TARGETDEPS += $$buildswig.target

INSTALLS += importer
INSTALLS += beatroot
INSTALLS += ruby_db_files
INSTALLS += ruby_annotation_files
INSTALLS += ruby_files

# Input
HEADERS +=  include/application.hpp \
				include/applicationmodel.hpp \
				include/applicationview.hpp \
				include/audiodriver.hpp \
				include/audioio.hpp \
				include/buffer.hpp \
				include/bufferplayer.hpp \
				include/command.hpp \
				include/config.hpp \
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
				include/workdbview.hpp \
				include/workdetailview.hpp \
				include/workfilterlist.hpp \
				include/workfilterlistview.hpp \
				include/workfiltermodel.hpp \
				include/workloader.hpp \
				include/workpreviewer.hpp \
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
				src/config.cpp \
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
				src/workdbview.cpp \
				src/workdetailview.cpp \
				src/workfilterlist.cpp \
				src/workfilterlistview.cpp \
				src/workfiltermodel.cpp \
				src/workloader.cpp \
				src/workpreviewer.cpp \
				src/worktablemodel.cpp \
				src/worktagmodelfilter.cpp 

