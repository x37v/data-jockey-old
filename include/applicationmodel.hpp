#ifndef APPLICATION_MODEL_HPP
#define APPLICATION_MODEL_HPP

#include <QObject>
#include <QSqlDatabase>

class AudioWorkTableModel;
class MixerPanelModel;
class TagModel;
class WorkFilterModelProxy;
class WorkFilterList;

class ApplicationModel : public QObject {
	Q_OBJECT
	public:
		ApplicationModel(unsigned int num_mixers, QSqlDatabase & db, QObject * parent = NULL);
		virtual ~ApplicationModel();
		QSqlDatabase db() const;
		AudioWorkTableModel * audioWorkTable() const;
		MixerPanelModel * mixerPanel() const;
		TagModel * tagModel() const;
		WorkFilterModelProxy * filteredWorkTable() const;
		WorkFilterList * workFilterList() const;
	private:
		AudioWorkTableModel * mAudioWorkTable;
		MixerPanelModel * mMixerPanel;
		TagModel * mTagModel;
		WorkFilterModelProxy * mFilterProxy;
		WorkFilterList * mWorkFilterList;
		QSqlDatabase mDB;
};

#endif

