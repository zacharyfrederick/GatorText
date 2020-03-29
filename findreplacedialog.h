#ifndef FINDREPLACEDIALOG_H
#define FINDREPLACEDIALOG_H

#include <QDialog>
#include <QTextCursor>
#include <QlineEdit>
#include <QCheckBox>
#include "codeeditor.h"

//header for the find and replace dialog. Allows you to use regular expressions to find words and phrases
namespace Ui {
class FindReplaceDialog;
}

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget *parent = 0);
    ~FindReplaceDialog();
    void setEditor(CodeEditor* editor);

private:
    //ui components
    Ui::FindReplaceDialog *ui;
    QTextCursor cursor;
    QLineEdit* findTextEdit;
    QLineEdit* replaceTextEdit;
    QPushButton* findBtn;
    QPushButton* replaceBtn;
    QPushButton* replaceAllBtn;
    QPushButton* cancelBtn;
    QLabel* dataLabel;
    QString previousFind;
    CodeEditor* editor;

    //used to store where formatting has been applied so that it can be removed when the dialog is destroyed in the destructor
    int oldFormatPosition = -1;
    int oldFormatEndPosition;
    bool wordFoundBefore = false;
    bool replacedOnce = false;

public slots:
    //slots for all the buttons
    void findNext();
    void replace();
    void replaceAll();

protected:
    void closeEvent(QCloseEvent *e) override;

};

#endif // FINDREPLACEDIALOG_H
