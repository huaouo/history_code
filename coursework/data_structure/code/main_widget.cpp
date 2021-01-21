#include <QTextStream>
#include <QString>
#include <QFile>
#include <QQueue>
#include <QPair>

#include "main_widget.h"
#include "ui_main_widget.h"

MainWidget::MainWidget(TreeViewer& palette, QWidget* parent) :
    QWidget(parent),
    palette(palette),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::closeEvent(QCloseEvent* e)
{
    palette.close();
}

void MainWidget::on_loadDataButton_clicked()
{
    QString dataPath = ui->filenameEditor->text();

    QFile file(dataPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->operationStatusLabel->setText("文件打开失败");
        return;
    }

    QTextStream in(&file);
    QString rawStream = in.readAll().trimmed();
    QStringList list = rawStream.split(QRegExp("\\s+"));

    delete palette.tree;
    palette.tree = new huaouo::AVLTree<QString, QString>();

    QString username;
    QString password;
    bool hasFailure = false;
    for (int i = 0; i + 1 < list.size(); i += 2) {
        username = list.at(i);
        password = list.at(i + 1);
        if (palette.tree->insert(username, password) == FAILURE) {
            hasFailure = true;
        }
    }

    if(hasFailure) {
        ui->operationStatusLabel->setText("数据加载完成，但部分数据载入错误");
    } else {
        ui->operationStatusLabel->setText("数据加载成功");
    }
    palette.update();
}

void MainWidget::on_loginButton_clicked()
{
    QString username = ui->usernameEditor->text();
    QString password = ui->passwordEditor->text();

    if (username == "" || password == "") {
        ui->operationStatusLabel->setText("用户名和密码均不能为空");
        return;
    }

    const QString* result = palette.tree->find(username);

    if (result != nullptr && *result == password) {
        ui->operationStatusLabel->setText("登录成功");
    } else if (result != nullptr) {
        ui->operationStatusLabel->setText("密码错误，登录失败");
    } else {
        ui->operationStatusLabel->setText("用户不存在，登录失败");
    }
}

void MainWidget::on_userModifyButton_clicked()
{
    QString username = ui->usernameEditor->text();
    QString password = ui->passwordEditor->text();

    if (username == "") {
        ui->operationStatusLabel->setText("用户名不能为空");
        return;
    }

    OperationState state = palette.tree->update(username, password);

    if (state == FAILURE) {
        ui->operationStatusLabel->setText("用户不存在，密码修改失败");
    } else {
        ui->operationStatusLabel->setText("密码修改成功");
    }
}

void MainWidget::on_userAddButton_clicked()
{
    QString username = ui->usernameEditor->text();
    QString password = ui->passwordEditor->text();

    if (username == "" || password == "") {
        ui->operationStatusLabel->setText("用户名和密码均不能为空");
        return;
    }

    OperationState state = palette.tree->insert(username, password);

    if(state == FAILURE) {
        ui->operationStatusLabel->setText("用户已存在，添加失败");
    } else {
        ui->operationStatusLabel->setText("添加用户成功");
        palette.update();
    }
}

void MainWidget::on_userDeleteButton_clicked()
{
    QString username = ui->usernameEditor->text();

    if (username == "") {
        ui->operationStatusLabel->setText("用户名不能为空");
        return;
    }

    OperationState state = palette.tree->remove(username);

    if(state == FAILURE) {
        ui->operationStatusLabel->setText("用户不存在，删除失败");
    } else {
        ui->operationStatusLabel->setText("删除用户成功");
        palette.update();
    }
}

void MainWidget::on_saveDataButton_clicked()
{
    if (palette.tree->root == nullptr) {
        ui->operationStatusLabel->setText("无数据可供保存");
        return;
    }

    QString dataPath = ui->filenameEditor->text();

    QFile file(dataPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ui->operationStatusLabel->setText("文件打开失败");
        return;
    }

    QTextStream out(&file);

    if (palette.tree->root != nullptr) {
        QQueue<huaouo::Node<QString, QString>*> queue;
        queue.push_back(palette.tree->root);

        while (!queue.empty()) {
            auto nodePtr = queue.front();
            queue.pop_front();
            out << nodePtr->key << ' ' << nodePtr->val << '\n';
            if (nodePtr->left != nullptr) {
                queue.push_back(nodePtr->left);
            }
            if (nodePtr->right != nullptr) {
                queue.push_back(nodePtr->right);
            }
        }
    }

    file.close();
    ui->operationStatusLabel->setText("数据保存成功");
}

void MainWidget::on_resetButton_clicked()
{
    delete palette.tree;
    palette.tree = new huaouo::AVLTree<QString, QString>();
    palette.update();

    ui->operationStatusLabel->setText("重置AVL树视图成功");
}
