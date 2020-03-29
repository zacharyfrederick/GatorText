#include "changefontsizedialog.h"
#include "ui_changefontsizedialog.h"

ChangeFontSizeDialog::ChangeFontSizeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeFontSizeDialog)
{
    ui->setupUi(this);

    changeSizeBtn = ui->changeSizeBtn;
    sizeSpinner = ui->fontSpinner;

    connect(changeSizeBtn, &QPushButton::pressed, this, &ChangeFontSizeDialog::changeSize);
}

ChangeFontSizeDialog::~ChangeFontSizeDialog()
{
    delete ui;
}

void ChangeFontSizeDialog::changeSize() {
    int newSize = sizeSpinner->value();
    QFont font = editorPtr->font();
    font.setPointSize(newSize);
    editorPtr->setFont(font);

    this->close();
}

//TODO rewrite editor to have a get font and set font function
void ChangeFontSizeDialog::setEditor(CodeEditor *editor) {
    editorPtr = editor;
    QFont font = editor->font();
    int size = font.pointSize();
    sizeSpinner->setValue(size);
}
