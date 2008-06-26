#ifndef APPLICATION_VIEW_HPP
#define APPLICATION_VIEW_HPP

#include <QWidget>

class ApplicationModel;
class WorkDetailView;
class AudioWorkDBView;
class TagEditor;
class MixerPanelView;
class WorkFilterListView;

class ApplicationView : public QWidget {
	Q_OBJECT
	public:
		ApplicationView(ApplicationModel * model);
		MixerPanelView * mixerPanel() const;
		AudioWorkDBView * workDB() const;
		TagEditor * tagEditor() const;
	private:
		ApplicationModel * mModel;
		WorkDetailView * mWorkDetail;
		AudioWorkDBView * mAudioWorkDB;
		TagEditor * mTagEditor;
		MixerPanelView * mMixerPanel;
		WorkFilterListView * mWorkFilterList;
		void connectToModel();
};

#endif

