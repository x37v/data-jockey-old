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

