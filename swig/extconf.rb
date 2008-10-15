require 'mkmf'
`swig -w801 -c++ -ruby datajockey.i`
#platform dependent b.s.
if RUBY_PLATFORM =~ /darwin/
	qtbasedir = "/opt/local/include/qt4-mac"
	dir_config(".", 
			["../include/", 
			qtbasedir, File.join(qtbasedir, "/QtCore"), File.join(qtbasedir, "/QtGui"), File.join(qtbasedir, "/QtSql"), 
			"/opt/local/include", "/usr/local/include"], 
			["..", "/opt/local/lib", "/usr/local/lib"])
else
	dir_config(".", ["../include/", "/usr/include/qt4", "/usr/include/qt4/QtCore", "/usr/include/qt4/QtGui", "/usr/include/qt4/QtSql"], "..")
end
create_makefile('datajockey')
