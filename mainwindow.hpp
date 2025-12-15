#pragma once

#include "paintcanvas.hpp"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QCheckBox>
#include <QCloseEvent>
#include <QColorDialog>
#include <QCoreApplication>
#include <QFileDialog>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

QT_BEGIN_NAMESPACE
namespace Ui
{
  class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* const parent = nullptr);
  ~MainWindow();

private slots:
  void penWidthChanged(const int width);
  void changePenColor();
  void changeFillColor();
  void changeFillProperty();

  // File toolbar menu options
  void newFile();
  void loadFile();
  void saveFile();
  void saveFileAs();
  void exitApp();

private:
  void closeEvent(QCloseEvent* event) override;

  std::unique_ptr<Ui::MainWindow> ui{nullptr};
  PaintCanvas* canvas{nullptr};
  QPushButton* penColorButton{nullptr};
  QPushButton* fillColorButton{nullptr};
  QCheckBox* fillCheckBox{nullptr};
  QSpinBox* penWidthSpinBox{nullptr};
  QString currentFilePath{};
};
