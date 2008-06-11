#ifndef APPLICATION_HPP
#define APPLICATION_HPP

class MixerPanelView;
class MixerPanelModel;
class AudioDriver;

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

class WorkLoaderProxy;

class DataJockeyApplication {
	public:
		static int run(int argc, char *argv[]);
		static void connectMixerPanelModelView(MixerPanelModel * model, MixerPanelView * view);
		static void connectMixerPanelModelDriver(MixerPanelModel * model, AudioDriver * driver);
};

class WorkLoaderProxy : public QObject {
	Q_OBJECT
	public:
		WorkLoaderProxy(const QSqlDatabase & db = QSqlDatabase(), QObject * parent = NULL);
	public slots:
		void selectWork(int work);
		void loadWork(unsigned int mixer);
	signals:
		void mixerLoad(unsigned int mixer, QString audiobufloc, QString beatbufloc, bool wait_for_measure = false);
	private:
		static QString cQueryString;
		int mWork;
		QSqlQuery mQuery;
};

#endif

