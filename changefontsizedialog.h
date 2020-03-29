#ifndef CHANGEFONTSIZEDIALOG_H
#define CHANGEFONTSIZEDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QSpinBox>
#include "codeeditor.h"

/*header file for the change font size dialog
 * */
namespace Ui {
class ChangeFontSizeDialog;
}

class ChangeFontSizeDialog : public QDialog
{
    Q_OBJECT

public:
    //constructor destructor
    explicit ChangeFontSizeDialog(QWidget *parent = 0);
    ~ChangeFontSizeDialog();

    //used to set the editor so we can access font size
    void setEditor(CodeEditor* editor);

private:
    //ui methods and buttons
    Ui::ChangeFontSizeDialog *ui;
    QPushButton* changeSizeBtn;
    QSpinBox* sizeSpinner;

    //code editor pointer
    CodeEditor* editorPtr;

    //slot function to change font size
    void changeSize();
};

#endif // CHANGEFONTSIZEDIALOG_H
