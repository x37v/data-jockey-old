#include "workfiltermodel.hpp"
#include "audioworktablemodel.hpp"

WorkFilterModel::WorkFilterModel(QObject * parent) :
	QObject(parent)
{
}

WorkFilterModel::~WorkFilterModel(){
}

WorkFilterModelProxy::WorkFilterModelProxy(AudioWorkTableModel * parent) : 
	QSortFilterProxyModel(parent)
{
	mFilter = NULL;
	setSourceModel(parent);
}

bool WorkFilterModelProxy::lessThan(const QModelIndex &left,
		const QModelIndex &right) const {
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);
	if(leftData.canConvert(QVariant::String)){
		//get the data
		QString leftArtist = 
			sourceModel()->data(left.sibling(left.row(), AudioWorkTableModel::artistColumn)).toString().toCaseFolded();
		QString leftAlbum = 
			sourceModel()->data(left.sibling(left.row(), AudioWorkTableModel::albumColumn)).toString().toCaseFolded();
		QString leftTitle = 
			sourceModel()->data(left.sibling(left.row(), AudioWorkTableModel::titleColumn)).toString().toCaseFolded();
		int leftTrack = sourceModel()->data(left.sibling(left.row(), AudioWorkTableModel::trackColumn)).toInt();
		QString rightArtist = 
			sourceModel()->data(right.sibling(right.row(), AudioWorkTableModel::artistColumn)).toString().toCaseFolded();
		QString rightAlbum = 
			sourceModel()->data(right.sibling(right.row(), AudioWorkTableModel::albumColumn)).toString().toCaseFolded();
		QString rightTitle = 
			sourceModel()->data(right.sibling(right.row(), AudioWorkTableModel::titleColumn)).toString().toCaseFolded();
		int rightTrack = sourceModel()->data(right.sibling(right.row(), AudioWorkTableModel::trackColumn)).toInt();

		//we sort specially for artist, title and album
		//artist, album, track, title
		if((unsigned int)left.column() == AudioWorkTableModel::artistColumn){
			if(leftArtist != rightArtist)
				return leftArtist < rightArtist;
			if(leftAlbum != rightAlbum)
				return leftAlbum < rightAlbum;
			if(leftTrack != rightTrack)
				return leftTrack < rightTrack;
			return leftTitle < rightTitle;
		//title, artist, album
		} else if((unsigned int)left.column() == AudioWorkTableModel::titleColumn){
			if(leftTitle != rightTitle)
				return leftTitle < rightTitle;
			if(leftArtist != rightArtist)
				return leftArtist < rightArtist;
			return leftAlbum < rightAlbum;
		//album, track, artist, title
		} else if((unsigned int)left.column() == AudioWorkTableModel::albumColumn){
			if(leftAlbum != rightAlbum)
				return leftAlbum < rightAlbum;
			if(leftTrack != rightTrack)
				return leftTrack < rightTrack;
			if(leftArtist != rightArtist)
				return leftArtist < rightArtist;
			return leftTitle < rightTitle;
		}
		//otherwise just compare strings
		return leftData.toString() < rightData.toString();
	} else if(leftData.canConvert(QVariant::Double)){
		return leftData.toDouble() < rightData.toDouble();
	} else
		return true;
}

WorkFilterModel * WorkFilterModelProxy::workFilter() const {
	return mFilter;
}

void WorkFilterModelProxy::setFilter(WorkFilterModel * model){
	mFilter = model;
	mFiltering = true;
}

bool WorkFilterModelProxy::filterAcceptsRow(int sourceRow,
		const QModelIndex &sourceParent) const {
	if(!mFiltering || !mFilter)
		return true;
	else {
		QModelIndex rowIndex = sourceModel()->index(sourceRow, AudioWorkTableModel::idColumn, sourceParent);
		return mFilter->acceptsWork(rowIndex.data().toInt());
	}
}

void WorkFilterModelProxy::filter(bool filter){
	if(mFiltering != filter){
		mFiltering = filter;
		invalidateFilter();
	}
}

void WorkFilterModelProxy::clearFilter(){
	if(mFiltering)
		filter(false);
}

