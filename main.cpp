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
	QHBoxLayout * layout = new QHBoxLayout(window);
	window->setLayout(layout);

	MixerPanelView * mixerPannel = new MixerPanelView(4,window);
	AudioWorkTableModel tableModel(db);
	tableModel.setFiltered(true);
	tableModel.query();

	AudioWorkDBView * tableView = new AudioWorkDBView(&tableModel, window);

	layout->addWidget(mixerPannel, 1);
	layout->addWidget(tableView, 10);
	window->show();

	MixerChannelModel * mixerModel = new MixerChannelModel;
	MixerChannelView * mixerChan = mixerPannel->mixerChannels()->front()->mixerChannel();
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

