#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget* const parent)
  : QMainWindow{parent}, ui{new Ui::MainWindow{}}
{
  this->ui->setupUi(this);

  this->canvas = new PaintCanvas{this};
  this->setCentralWidget(this->canvas);

  const QFileInfo exeInfo{QCoreApplication::applicationFilePath()};
  this->currentFilePath = exeInfo.dir().filePath(
    QStringLiteral("%1.png").arg(exeInfo.completeBaseName()));

  // set action icons
  this->ui->actionNew->setIcon(QIcon{":/images/new.png"});
  this->ui->actionLoad->setIcon(QIcon{":/images/load.png"});
  this->ui->actionSave->setIcon(QIcon{":/images/save.png"});
  this->ui->actionSaveAs->setIcon(QIcon{":/images/save-as.png"});
  this->ui->actionExit->setIcon(QIcon{":/images/exit.png"});

  // connect menu actions
  this->connect(
    this->ui->actionNew,
    &QAction::triggered,
    this,
    &MainWindow::newFile);
  this->connect(
    this->ui->actionLoad,
    &QAction::triggered,
    this,
    &MainWindow::loadFile);
  this->connect(
    this->ui->actionSave,
    &QAction::triggered,
    this,
    &MainWindow::saveFile);
  this->connect(
    this->ui->actionSaveAs,
    &QAction::triggered,
    this,
    &MainWindow::saveFileAs);
  this->connect(
    this->ui->actionExit,
    &QAction::triggered,
    this,
    &MainWindow::exitApp);

  QLabel* const penWidthLabel{new QLabel{"Pen Width", this}};
  this->penWidthSpinBox = new QSpinBox{this};
  this->penWidthSpinBox->setRange(1, 15);
  this->penWidthSpinBox->setValue(this->canvas->getPenWidth());

  QLabel* const penColorLabel{new QLabel{"Pen Color", this}};
  this->penColorButton = new QPushButton{this};

  QLabel* const fillColorLabel{new QLabel{"Fill Color", this}};
  this->fillColorButton = new QPushButton{this};

  this->fillCheckBox = new QCheckBox{"Fill Shape", this};
  this->fillCheckBox->setChecked(this->canvas->getFill());

  QPushButton* const modifyButton{new QPushButton{this}};
  modifyButton->setIcon(QIcon(":/images/modification.png"));

  QPushButton* const squareButton{new QPushButton{this}};
  squareButton->setIcon(QIcon(":/images/square.png"));

  QPushButton* const rectButton{new QPushButton{this}};
  rectButton->setIcon(QIcon(":/images/rectangle.png"));

  QPushButton* const triangleButton{new QPushButton{this}};
  triangleButton->setIcon(QIcon(":/images/triangle.png"));

  QPushButton* const circleButton{new QPushButton{this}};
  circleButton->setIcon(QIcon(":/images/circle.png"));

  this->connect(
    modifyButton,
    &QPushButton::clicked,
    this,
    [this]()
    {
      this->canvas->setTool(PaintCanvas::ToolType::Modify);
      this->statusBar()->showMessage("Current mode: Modification");
    });

  this->connect(
    squareButton,
    &QPushButton::clicked,
    this,
    [this]()
    {
      this->canvas->setTool(PaintCanvas::ToolType::Square);
      this->statusBar()->showMessage("Current mode: Square");
    });

  this->connect(
    rectButton,
    &QPushButton::clicked,
    this,
    [this]()
    {
      this->canvas->setTool(PaintCanvas::ToolType::Rect);
      this->statusBar()->showMessage("Current mode: Rectangle");
    });

  this->connect(
    triangleButton,
    &QPushButton::clicked,
    this,
    [this]()
    {
      this->canvas->setTool(PaintCanvas::ToolType::Triangle);
      this->statusBar()->showMessage("Current mode: Triangle");
    });

  this->connect(
    circleButton,
    &QPushButton::clicked,
    this,
    [this]()
    {
      this->canvas->setTool(PaintCanvas::ToolType::Ellipse);
      this->statusBar()->showMessage("Current mode: Circle");
    });

  this->connect(
    this->penWidthSpinBox,
    &QSpinBox::valueChanged,
    this,
    &MainWindow::penWidthChanged);
  this->connect(
    this->penColorButton,
    &QPushButton::clicked,
    this,
    &MainWindow::changePenColor);
  this->connect(
    this->fillColorButton,
    &QPushButton::clicked,
    this,
    &MainWindow::changeFillColor);
  this->connect(
    this->fillCheckBox,
    &QCheckBox::clicked,
    this,
    &MainWindow::changeFillProperty);

  this->ui->mainToolBar->addWidget(penWidthLabel);
  this->ui->mainToolBar->addWidget(this->penWidthSpinBox);
  this->ui->mainToolBar->addWidget(penColorLabel);
  this->ui->mainToolBar->addWidget(this->penColorButton);
  this->ui->mainToolBar->addWidget(fillColorLabel);
  this->ui->mainToolBar->addWidget(this->fillColorButton);
  this->ui->mainToolBar->addWidget(this->fillCheckBox);
  this->ui->mainToolBar->addSeparator();
  this->ui->mainToolBar->addWidget(modifyButton);
  this->ui->mainToolBar->addWidget(squareButton);
  this->ui->mainToolBar->addWidget(rectButton);
  this->ui->mainToolBar->addWidget(triangleButton);
  this->ui->mainToolBar->addWidget(circleButton);

  const auto applyUiColors = [this]()
  {
    QString css{
      QString{"background-color: %1"}.arg(this->canvas->getPenColor().name())};
    this->penColorButton->setStyleSheet(css);

    css =
      QString{"background-color: %1"}.arg(this->canvas->getFillColor().name());
    this->fillColorButton->setStyleSheet(css);
    this->fillCheckBox->setChecked(this->canvas->getFill());
    this->penWidthSpinBox->setValue(this->canvas->getPenWidth());
  };

  applyUiColors();

  const QFileInfo defaultFile{this->currentFilePath};
  if (defaultFile.isFile())
  {
    const QImage img{defaultFile.absoluteFilePath()};
    if (!img.isNull())
    {
      const QString meta{img.text("shapes")};
      this->canvas->loadFromSerialized(meta);
      applyUiColors();
    }
  }

  this->update();
}

