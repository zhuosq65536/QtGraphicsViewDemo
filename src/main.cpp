#include "mainwindow.h"
#include <QApplication>
// 保留编码头文件，但仅使用未废弃的函数
#include <QTextCodec>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QTextCodec* codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForLocale(codec); // 仅保留这一行（核心）

  MainWindow window;
  window.show();

  return app.exec();
}