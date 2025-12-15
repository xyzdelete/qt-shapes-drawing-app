#pragma once

#include <QApplication>
#include <QClipboard>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMimeData>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QUrl>
#include <QWidget>
#include <QtMath>

// C++ standard
#include <algorithm>
#include <memory>
#include <ranges>

class PaintCanvas : public QWidget
{
  Q_OBJECT
public:
  enum class ToolType
  {
    Modify,
    Square,
    Rect,
    Triangle,
    Ellipse,
  };

  explicit PaintCanvas(QWidget* const parent = nullptr);

  ToolType getTool() const;
  void setTool(const ToolType& newTool);
  bool getFill() const;
  void setFill(const bool newFill);
  bool isDrawingEnabled() const;
  void setDrawingEnabled(const bool isDrawingEnabled);
  int getPenWidth() const;
  void setPenWidth(const int newPenWidth);
  QColor getFillColor() const;
  void setFillColor(const QColor& newFillColor);
  QColor getPenColor() const;
  void setPenColor(const QColor& newPenColor);
  QPointF getLastPoint() const;
  void setLastPoint(const QPointF& newLastPoint);
  QPointF getLastPos() const;
  void setLastPos(const QPointF& newLastPos);
  QRectF getLastRect() const;
  void setLastRect(const QRectF& newLastRect);
  QImage getImage() const;
  void setImage(const QImage& newImage);

  void clearAll();

  QImage toImage() const;
  QString toSerialized() const;
  void loadFromSerialized(const QString& json);
  bool isMoved() const;
  void setMoved(const bool isMoved);

  QPointF getDragStart() const;
  void setDragStart(const QPointF& newDragStart);

  bool isSelected() const;
  void setSelected(bool isSelected);

  QPointF getSelectionStart() const;
  void setSelectionStart(const QPointF& newSelectionStart);

  QRectF getSelectionRect() const;
  void setSelectionRect(const QRectF& newSelectionRect);

  bool isRotated() const;
  void setRotated(const bool isRotated);
  QPointF getRotateAnchor() const;
  void setRotateAnchor(const QPointF& newRotateAnchor);

  bool isCloned() const;
  void setCloned(const bool isCloned);

  bool isClonesCreated() const;
  void setClonesCreated(const bool isClonesCreated);

private:
  struct Shape
  {
    ToolType type{ToolType::Rect};
    QVector<QPointF> points{};
    qreal rotation{0.0};
    bool isSelected{false};
    QColor pen{Qt::black};
    QColor fill{Qt::gray};
    int width{3};
  };

  ToolType tool{ToolType::Modify};
  bool fill{false};
  bool drawingEnabled{false};
  int penWidth{3};
  QColor fillColor{Qt::gray};
  QColor penColor{Qt::black};
  QPointF lastPoint{};
  QPointF lastPos{};
  QRectF lastRect{};
  QImage image{};

  QVector<Shape> shapes;
  QVector<Shape> clones;
  QVector<QPointF> trianglePoints;
  QRectF selectionRect{};
  QPointF selectionStart{};
  QPointF dragStart{};
  QPointF rotateAnchor{};
  bool selected{false};
  bool moved{false};
  bool rotated{false};
  bool cloned{false};
  bool clonesCreated{false};

  QPainterPath shapePath(const Shape& s) const;
  QPointF shapeCenter(const Shape& s) const;
  QRectF shapeBounds(const Shape& s) const;

  bool isImage(const QString& fullpath) const;
  void resizeImage(QImage* const image, const QSize& newSize);
  void drawRectTo(const QPointF& endPoint, bool ellipse = false);

  Shape
  makeRectShape(const QPointF& a, const QPointF& b, const ToolType& t) const;
  Shape makeSquareShape(const QPointF& center, const QPointF& cursor) const;
  Shape makeEllipseShape(const QPointF& center, const QPointF& cursor) const;
  Shape makeTriangleShape(const QVector<QPointF>& pts) const;

  bool hitTest(const Shape& s, const QPointF& p) const;
  void clearSelections();
  void selectShape(Shape& s, const bool add);
  void applySelectionRect(const bool add);
  void moveSelected(const QPointF& delta);
  void rotateSelected(const QPointF& start, const QPointF& now);
  void cloneSelected();
  void deleteSelected();
  Shape* topHit(const QPointF& p);

  QJsonObject shapeToJson(const Shape& s) const;
  Shape jsonToShape(const QJsonObject& obj) const;

protected:
  virtual void mousePressEvent(QMouseEvent* event) override;
  virtual void mouseReleaseEvent(QMouseEvent* event) override;
  virtual void mouseMoveEvent(QMouseEvent* event) override;
  virtual void paintEvent(QPaintEvent* event) override;
  virtual void resizeEvent(QResizeEvent* event) override;
  virtual void keyPressEvent(QKeyEvent* event) override;
};
