DEPENDENCIES:
	*from others (in apt):
		I've given the ubuntu package info when possible.. you might be able to use
		fink to do something similar on OSX, if you do please provide me with the info!
		I've also provided the URLS in case you cannot use a package manager.
	apt (in one line):
		apt-get install libjack0.100.0-dev libsyck0-dev libsndfile1-dev libvorbis-dev libmad0-dev libboost-regex-dev libboost-dev libqt4-dev libqt4-sql-mysql swig ruby1.8-dev rubygems sun-java6-bin libtag1-dev mysql-server librdf0-dev
	macports (in one line):

		port install libsndfile +flac &&
		port install syck jack libmad libvorbis boost swig ruby rb-rubygems taglib redland && 
		port install mysql5 +server &&
		port install qt4-mac +mysql5 +sqlite &&
		port install fftw-3-single fftw-3
		
		had problems installing sqlite3 [i think swig requires it]
		i followed the instructions here:
		http://www.nabble.com/Building-sqlite3-fails-td19454313.html
		port clean --work sqlite3 
		then port install swig....

	broken down:
		jack:
			apt-get install libjack0.100.0-dev
			port install jack
			http://jackaudio.org/download
		libsyck:
			apt-get install libsyck0-dev
			port install syck
			http://rubyforge.org/frs/download.php/4492/syck-0.55.tar.gz
		libsndfile (might have to install libflac to get flac support):
			apt-get install libsndfile1-dev
			port install libsndfile +flac
			http://www.mega-nerd.com/libsndfile/#Download
			for some reason libsndfile on osx [macports] doesn't work with current flacs??
		libvorbis:
			apt-get install libvorbis-dev
			port install libvorbis
			http://www.xiph.org/downloads/
		libmad:
			apt-get install libmad0-dev
			port install libmad
			http://sourceforge.net/project/showfiles.php?group_id=12349
		boost (just need the regex part [a library] and the shared/intrusive ptr [header]):
			apt-get install libboost-regex-dev libboost-dev
			port install boost
			http://www.boost.org/users/download/
		qt4: (qt and qtsql)
			apt-get install libqt4-dev libqt4-sql-mysql
			port install qt4-mac +mysql5 +sqlite
			http://trolltech.com/downloads/opensource#qt-open-source-edition
				(I assume the main download has the sql stuff in it, but the main ubuntu package doesn't)
		swig:
			apt-get install swig
			port install swig
			http://www.swig.org/download.html
		taglib:
		 	apt-get install libtag1-dev
			port install taglib
			http://developer.kde.org/~wheeler/taglib.html
		mysql:
			apt-get install mysql-server
			port install mysql5 +server
		java:
		 	apt-get install sun-java6-bin
		ruby + gems:
			apt-get install ruby1.8-dev rubygems
			port install ruby rb-rubygems
			http://www.ruby-lang.org/en/downloads/
			http://rubyforge.org/projects/rubygems
		redland rdf (for slv2):
			apt-get install librdf0-dev
			port install redland

	*from others (not in apt or macports):
		activerecord rtaglib (ruby):
			gem install activerecord rtaglib --remote
			taglib didn't build for me on mac (because it cannot find taglib include and library files) 
			so i did this (as root):
			gem install rtaglib -- --build-flags --with-tag_c-lib=/opt/local/lib/ --with_tag_c-include=/opt/local/include/
		slv2:
			http://download.drobilla.net/slv2-0.6.0.tar.gz
			have to install deps, then for osx i had to do:
			export PKG_CONFIG_PATH=PKG_CONFIG_PATH:/usr/local/lib/pkgconfig
			because pkgconfig stuff is in /usr/local/lib/pkgconfig
			the jack host didn't build, but i replaced #include <malloc.h> with: #include <stdlib.h> and it worked
		oscpack:
			http://www.audiomulch.com/~rossb/code/oscpack/
			osx:
				have to edit the makefile to use the OS X case [search for OS X]
		swh lv2 plugins:
		 	http://plugin.org.uk/lv2/swh-lv2-1.0.15.tar.gz
		 	http://plugin.org.uk/
			requires fftw3 [installed via ports above]

	*dependencies from me:
		yaml_cpp:
			http://x37v.info/projects/cpp/yaml/files/yamlcpp.0.0.1.tar.gz
		jack_cpp:
			http://x37v.info/jack_cpp/code/jackcpp.0.3.tar.gz
		use pkgconfig for jack CFLAGS and LDFLAGS
		had to add /opt/local/include to CFLAGS and /opt/local/lib to LD flags

BUILD:
	makefile:
		qmake
		make
	xcode project (mac):
		qmake -spec macx-xcode datajockey.pro
		do xcode stuff...
	(don't install yet as that hasn't been fully set up)

CREATE A DATABASE (as root) 
	OSX:
		http://2tbsp.com/content/install_and_configure_mysql_5_macports

	Linux:
		mysql -u root or mysql -u root -p [if there is a password]:

	OSX: 
		mysql5 -u root or mysql5 -u root -p [if there is a password]:

	sub DBNAME with datajockey or whatever you want to call it
	sub USER with your user name if you want.. or dj_user or something..
	sub PASSWORD (if you're using one) with a password!

	*create it:
		create database DBNAME default character set 'utf8' default collate 'utf8_general_ci';
		example:
		create database datajockey default character set 'utf8' default collate 'utf8_general_ci';

	*grant perms:
		with password:
			grant all on DBNAME.* to 'USER'@'localhost' identified by 'PASSWORD';
			example
			grant all on datajockey.* to 'alex'@'localhost' identified by 'apassword';
		without password:
			grant all on DBNAME.* to 'USER'@'localhost';
			example:
			grant all on datajockey.* to 'alex'@'localhost';

CONFIG FILE:
	edit config.yaml to reflect your DB settings above

CREATE THE DB STRUCTURE:
	pushd ruby && rake db:migrate && popd

IMPORT DATA!:
	Presently this requires that you have properly tagged files (preferably flac)
	Also, the path to these files should not change after you import them.
	./ruby/import_audio.rb FILES

RUN THE APP!
	might want to start jack separately, though you shouldn't have to
	 for testing i use: jackd -d alsa -p 1024 -n 3 -r 44100 ]

	./datajockey

