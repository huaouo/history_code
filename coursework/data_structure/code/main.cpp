#include <QApplication>

#include "main_widget.h"
#include "tree_viewer.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    TreeViewer t;  //AVL树视图窗口
    t.show();

    MainWidget w(t);  //主窗口
    w.show();

    return app.exec();
}
