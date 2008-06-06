#include "mixerpanelview.hpp"
#include <QApplication>
#include <QWidget>
#include <QCleanlooksStyle>

#include <QHBoxLayout>
#include <QSqlDatabase>
#include <QSqlError>
#include <QMessageBox>
#include <QTableView>

#include <QHeaderView>
#include <QSortFilterProxyModel>

#include "crossfadeview.hpp"
#include "audioworktablemodel.hpp"
#include "audioworkdbview.hpp"

#include "mixerchannelmodel.hpp"
#include "mixerchannelview.hpp"

#include "djmixerchannelview.hpp"

#include "djmixercontrolview.hpp"
#include "djmixercontrolmodel.hpp"

#include "eqview.hpp"
#include "eqmodel.hpp"

#include "workdetailview.hpp"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setStyle(new QCleanlooksStyle);

	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setDatabaseName("dj_development");
	if (!db.open()) {
		QMessageBox::critical(0, app.tr("Cannot open database"),
				app.tr("Unable to establish a database connection.\n"
					"Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}


	QWidget * window = new QWidget;
	window->setWindowTitle("floatme");
	QGridLayout * layout = new QGridLayout(window);
	layout->setContentsMargins(2,2,2,2);
	window->setLayout(layout);

	WorkDetailView * detailView = new WorkDetailView(db, window);
	detailView->show();
	detailView->setWork(12);

	MixerPanelView * mixerPannel = new MixerPanelView(4, window);

	AudioWorkTableModel tableModel(db);
	tableModel.setFiltered(true);
	tableModel.query();

	AudioWorkDBView * tableView = new AudioWorkDBView(&tableModel, window);

	layout->addWidget(mixerPannel, 0, 0);
	layout->addWidget(tableView, 0, 1, 2, 1);
	layout->addWidget(detailView, 1, 0);
	layout->setColumnStretch(1,10);
	layout->setColumnStretch(0,0);
	layout->setRowStretch(0,10);
	layout->setRowStretch(1,0);
	window->show();

	MixerChannelModel * mixerModel = new MixerChannelModel;
	MixerChannelView * mixerChan = mixerPannel->mixerChannels()->front()->mixerChannel();

	DJMixerControlModel * djModel = new DJMixerControlModel;

	QObject::connect(
			(*mixerPannel->mixerChannels())[0]->DJMixerControl(),
			SIGNAL(pausedChanged(bool)),
			djModel, SLOT(setPaused(bool)));
	QObject::connect(
			(*mixerPannel->mixerChannels())[0]->DJMixerControl(),
			SIGNAL(syncModeChanged(bool)),
			djModel, SLOT(setRunFree(bool)));
	QObject::connect(
			djModel,
			SIGNAL(progressChanged(float)),
			(*mixerPannel->mixerChannels())[0]->DJMixerControl(),
			SLOT(setProgress(float)));
	djModel->setProgress(0.324);

	EQView * eqView = mixerChan->eq();
	EQModel * eqModel = new EQModel();

	QObject::connect(
			eqView,
			SIGNAL(highValueChanged(float)),
			eqModel, SLOT(setHigh(float)));
	QObject::connect(
			eqModel,
			SIGNAL(highChanged(float)),
			eqView, SLOT(setHigh(float)));

	QObject::connect(
			eqView,
			SIGNAL(midValueChanged(float)),
			eqModel, SLOT(setMid(float)));
	QObject::connect(
			eqModel,
			SIGNAL(midChanged(float)),
			eqView, SLOT(setMid(float)));

	QObject::connect(
			eqView,
			SIGNAL(lowValueChanged(float)),
			eqModel, SLOT(setLow(float)));
	QObject::connect(
			eqModel,
			SIGNAL(lowChanged(float)),
			eqView, SLOT(setLow(float)));

	QObject::connect(
			mixerChan,
			SIGNAL(volumeChanged(float)),
			mixerModel, SLOT(setVolume(float)));

	QObject::connect(
			mixerModel,
			SIGNAL(volumeChanged(float)),
			mixerChan,
			SLOT(setVolume(float)));

	QObject::connect(
			mixerChan,
			SIGNAL(mutedChanged(bool)),
			mixerModel,
			SLOT(setMuted(bool)));
	QObject::connect(
			mixerModel,
			SIGNAL(mutedChanged(bool)),
			mixerChan,
			SLOT(setMuted(bool)));

	mixerModel->setMuted(true);
	mixerModel->setVolume(2.0);

	return app.exec();
}

