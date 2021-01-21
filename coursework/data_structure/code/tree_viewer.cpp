#include "tree_viewer.h"
#include "ui_tree_viewer.h"

TreeViewer::TreeViewer(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::TreeViewer)
{
    ui->setupUi(this);
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
}

TreeViewer::~TreeViewer()
{
    delete tree;
    delete painter;
    delete ui;
}

void TreeViewer::paintEvent(QPaintEvent* e)
{
    if (tree->root == nullptr) {
        return;
    }

    painter->begin(this);
    QPen blackPen(Qt::black, 3);
    painter->setPen(blackPen);
    QBrush colorBrush(QColor(188, 159, 119), Qt::SolidPattern);
    painter->setBrush(colorBrush);

    int baseX = 100 + ((1<<(tree->root->height - 1)) - 1)*INIT_X_STEP;  //等比求和
    int baseY = 100;

    paintLine(baseX, baseY, tree->root);
    paintNode(baseX, baseY, tree->root);

    painter->end();
}

void TreeViewer::paintLine(int rootX, int rootY, huaouo::Node<QString, QString>* root)
{
    int currentLeftX = rootX - (INIT_X_STEP*(1<<(root->height - 2)));
    int currentRightX = rootX + (INIT_X_STEP*(1<<(root->height - 2)));
    int currentY = rootY + Y_STEP;

    if(root->left != nullptr) {
        painter->drawLine(QPoint(rootX, rootY), QPoint(currentLeftX, currentY));
        paintLine(currentLeftX, currentY, root->left);
    }
    if(root->right != nullptr) {
        painter->drawLine(QPoint(rootX, rootY), QPoint(currentRightX, currentY));
        paintLine(currentRightX, currentY, root->right);
    }
}

void TreeViewer::paintNode(int rootX, int rootY, huaouo::Node<QString, QString>* root)
{
    int currentLeftX = rootX - (INIT_X_STEP*(1<<(root->height - 2)));
    int currentRightX = rootX + (INIT_X_STEP*(1<<(root->height - 2)));
    int currentY = rootY + Y_STEP;

    painter->drawEllipse(QPoint(rootX, rootY), DIAMETER/2, DIAMETER/2);
    painter->drawText(QPoint(rootX - 10, rootY + 5), root->key);
    if(root->left != nullptr) {
        paintNode(currentLeftX, currentY, root->left);
    }

    if(root->right != nullptr) {
        paintNode(currentRightX, currentY, root->right);
    }
}
