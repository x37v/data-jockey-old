#include "workfilterlist.hpp"
#include "workfiltermodel.hpp"

WorkFilterList::WorkFilterList(QObject * parent) :
	QObject(parent)
{
	mSelected = -1;
}

WorkFilterModel * WorkFilterList::selected() const {
	if(mSelected < 0 || mSelected >= mModelList.size())
		return NULL;
	else
		return mModelList[mSelected];
}

unsigned int WorkFilterList::selectedIndex() const {
	return mSelected;
}

QList<WorkFilterModel *> WorkFilterList::list() const {
	return mModelList;
}

WorkFilterModel * WorkFilterList::at(unsigned int i) const {
	if(i < (unsigned int)mModelList.size())
		return mModelList[i];
	else
		return NULL;
}

unsigned int WorkFilterList::size() const {
	return (unsigned int)mModelList.size();
}

void WorkFilterList::addFilter(WorkFilterModel * filter){
	if(filter){
		//select the first filter
		if(mSelected < 0)
			mSelected = 0;
		mModelList.push_back(filter);
		emit(filterAdded(mModelList.size()));
	}
}

void WorkFilterList::removeFilter(unsigned int index){
	if(index < (unsigned int)mModelList.size()){
		mModelList.removeAt(index);
		if(mModelList.size() == 0)
			mSelected = -1;
		emit(filterRemoved(index));
	}
}

void WorkFilterList::removeFilter(WorkFilterModel * filter){
	int index = mModelList.indexOf(filter);
	if(index >= 0)
		removeFilter((unsigned int)index);
}

void WorkFilterList::selectFilter(unsigned int index){
	if(index < (unsigned int)mModelList.size()){
		unsigned int old = mSelected;
		mSelected = index;
		emit(selectionChanged(mSelected, old));
	}
}

void WorkFilterList::selectFilter(WorkFilterModel * filter){
	int index = mModelList.indexOf(filter);
	if(index >= 0)
		selectFilter((unsigned int)index);
}

