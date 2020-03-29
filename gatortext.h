#ifndef GATORTEXT_H
#define GATORTEXT_H

#include <QMainWindow>
#include "codeeditor.h"
#include <QMenu>
#include <QToolBar>
#include "highlighter.h"
#include <QString>
#include <QDockWidget>
#include <QSignalMapper>
#include "languagetypes.h"
//#include <QWebEngineView> If i include this in the header file it wont compile. Its such a wear bug. seriously dont touch this shit
//#include <QtWebView/QtWebView>


/*
 *header file got GatorText. represents the heart of the program and is the main window.
 * childs the main window class
*/
class GatorText: public QMainWindow {
public:
    GatorText();
    void loadFile(const QString& filename);
    bool checkForSave();
    bool currentFileExist();

private:
    void createActions();
    void createMenus();
    bool saveFile(const QString& filename);
    void setCurrentFile(const QString& filename);
    void createWebDock();
    void createWordDock();
    void updateMasterList();
    void updateAdditionalItems();
    void changeWebUrl(QString url);
    QString mapLanguageToEnum(LanguageType type);

    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* helpMenu;
    QMenu* viewMenu;
    QMenu* toolsMenu;
    QMenu* optionsMenu;
    QMenu* languageMenu;
    QToolBar* toolBar;
    CodeEditor* editor;
    Highlighter* highlighter;
    QDockWidget* dock;
    QString currentFile = "";
    const QString url = "https://devdocs.io/";
    QString urlModifier = "cpp"; //default value
    //QWebEngineView* webEngine;
    LanguageType selectedLanguage;
    QStringList* masterListPtr;
    QStringList* additionalItemsPtr;
    bool codeWordSelected = false;

private slots:
    void newFile();
    void openFile();
    bool save();
    bool saveAs();
    void cut();
    void copy();
    void paste();
    void find();
    void addIndentation();
    void changeFont();
    void changeFontSize();
    void languageChanged(LanguageType type);
    void changeTheme(QString theme);
    void about();
    void commentOutCode();
    void unCommentOutCode();
    void quitProgram();
    void toggleCodeViewer();
    void highlightSelectedWord(QListWidgetItem* item);
    void unHighlightSelectedWord(QListWidgetItem* item);
    void listItemContextSelected(const QPoint &point);

protected:
    void closeEvent(QCloseEvent *event) override;

};
#endif // GATORTEXT_H
