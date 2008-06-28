#ifndef APPLICATION_MODEL_HPP
#define APPLICATION_MODEL_HPP

#include <QSqlDatabase>
#include <QObject>

class AudioWorkTableModel;
class MixerPanelModel;
class TagModel;
class WorkFilterModelProxy;
class WorkFilterList;

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
		AudioWorkTableModel * audioWorkTable() const;
		MixerPanelModel * mixerPanel() const;
		TagModel * tagModel() const;
		WorkFilterModelProxy * filteredWorkTable() const;
		WorkFilterList * workFilterList() const;
	protected:
		ApplicationModel();
		ApplicationModel(const ApplicationModel&);
		ApplicationModel& operator= (const ApplicationModel&);
		virtual ~ApplicationModel();
	private:
		static ApplicationModel * cInstance;
		static unsigned int cNumMixers;
		static QSqlDatabase cDB;
		AudioWorkTableModel * mAudioWorkTable;
		MixerPanelModel * mMixerPanel;
		TagModel * mTagModel;
		WorkFilterModelProxy * mFilterProxy;
		WorkFilterList * mWorkFilterList;
};

#endif

