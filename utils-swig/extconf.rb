require 'mkmf'
`swig -w801 -c++ -ruby utilities.i`
$libs = append_library($libs, "stdc++")
$libs = append_library($libs, "sndfile")
$libs = append_library($libs, "mad")
dir_config(".", ["../include/"], "..")
create_makefile('datajockey_utils')
