#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include <QWidget>
#include <QTextStream>

#include "tree_viewer.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(TreeViewer& palette, QWidget* parent = nullptr);
    ~MainWidget();
    void closeEvent(QCloseEvent* e) override;

private slots:
    void on_loadDataButton_clicked();
    void on_loginButton_clicked();
    void on_userModifyButton_clicked();
    void on_userAddButton_clicked();
    void on_userDeleteButton_clicked();
    void on_saveDataButton_clicked();
    void on_resetButton_clicked();

private:
    TreeViewer& palette;  //AVL树视图界面对象引用
    Ui::MainWidget* ui;
};

#endif // MAIN_WIDGET_H
