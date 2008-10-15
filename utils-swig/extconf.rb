require 'mkmf'
`swig -w801 -c++ -ruby utilities.i`

$libs = append_library($libs, "stdc++")
$libs = append_library($libs, "sndfile")
$libs = append_library($libs, "mad")
$libs = append_library($libs, "vorbisfile")
#platform dependent b.s.
if RUBY_PLATFORM =~ /darwin/
	dir_config(".", ["../include/", "/opt/local/include", "/usr/local/include"], ["..", "/opt/local/lib", "/usr/local/lib"])
else
	dir_config(".", ["../include/"], "..")
end
create_makefile('datajockey_utils')
