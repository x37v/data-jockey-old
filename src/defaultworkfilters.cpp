#include "defaultworkfilters.hpp"
#include "applicationmodel.hpp"
#include "mixerpanelmodel.hpp"
#include "mastermodel.hpp"
#include <math.h>

TagSelectionFilter::TagSelectionFilter(ApplicationModel * appModel, QObject * parent):
	WorkFilterModel(appModel, parent), mQuery("", appModel->db())
{
}

bool TagSelectionFilter::beforeFilter(){
	mSelectedWorks.clear();
	if(mSelectedTags.size() > 0){
		QString queryStr("select audio_work_id from audio_work_tags where ");
		QString tag_id;

		//add the first one
		queryStr.append("tag_id = ");
		tag_id.setNum(mSelectedTags[0]);
		queryStr.append(tag_id);

		//do the rest
		for(int i = 1; i < mSelectedTags.size(); i++){
			queryStr.append(" or tag_id = ");
			tag_id.setNum(mSelectedTags[i]);
			queryStr.append(tag_id);
		}
		//execute the query
		mQuery.exec(queryStr);
		while(mQuery.next())
			mSelectedWorks.insert(mQuery.value(0).toInt());
	}
	return true;
}

bool TagSelectionFilter::acceptsWork(int work_id){
	//if there are no works or tags selected let all through
	//if there are no works selected but there are tags selected 
	//[ie tags which have no associations with works]
	//don't let the works through
	if(mSelectedWorks.empty()){
		if(mSelectedTags.empty())
			return true;
		else
			return false;
	} else if(mSelectedWorks.find(work_id) != mSelectedWorks.end())
		return true;
	else
		return false;
}

std::string TagSelectionFilter::description(){
	return "Filters works based on selections in the tag view.  "
		"It shows only those works which have at least one of the tags that the user has selected.  "
		"If there are no tags selected, it shows all works.";
}

std::string TagSelectionFilter::name(){
	return "Tag Selection Filter";
}

void TagSelectionFilter::addTag(int tag_id){
	mSelectedTags.push_back(tag_id);
}

void TagSelectionFilter::clearTags(){
	mSelectedTags.clear();
	mSelectedWorks.clear();
}

void TagSelectionFilter::setTags(QList<int> tags){
	mSelectedTags.clear();
	for(int i = 0; i < tags.size(); i++)
		mSelectedTags.push_back(tags[i]);
}

TempoRangeFilter::TempoRangeFilter(ApplicationModel * appModel, QObject * parent) :
	WorkFilterModel(appModel, parent), mQuery("", appModel->db())
{
	mBelow = 7.0f;
	mAbove = 7.0f;
	mPrevTempo = -1.0f;
}

/*
select audio_works.id from audio_works 
join descriptors on audio_works.id = descriptors.audio_work_id
join descriptor_types on descriptors.descriptor_type_id = descriptor_types.id
where descriptor_types.name = 'tempo median'
and descriptors.float_value > 120.0 and descriptors.float_value < 130.0
*/

bool TempoRangeFilter::beforeFilter(){
	//get the current tempo
	float tempo = applicationModel()->mixerPanel()->master()->tempo();
	if(mPrevTempo == tempo)
		return false;
	mPrevTempo = tempo;

	mSelectedWorks.clear();
	QString queryStr = QString("select audio_works.id from audio_works\n"
			"\tjoin descriptors on audio_works.id = descriptors.audio_work_id\n"
			"\tjoin descriptor_types on descriptors.descriptor_type_id = descriptor_types.id\n"
			"where descriptor_types.name = 'tempo median'\n"
			"AND descriptors.float_value > %1 AND descriptors.float_value < %2").arg( 
			tempo - mBelow).arg(tempo + mAbove);
	//execute the query
	mQuery.exec(queryStr);
	while(mQuery.next())
		mSelectedWorks.insert(mQuery.value(0).toInt());

	return true;
}

bool TempoRangeFilter::acceptsWork(int work_id){
	if(mSelectedWorks.find(work_id) != mSelectedWorks.end())
		return true;
	else
		return false;
}

std::string TempoRangeFilter::description(){
	QString above;
	QString below;
	above.setNum(mAbove, 'g', 3);
	below.setNum(mBelow, 'g', 3);
	QString text = QString("Filters works based on their tempo and the current master tempo.  "
			"If a work's tempo is above the master tempo - %1 bpm"
			" and below the master tempo + %2 bpm the work is displayed.").arg(below, above);
	return text.toStdString();
}

std::string TempoRangeFilter::name(){
	return "Tempo Range Filter";
}

void TempoRangeFilter::setRange(float below, float above){
	mBelow = fabsf(below);
	mAbove = fabsf(above);
}

