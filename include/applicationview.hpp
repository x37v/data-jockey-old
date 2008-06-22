#ifndef APPLICATION_VIEW_HPP
#define APPLICATION_VIEW_HPP

#include <QWidget>

class ApplicationModel;
class WorkDetailView;
class AudioWorkDBView;
class TagEditor;
class MixerPanelView;

class ApplicationView : public QWidget {
	Q_OBJECT
	public:
		ApplicationView(ApplicationModel * model);
		MixerPanelView * mixerPanel();
		AudioWorkDBView * workDB();
	private:
		ApplicationModel * mModel;
		WorkDetailView * mWorkDetail;
		AudioWorkDBView * mAudioWorkDB;
		TagEditor * mTagEditor;
		MixerPanelView * mMixerPanel;
		void connectToModel();
};

#endif

