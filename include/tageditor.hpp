#ifndef TAG_EDITOR_HPP
#define TAG_EDITOR_HPP

#include <QWidget>
#include <QSpinBox>

class TagModel;
class TagView;
class QPushButton;
class QLineEdit;
class QComboBox;

class TagEditor : public QWidget {
	Q_OBJECT
	public:
		TagEditor(TagModel * model, QWidget * parent = NULL);
	signals:
		//add a tag to an existing class
		void tagAdded(int classIndex, QString tagName);
		//create a new class with this tag
		void tagAdded(QString className, QString tagName);
		void tagSelectionChanged(QList<int> tags_selected);
	protected slots:
		void addTagButtonPushed();
		void addToClassList(QPair<int, QString> item);
	private:
		TagView * mTagView;
		QPushButton * mAddTagBtn;
		QComboBox * mTagClassSelect;
		QLineEdit * mTagNameInput;
};

#endif

