#include "applicationview.hpp"

//views
#include "workdbview.hpp"
#include "crossfadeview.hpp"
#include "djmixerchannelview.hpp"
#include "djmixercontrolview.hpp"
#include "eqview.hpp"
#include "masterview.hpp"
#include "mixerchannelview.hpp"
#include "mixerpanelview.hpp"
#include "interpreterview.hpp"
#include "tageditor.hpp"
#include "workdetailview.hpp"
#include "workfilterlistview.hpp"

//models
#include "applicationmodel.hpp"
#include "worktablemodel.hpp"
#include "crossfademodel.hpp"
#include "djmixerchannelmodel.hpp"
#include "djmixercontrolmodel.hpp"
#include "eqmodel.hpp"
#include "mastermodel.hpp"
#include "mixerchannelmodel.hpp"
#include "mixerpanelmodel.hpp"
#include "interpretermodel.hpp"
#include "tagmodel.hpp"
#include "workfiltermodel.hpp"

//qt stuff
#include <QSplitter>
#include <QVBoxLayout>
#include <QTabWidget>

ApplicationView::ApplicationView(ApplicationModel * model):
	QWidget()
{
	//store a pointer to our model
	mModel = model;

	//create subviews
	mWorkDetail = new WorkDetailView(mModel->tagModel(), mModel->db(), this);
	mWorkDB = new WorkDBView(mModel->filteredWorkTable(), this);
	mMixerPanel = new MixerPanelView(mModel->mixerPanel()->numMixerChannels(), this);
	mTagEditor = new TagEditor(mModel->tagModel(), this);
	mWorkFilterList = new WorkFilterListView(mModel->workFilterList(), this);

	mInterpView = new InterpreterView(mModel->interpreter(), this);

	//set up our view
	setWindowTitle("data jockey");
	QVBoxLayout * layout = new QVBoxLayout(this);
	QSplitter * horiSplit = new QSplitter(Qt::Horizontal, this);
	QSplitter * vertSplit = new QSplitter(Qt::Vertical, this);
	QTabWidget * leftTopTabView = new QTabWidget(this);
	QTabWidget * rightTabView = new QTabWidget(this);
	layout->addWidget(horiSplit);
	layout->setContentsMargins(2,2,2,2);
	setLayout(layout);

	leftTopTabView->addTab(mMixerPanel, "mixer panel");
	leftTopTabView->addTab(mTagEditor, "tags");
	leftTopTabView->addTab(mWorkFilterList, "filters");

	rightTabView->addTab(mWorkDB, "work list");
	rightTabView->addTab(mInterpView, "script interpreter");

	vertSplit->addWidget(leftTopTabView);
	vertSplit->addWidget(mWorkDetail);

	horiSplit->addWidget(vertSplit);
	horiSplit->addWidget(rightTabView);
	horiSplit->setStretchFactor(0,0);
	horiSplit->setStretchFactor(1,10);

	//connect up internal sigs/slots
	QObject::connect(mWorkDB, SIGNAL(workSelected(int)), mWorkDetail, SLOT(setWork(int)));

	//connect to model
	connectToModel();
}

MixerPanelView * ApplicationView::mixerPanel() const {
	return mMixerPanel;
}

WorkDBView * ApplicationView::workDB() const {
	return mWorkDB;
}

TagEditor * ApplicationView::tagEditor() const {
	return mTagEditor;
}

