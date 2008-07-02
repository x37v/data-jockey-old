#ifndef APPLICATION_MODEL_HPP
#define APPLICATION_MODEL_HPP

#include <QSqlDatabase>
#include <QObject>

class WorkTableModel;
class MixerPanelModel;
class TagModel;
class WorkFilterModelProxy;
class WorkFilterList;
class InterpreterModel;
class RemoteWorkFilterModel;

class ApplicationModel : public QObject {
	Q_OBJECT
	public:
		static void setNumberOfMixers(unsigned int num);
		static void setDataBase(QString type, 
				QString name, 
				QString password = "", 
				int port = -1, 
				QString host = QString("localhost"));
		static ApplicationModel * instance();

		QSqlDatabase db() const;
		WorkTableModel * WorkTable() const;
		MixerPanelModel * mixerPanel() const;
		TagModel * tagModel() const;
		WorkFilterModelProxy * filteredWorkTable() const;
		WorkFilterList * workFilterList() const;
		InterpreterModel * interpreter() const;
	protected:
		ApplicationModel();
		ApplicationModel(const ApplicationModel&);
		ApplicationModel& operator= (const ApplicationModel&);
		virtual ~ApplicationModel();
	private:
		static ApplicationModel * cInstance;
		static unsigned int cNumMixers;
		static QSqlDatabase cDB;
		WorkTableModel * mWorkTable;
		MixerPanelModel * mMixerPanel;
		TagModel * mTagModel;
		WorkFilterModelProxy * mFilterProxy;
		WorkFilterList * mWorkFilterList;
		InterpreterModel * mInterp;
};

class ApplicationModelProxy : public QObject {
	Q_OBJECT
	public:
		ApplicationModelProxy(Qt::ConnectionType type = Qt::AutoCompatConnection, 
				QObject * parent = NULL);
		MixerPanelModel * mixerPanel() const;
		void addFilter(RemoteWorkFilterModel * filter);
		void removeFilter(RemoteWorkFilterModel * filter);
		unsigned int numFilters();
		RemoteWorkFilterModel * filter(unsigned int i);
	private:
		QList<RemoteWorkFilterModel *> mFilterList;
		MixerPanelModel * mMixerPanel;
};

#endif

