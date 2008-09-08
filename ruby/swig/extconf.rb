require 'mkmf'
`swig -w801 -c++ -ruby datajockey.i`
dir_config(".", ["../include/", "/usr/include/qt4", "/usr/include/qt4/QtCore", "/usr/include/qt4/QtGui", "/usr/include/qt4/QtSql"], "..")
create_makefile('datajockey')
