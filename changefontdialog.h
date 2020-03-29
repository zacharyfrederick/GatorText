#ifndef CHANGEFONTDIALOG_H
#define CHANGEFONTDIALOG_H

#include <QDialog>
#include <QFontComboBox>
#include <QPushButton>
#include "codeeditor.h"

/* Fileheader for the change font dialog. Responsible for presenting the font options
 * updating to show the current font and updating the font within the code editor
 *
 * */

namespace Ui {
class ChangeFontDialog;
}

class ChangeFontDialog : public QDialog
{
    Q_OBJECT

public:
    //constructor
    explicit ChangeFontDialog(QWidget *parent = 0);

    //destructor
    ~ChangeFontDialog();

    //function used to set the editor allowing it to access current font
    void setEditor(CodeEditor* editor);

    //UI components found on the dialog
    QFontComboBox* fontBox;
    QPushButton* changeFontButton;

    //editor pointer
    CodeEditor* editorPtr;

private:
    //creates the ui
    Ui::ChangeFontDialog *ui;

    //slot for the button
    void changeFont();
};

#endif // CHANGEFONTDIALOG_H