void ApplicationView::connectToModel(){

	MixerPanelModel * mixerModel = mModel->mixerPanel();

	//mixer panel model + view
	for(unsigned int i = 0; i < mixerModel->numMixerChannels(); i++){
		DJMixerChannelModel * djMixerModel = mixerModel->mixerChannels()->at(i);
		DJMixerChannelView * djMixerView = mMixerPanel->mixerChannels()->at(i);

		//****** mixer signals
		//volume
		QObject::connect(
				djMixerModel,
				SIGNAL(volumeChanged(float)),
				djMixerView->mixerChannel(),
				SLOT(setVolume(float)));
		QObject::connect(
				djMixerView->mixerChannel(),
				SIGNAL(volumeChanged(float)),
				djMixerModel,
				SLOT(setVolume(float)));
		//mute
		QObject::connect(
				djMixerModel,
				SIGNAL(mutedChanged(bool)),
				djMixerView->mixerChannel(),
				SLOT(setMuted(bool)));
		QObject::connect(
				djMixerView->mixerChannel(),
				SIGNAL(mutedChanged(bool)),
				djMixerModel,
				SLOT(setMuted(bool)));
		//eq
		EQModel * eqModel = djMixerModel->eq();
		EQView * eqView = djMixerView->mixerChannel()->eq();
		QObject::connect(
				eqModel,
				SIGNAL(highChanged(float)),
				eqView,
				SLOT(setHigh(float)));
		QObject::connect(
				eqModel,
				SIGNAL(midChanged(float)),
				eqView,
				SLOT(setMid(float)));
		QObject::connect(
				eqModel,
				SIGNAL(lowChanged(float)),
				eqView,
				SLOT(setLow(float)));
		QObject::connect(
				eqView,
				SIGNAL(highChanged(float)),
				eqModel,
				SLOT(setHigh(float)));
		QObject::connect(
				eqView,
				SIGNAL(midChanged(float)),
				eqModel,
				SLOT(setMid(float)));
		QObject::connect(
				eqView,
				SIGNAL(lowChanged(float)),
				eqModel,
				SLOT(setLow(float)));

		//cue
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(cueModeChanged(bool)),
				djMixerView->DJMixerControl(),
				SLOT(setCueing(bool)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(cueModeChanged(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(setCueing(bool)));
		//pause
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(pausedChanged(bool)),
				djMixerView->DJMixerControl(),
				SLOT(setPaused(bool)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(pausedChanged(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(setPaused(bool)));
		//sync mode
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(syncModeChanged(bool)),
				djMixerView->DJMixerControl(),
				SLOT(setSync(bool)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(syncModeChanged(bool)),
				djMixerModel->DJMixerControl(),
				SLOT(setSync(bool)));
		//tempo multiplier
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(tempoMulChanged(double)),
				djMixerView->DJMixerControl(),
				SLOT(setTempoMul(double)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(tempoMulChanged(double)),
				djMixerModel->DJMixerControl(),
				SLOT(setTempoMul(double)));
		//progress
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(progressChanged(float)),
				djMixerView->DJMixerControl(),
				SLOT(setProgress(float)));
		//seek [just from view to mixerModel]
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(seekingFwd()),
				djMixerModel->DJMixerControl(),
				SLOT(seekFwd()));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(seekingBwd()),
				djMixerModel->DJMixerControl(),
				SLOT(seekBkwd()));
		//beat offset
		QObject::connect(
				djMixerModel->DJMixerControl(),
				SIGNAL(beatOffsetChanged(int)),
				djMixerView->DJMixerControl(),
				SLOT(setBeatOffset(int)));
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(beatOffsetChanged(int)),
				djMixerModel->DJMixerControl(),
				SLOT(setBeatOffset(int)));
		//reset 
		QObject::connect(
				djMixerView->DJMixerControl(),
				SIGNAL(resetClicked()),
				djMixerModel->DJMixerControl(),
				SLOT(resetWorkPosition()));
	}

	//master
	//volume
	QObject::connect(
			mixerModel->master(),
			SIGNAL(volumeChanged(float)),
			mMixerPanel->master(),
			SLOT(setVolume(float)));
	QObject::connect(
			mMixerPanel->master(),
			SIGNAL(volumeChanged(float)),
			mixerModel->master(),
			SLOT(setVolume(float)));
	//tempo
	QObject::connect(
			mixerModel->master(),
			SIGNAL(tempoChanged(float)),
			mMixerPanel->master(),
			SLOT(setTempo(float)));
	QObject::connect(
			mMixerPanel->master(),
			SIGNAL(tempoChanged(float)),
			mixerModel->master(),
			SLOT(setTempo(float)));
	//tempo mul
	QObject::connect(
			mixerModel->master(),
			SIGNAL(tempoMulChanged(double)),
			mMixerPanel->master(),
			SLOT(setTempoMul(double)));
	QObject::connect(
			mMixerPanel->master(),
			SIGNAL(tempoMulChanged(double)),
			mixerModel->master(),
			SLOT(setTempoMul(double)));
	//sync src
	QObject::connect(
			mixerModel->master(),
			SIGNAL(syncSourceChanged(unsigned int)),
			mMixerPanel->master(),
			SLOT(setSyncSource(unsigned int)));
	QObject::connect(
			mMixerPanel->master(),
			SIGNAL(syncSourceChanged(unsigned int)),
			mixerModel->master(),
			SLOT(setSyncSource(unsigned int)));

	//cross fade
	QObject::connect(
			mixerModel->crossFade(),
			SIGNAL(mixersChanged(unsigned int, unsigned int)),
			mMixerPanel->crossFade(),
			SLOT(setMixers(unsigned int, unsigned int)));
	QObject::connect(
			mMixerPanel->crossFade(),
			SIGNAL(mixersChanged(unsigned int, unsigned int)),
			mixerModel->crossFade(),
			SLOT(setMixers(unsigned int, unsigned int)));
	QObject::connect(
			mixerModel->crossFade(),
			SIGNAL(disabled()),
			mMixerPanel->crossFade(),
			SLOT(disable()));
	QObject::connect(
			mMixerPanel->crossFade(),
			SIGNAL(disabled()),
			mixerModel->crossFade(),
			SLOT(disable()));
	QObject::connect(
			mixerModel->crossFade(),
			SIGNAL(positionChanged(float)),
			mMixerPanel->crossFade(),
			SLOT(setPosition(float)));
	QObject::connect(
			mMixerPanel->crossFade(),
			SIGNAL(positionChanged(float)),
			mixerModel->crossFade(),
			SLOT(setPosition(float)));

	//tag editor + tag model
	QObject::connect(
			mTagEditor,
			SIGNAL(tagAdded(int,QString)),
			mModel->tagModel(),
			SLOT(addTag(int,QString)));
	QObject::connect(
			mTagEditor,
			SIGNAL(tagAdded(QString,QString)),
			mModel->tagModel(),
			SLOT(addClassAndTag(QString,QString)));

	//filtering the work view
	QObject::connect(
			mWorkDB,
			SIGNAL(applyFilterPushed()),
			mModel->filteredWorkTable(),
			SLOT(filter()));
	QObject::connect(
			mWorkDB,
			SIGNAL(removeFilterPushed()),
			mModel->filteredWorkTable(),
			SLOT(clearFilter()));

	//connect up the interpreter
	QObject::connect(
			mInterpView,
			SIGNAL(newInput(QString)),
			mModel->interpreter(),
			SLOT(addToInput(QString)));
	QObject::connect(
			mInterpView,
			SIGNAL(cancelingInput()),
			mModel->interpreter(),
			SLOT(cancelInput()));
	QObject::connect(
			mModel->interpreter(),
			SIGNAL(newOutput(QString)),
			mInterpView,
			SLOT(addToOutput(QString)));
}

