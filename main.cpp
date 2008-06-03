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
	QSortFilterProxyModel sortedModel;
	sortedModel.setSourceModel(&tableModel);
	sortedModel.setDynamicSortFilter(true);
	sortedModel.setSortCaseSensitivity(Qt::CaseInsensitive);

	QTableView *tableView = new QTableView(window);
	tableView->setSortingEnabled(true);
	tableView->setModel(&sortedModel);
	tableView->setColumnHidden(0, true);
	tableView->horizontalHeader()->setMovable(true);

	layout->addWidget(mixerPannel, 1);
	layout->addWidget(tableView, 10);
	window->show();
	return app.exec();
}

