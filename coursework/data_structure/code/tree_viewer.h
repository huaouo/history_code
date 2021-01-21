#ifndef TREE_VIEWER_H
#define TREE_VIEWER_H

#include <QWidget>
#include <QString>
#include <QPainter>

#include "avl_tree.hpp"

namespace Ui {
class TreeViewer;
}

class TreeViewer : public QWidget
{
    Q_OBJECT

public:
    explicit TreeViewer(QWidget* parent = nullptr);
    ~TreeViewer();
    void paintEvent(QPaintEvent* e) override;

    huaouo::AVLTree<QString, QString>* tree = new huaouo::AVLTree<QString, QString>();

private:
    void paintLine(int rootX, int rootY, huaouo::Node<QString, QString>* root);
    void paintNode(int rootX, int rootY, huaouo::Node<QString, QString>* root);

    const int DIAMETER = 50;  //作图过程中相关参数
    const int GAP = DIAMETER;
    const int INIT_X_STEP = 0.5*DIAMETER + 0.5*GAP;
    const int Y_STEP = 100;

    Ui::TreeViewer* ui;
    QPainter* painter = new QPainter();  //画笔，在多个函数内被用到
};

#endif // TREE_VIEWER_H