MainWindow::~MainWindow()
{
}

void MainWindow::newFile()
{
  this->canvas->clearAll();
  this->currentFilePath.clear();

  this->statusBar()->showMessage(
    "New operation has been completed successfully");

  this->update();
}

void MainWindow::loadFile()
{
  const QString path{QFileDialog::getOpenFileName(
    this,
    this->tr("Load drawing"),
    QString{},
    this->tr("PNG Images (*.png)"))};

  if (path.isEmpty())
  {
    return;
  }

  QImage img{path};
  if (img.isNull())
  {
    QMessageBox::warning(this, tr("Load failed"), tr("Cannot load image."));
    return;
  }

  const QString meta{img.text("shapes")};
  this->canvas->loadFromSerialized(meta);
  this->currentFilePath = path;

  // sync UI with loaded settings
  this->penWidthSpinBox->setValue(this->canvas->getPenWidth());
  QString css{
    QString{"background-color: %1"}.arg(this->canvas->getPenColor().name())};
  this->penColorButton->setStyleSheet(css);
  css =
    QString{"background-color: %1"}.arg(this->canvas->getFillColor().name());
  this->fillColorButton->setStyleSheet(css);
  this->fillCheckBox->setChecked(this->canvas->getFill());

  this->statusBar()->showMessage(
    "Load operation has been completed successfully");

  this->update();
}

void MainWindow::saveFile()
{
  if (this->currentFilePath.isEmpty())
  {
    const QFileInfo exeInfo{QCoreApplication::applicationFilePath()};
    this->currentFilePath = exeInfo.dir().filePath(
      QStringLiteral("%1.png").arg(exeInfo.completeBaseName()));
  }

  QImage img{this->canvas->toImage()};
  img.setText("shapes", this->canvas->toSerialized());

  if (!img.save(this->currentFilePath, "PNG"))
  {
    QMessageBox::warning(this, tr("Save failed"), tr("Cannot save image."));
    return;
  }

  this->statusBar()->showMessage(
    "Save operation has been completed successfully");
}

void MainWindow::saveFileAs()
{
  const QString path{QFileDialog::getSaveFileName(
    this,
    tr("Save drawing as"),
    this->currentFilePath,
    tr("PNG Images (*.png)"))};

  if (path.isEmpty())
  {
    return;
  }

  QImage img{this->canvas->toImage()};
  img.setText("shapes", this->canvas->toSerialized());

  if (!img.save(path, "PNG"))
  {
    QMessageBox::warning(this, tr("Save failed"), tr("Cannot save image."));
    return;
  }

  this->currentFilePath = path;

  this->statusBar()->showMessage(
    "Save as operation has been completed successfully");
}

void MainWindow::exitApp()
{
  this->close();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  this->saveFile();
  QMainWindow::closeEvent(event);
}

void MainWindow::penWidthChanged(const int width)
{
  this->canvas->setPenWidth(width);

  this->statusBar()->showMessage(
    "Pen width change operation has been completed successfully");

  this->update();
}

void MainWindow::changePenColor()
{
  const QColor color{QColorDialog::getColor(this->canvas->getPenColor())};

  if (color.isValid())
  {
    this->canvas->setPenColor(color);
    const QString css{QString("background-color: %1").arg(color.name())};
    this->penColorButton->setStyleSheet(css);
  }

  this->statusBar()->showMessage(
    "Pen color change operation has been completed successfully");

  this->update();
}
void MainWindow::changeFillColor()
{
  const QColor color{QColorDialog::getColor(this->canvas->getPenColor())};

  if (color.isValid())
  {
    this->canvas->setFillColor(color);
    const QString css{QString{"background-color: %1"}.arg(color.name())};
    this->fillColorButton->setStyleSheet(css);
  }

  this->statusBar()->showMessage(
    "Fill color change operation has been completed successfully");

  this->update();
}

void MainWindow::changeFillProperty()
{
  this->canvas->setFill(this->fillCheckBox->isChecked());

  this->update();
}
