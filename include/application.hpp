#ifndef APPLICATION_HPP
#define APPLICATION_HPP

class MixerPanelView;
class MixerPanelModel;
class AudioDriver;

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QThread>
#include "buffer.hpp"

class AudioDriverThread;

class DataJockeyApplication {
	public:
		static int run(int argc, char *argv[]);
		static void connectMixerPanelModelView(MixerPanelModel * model, MixerPanelView * view);
		static void connectMixerPanelModelDriver(MixerPanelModel * model, AudioDriver * driver);
};

class AudioDriverThread : public QThread {
	Q_OBJECT
	public:
		AudioDriverThread(QObject * parent = NULL);
		void setAudioDriver(AudioDriver * driver);
		void run();
	public slots:
		void stop();
	private:
		AudioDriver * mDriver;
};

#endif

