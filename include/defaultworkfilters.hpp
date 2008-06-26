#ifndef DEFAULT_WORK_FILTERS_HPP
#define DEFAULT_WORK_FILTERS_HPP

#include "workfiltermodel.hpp"
#include <set>
#include <QList>
#include <QSqlQuery>

class ApplicationModel;

class TagSelectionFilter : public WorkFilterModel {
	Q_OBJECT
	public:
		TagSelectionFilter(ApplicationModel * appModel, QObject * parent = NULL);
		virtual bool beforeFilter();
		virtual bool acceptsWork(int work_id);
		virtual std::string description();
		virtual std::string name();
	public slots:
		void addTag(int tag_id);
		void clearTags();
		void setTags(QList<int> tags);
	private:
		QSqlQuery mQuery;
		QList<int> mSelectedTags;
		std::set<int> mSelectedWorks;
};

class TempoRangeFilter : public WorkFilterModel {
	Q_OBJECT
	public:
		TempoRangeFilter(ApplicationModel * appModel, QObject * parent = NULL);
		virtual bool beforeFilter();
		virtual bool acceptsWork(int work_id);
		virtual std::string description();
		virtual std::string name();
	public slots:
		void setRange(float below, float above);
	private:
		QSqlQuery mQuery;
		std::set<int> mSelectedWorks;
		float mBelow;
		float mAbove;
		float mPrevTempo;
};


#endif

