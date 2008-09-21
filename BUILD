DEPENDENCIES:
	*from others (in apt):
		I've given the ubuntu package info when possible.. you might be able to use
		fink to do something similar on OSX, if you do please provide me with the info!
		I've also provided the URLS in case you cannot use a package manager.
	apt (in one line):
		apt-get install libjack0.100.0-dev libsyck0-dev libsndfile1-dev libvorbis-dev libmad0-dev libboost-regex-dev libboost-dev libqt4-dev libqt4-sql-mysql swig ruby1.8-dev rubygems sun-java6-bin

	broken down:
		jack:
			apt-get install libjack0.100.0-dev
			http://jackaudio.org/download
		libsyck:
			apt-get install libsyck0-dev
			http://rubyforge.org/frs/download.php/4492/syck-0.55.tar.gz
		libsndfile (might have to install libflac to get flac support):
			apt-get install libsndfile1-dev
			http://www.mega-nerd.com/libsndfile/#Download
		libvorbis:
			apt-get install libvorbis-dev
			http://www.xiph.org/downloads/
		libmad:
			apt-get install libmad0-dev
			http://sourceforge.net/project/showfiles.php?group_id=12349
		boost (just need the regex part [a library] and the shared/intrusive ptr [header]):
			apt-get install libboost-regex-dev libboost-dev
			http://www.boost.org/users/download/
		qt4: (qt and qtsql)
			apt-get install libqt4-dev libqt4-sql-mysql
			http://trolltech.com/downloads/opensource#qt-open-source-edition
				(I assume the main download has the sql stuff in it, but the main ubuntu package doesn't)
		swig:
			apt-get install swig
			http://www.swig.org/download.html
		ruby + gems:
			apt-get install ruby1.8-dev rubygems
			http://www.ruby-lang.org/en/downloads/
			http://rubyforge.org/projects/rubygems
		java:
		 	apt-get install sun-java6-bin

	*from others (not in apt):
		activerecord rtaglib (ruby):
			gem install activerecord rtaglib --remote
		slv2:
			http://download.drobilla.net/slv2-0.6.0.tar.gz
		oscpack:
			http://www.audiomulch.com/~rossb/code/oscpack/
		swh lv2 plugins:
		 	http://plugin.org.uk/lv2/swh-lv2-1.0.15.tar.gz
		 	http://plugin.org.uk/

	*dependencies from me:
		yaml_cpp:
			http://x37v.info/projects/cpp/yaml/files/yamlcpp.0.0.1.tar.gz
		jack_cpp:
			http://x37v.info/jack_cpp/code/jackcpp.0.3.tar.gz

BUILD:
	qmake (creates the makefile)
	make
	(don't install yet as that hasn't been fully set up)

CREATE A DATABASE (as root) 
	mysql -u root or mysql -u root -p [if there is a password]:

	sub DBNAME with datajockey or whatever you want to call it
	sub USER with your user name if you want.. or dj_user or something..
	sub PASSWORD (if you're using one) with a password!

	*create it:
		create database DBNAME default character set 'utf8' default collate 'utf8_general_ci';

	*grant perms:
		with password:
			grant all on DBNAME.* to 'USER'@'localhost' identified by 'PASSWORD';
		without password:
			grant all on DBNAME.* to 'USER'@'localhost';

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

