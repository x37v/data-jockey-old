#ifndef APPLICATION_MODEL_HPP
#define APPLICATION_MODEL_HPP

#include <QObject>
#include <QSqlDatabase>

class AudioWorkTableModel;
class MixerPanelModel;
class TagModel;
class WorkFilterModelProxy;

class ApplicationModel : public QObject {
	Q_OBJECT
	public:
		ApplicationModel(unsigned int num_mixers, QSqlDatabase db, QObject * parent = NULL);
		QSqlDatabase db();
		AudioWorkTableModel * audioWorkTable();
		MixerPanelModel * mixerPanel();
		TagModel * tagModel();
		WorkFilterModelProxy * filteredWorkTable();
	private:
		AudioWorkTableModel * mAudioWorkTable;
		MixerPanelModel * mMixerPanel;
		TagModel * mTagModel;
		WorkFilterModelProxy * mFilterProxy;
		QSqlDatabase mDB;
};

#endif

