/*
 *		Copyright (c) 2008 Alex Norman.  All rights reserved.
 *		http://www.x37v.info/datajockey
 *
 *		This file is part of Data Jockey.
 *		
 *		Data Jockey is free software: you can redistribute it and/or modify it
 *		under the terms of the GNU General Public License as published by the
 *		Free Software Foundation, either version 3 of the License, or (at your
 *		option) any later version.
 *		
 *		Data Jockey is distributed in the hope that it will be useful, but
 *		WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *		Public License for more details.
 *		
 *		You should have received a copy of the GNU General Public License along
 *		with Data Jockey.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

class MixerPanelView;
class MixerPanelModel;
class AudioDriver;
class OscReceiver;

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QThread>
#include "buffer.hpp"

class DataJockeyApplication {
	public:
		static int run(int argc, char *argv[]);
};

class OscThread : public QThread {
	Q_OBJECT
	public:
		OscThread(OscReceiver * receiver, unsigned int port);
		void run();
	private:
		OscReceiver * mOscReceiver;
		unsigned int mPort;
};

class RubyInterpreterThread : public QThread {
	Q_OBJECT
	public:
		void run();
};

#endif

