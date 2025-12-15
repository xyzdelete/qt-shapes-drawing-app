#include "paintcanvas.hpp"

PaintCanvas::PaintCanvas(QWidget* const parent) : QWidget{parent}
{
  this->setAcceptDrops(false);
  this->update();
}

PaintCanvas::ToolType PaintCanvas::getTool() const
{
  return this->tool;
}

void PaintCanvas::setTool(const ToolType& newTool)
{
  this->tool = newTool;
}

bool PaintCanvas::getFill() const
{
  return this->fill;
}

void PaintCanvas::setFill(const bool newFill)
{
  this->fill = newFill;
}

bool PaintCanvas::isDrawingEnabled() const
{
  return this->drawingEnabled;
}

void PaintCanvas::setDrawingEnabled(const bool isDrawingEnabled)
{
  this->drawingEnabled = isDrawingEnabled;
}

int PaintCanvas::getPenWidth() const
{
  return this->penWidth;
}

void PaintCanvas::setPenWidth(const int newPenWidth)
{
  this->penWidth = newPenWidth;
}

QColor PaintCanvas::getFillColor() const
{
  return this->fillColor;
}

void PaintCanvas::setFillColor(const QColor& newFillColor)
{
  this->fillColor = newFillColor;
}

QColor PaintCanvas::getPenColor() const
{
  return this->penColor;
}

void PaintCanvas::setPenColor(const QColor& newPenColor)
{
  this->penColor = newPenColor;
}

QPointF PaintCanvas::getLastPoint() const
{
  return this->lastPoint;
}

void PaintCanvas::setLastPoint(const QPointF& newLastPoint)
{
  this->lastPoint = newLastPoint;
}

QRectF PaintCanvas::getLastRect() const
{
  return this->lastRect;
}

void PaintCanvas::setLastRect(const QRectF& newLastRect)
{
  this->lastRect = newLastRect;
}

QImage PaintCanvas::getImage() const
{
  return this->image;
}

void PaintCanvas::setImage(const QImage& newImage)
{
  this->image = newImage;
}

void PaintCanvas::resizeImage(QImage* const image, const QSize& newSize)
{
  if (image->size() == newSize)
  {
    return;
  }

  QImage newImage{newSize, QImage::Format_RGB32};
  newImage.fill(qRgb(255, 255, 255));
  QPainter painter(&newImage);
  painter.drawImage(QPoint(0, 0), *image);
  *image = newImage;

  this->update();
}

void PaintCanvas::drawRectTo(const QPointF& endPoint, bool ellipse)
{
  QPainter painter{&(this->image)};

  painter.setPen(QPen(
    this->getPenColor(),
    this->getPenWidth(),
    Qt::SolidLine,
    Qt::RoundCap,
    Qt::RoundJoin));

  if (this->getFill())
  {
    painter.setBrush(this->getFillColor());
  }
  else
  {
    painter.setBrush(Qt::NoBrush);
  }

  if (!ellipse)
  {
    painter.drawRect(QRectF{this->getLastPoint(), endPoint});
  }
  else
  {
    painter.drawEllipse(QRectF{this->getLastPoint(), endPoint});
  }

  this->setLastRect(QRectF{this->getLastPoint(), endPoint});

  // Trigger a call to paintEvent
  this->update();
}

bool PaintCanvas::isImage(const QString& fullpath) const
{
  const QFileInfo file{fullpath};
  const QString& suffix{file.suffix().toLower()};
  return suffix == "png" || suffix == "jpg" || suffix == "jpeg";
}

void PaintCanvas::mousePressEvent(QMouseEvent* event)
{
  event->accept();
  this->setFocus();
  this->setLastPos(event->pos());

  if (this->getTool() == ToolType::Modify)
  {
    const bool ctrl{event->modifiers().testFlag(Qt::ControlModifier)};
    const bool anySelected{std::ranges::any_of(
      this->shapes,
      [](const Shape& s)
      {
        return s.isSelected;
      })};

    if (event->button() == Qt::LeftButton)
    {
      Shape* const hit{topHit(event->pos())};
      if (hit != nullptr)
      {
        const bool keepGroup{hit->isSelected && !ctrl};
        if (!keepGroup)
        {
          selectShape(*hit, ctrl);
        }
        this->setMoved(hit->isSelected);
        this->setDragStart(event->pos());
      }
      else
      {
        this->setSelected(true);
        this->setSelectionStart(event->pos());
        this->setSelectionRect(
          QRectF{this->getSelectionStart(), this->getSelectionStart()});
        if (!ctrl)
        {
          clearSelections();
        }
      }
    }
    else if (event->button() == Qt::RightButton)
    {
      Shape* const hit{topHit(event->pos())};
      if (hit != nullptr)
      {
        const bool keepGroup{hit->isSelected && !ctrl};
        if (!keepGroup)
        {
          selectShape(*hit, ctrl);
        }
        this->setRotated(true);
        this->setRotateAnchor(event->pos());
      }
      else if (anySelected)
      {
        this->setRotated(true);
        this->setRotateAnchor(event->pos());
      }
    }
    else if (event->button() == Qt::MiddleButton)
    {
      Shape* const hit{topHit(event->pos())};
      if (hit != nullptr)
      {
        const bool keepGroup{hit->isSelected && !ctrl};
        if (!keepGroup)
        {
          selectShape(*hit, ctrl);
        }
      }
      if (anySelected || hit != nullptr)
      {
        this->setCloned(true);
        this->setDragStart(event->pos());
        this->setClonesCreated(false);
      }
    }
  }
  else if (this->getTool() == ToolType::Triangle)
  {
    if (event->button() == Qt::LeftButton)
    {
      this->trianglePoints.push_back(event->pos());
      if (this->trianglePoints.size() == 3)
      {
        this->shapes.push_back(makeTriangleShape(this->trianglePoints));
        this->trianglePoints.clear();
      }
    }
  }
  else
  {
    if (event->button() == Qt::LeftButton)
    {
      this->setDrawingEnabled(true);
      this->setLastPoint(event->pos());
    }
  }

  this->update();
}

void PaintCanvas::mouseReleaseEvent(QMouseEvent* event)
{
  event->accept();
  if (this->getTool() == ToolType::Modify)
  {
    if (event->button() == Qt::LeftButton)
    {
      if (this->isSelected())
      {
        this->applySelectionRect(
          event->modifiers().testFlag(Qt::ControlModifier));
      }
      else if (!(this->isMoved()) && !(this->isSelected()))
      {
        if (!event->modifiers().testFlag(Qt::ControlModifier))
        {
          this->clearSelections();
        }
        Shape* const hit{topHit(event->pos())};
        if (hit != nullptr)
        {
          selectShape(*hit, event->modifiers().testFlag(Qt::ControlModifier));
        }
      }
      this->setSelected(false);
      this->setMoved(false);
    }
    else if (event->button() == Qt::RightButton)
    {
      this->setRotated(false);
    }
    else if (event->button() == Qt::MiddleButton)
    {
      this->setCloned(false);
      this->setClonesCreated(false);
    }
  }
  else
  {
    if (event->button() == Qt::LeftButton && this->isDrawingEnabled())
    {
      if (this->getTool() == ToolType::Rect)
      {
        this->shapes.push_back(this->makeRectShape(
          this->getLastPoint(),
          event->pos(),
          ToolType::Rect));
      }
      else if (this->getTool() == ToolType::Square)
      {
        this->shapes.push_back(
          this->makeSquareShape(this->getLastPoint(), event->pos()));
      }
      else if (this->getTool() == ToolType::Ellipse)
      {
        this->shapes.push_back(
          this->makeEllipseShape(this->getLastPoint(), event->pos()));
      }
      this->setDrawingEnabled(false);
    }
  }

  this->update();
}

void PaintCanvas::mouseMoveEvent(QMouseEvent* event)
{
  event->accept();
  this->setLastPos(event->pos());

  if (this->getTool() == ToolType::Modify)
  {
    if (this->isMoved() && event->buttons().testFlag(Qt::LeftButton))
    {
      const QPointF delta{event->pos() - this->getDragStart()};
      this->moveSelected(delta);
      this->setDragStart(event->pos());
    }
    else if (this->isSelected() && event->buttons().testFlag(Qt::LeftButton))
    {
      this->setSelectionRect(QRectF{this->getSelectionStart(), event->pos()});
    }
    else if (this->isRotated() && event->buttons().testFlag(Qt::RightButton))
    {
      this->rotateSelected(this->getRotateAnchor(), event->pos());
      this->setRotateAnchor(event->pos());
    }
    else if (this->isCloned() && event->buttons().testFlag(Qt::MiddleButton))
    {
      if (!(this->isClonesCreated()))
      {
        this->cloneSelected();
        this->setClonesCreated(true);
      }
      const QPointF delta{event->pos() - this->getDragStart()};
      this->moveSelected(delta);
      this->setDragStart(event->pos());
    }
  }

  this->update();
}

void PaintCanvas::paintEvent(QPaintEvent* event)
{
  event->accept();
  QPainter p{this};
  p.fillRect(this->rect(), Qt::white);

  // Some latest C++
  std::ranges::for_each(
    this->shapes,
    [this, &p](const auto& s)
    {
      QPainterPath path{this->shapePath(s)};
      QPen pen{s.pen, static_cast<qreal>(s.width)};
      pen.setCapStyle(Qt::RoundCap);
      pen.setJoinStyle(Qt::RoundJoin);
      p.setPen(pen);
      if (this->getFill())
      {
        p.setBrush(s.fill);
      }
      else
      {
        p.setBrush(Qt::NoBrush);
      }
      p.drawPath(path);
      if (s.isSelected)
      {
        QPen dashPen{Qt::DashLine};
        dashPen.setColor(Qt::blue);
        p.setPen(dashPen);
        p.setBrush(Qt::NoBrush);
        p.drawRect(path.boundingRect());
      }
    });

  if (this->getTool() == ToolType::Modify && this->isSelected())
  {
    QPen dashPen{Qt::DashLine};
    dashPen.setColor(Qt::darkGray);
    p.setPen(dashPen);
    p.setBrush(QColor(0, 0, 255, 30));
    p.drawRect(this->getSelectionRect());
  }

  if (
    this->getTool() == ToolType::Triangle && !(this->trianglePoints.isEmpty()))
  {
    QPen previewPen{Qt::DashLine};
    previewPen.setColor(Qt::darkGreen);
    p.setPen(previewPen);
    p.setBrush(Qt::NoBrush);
    if (this->trianglePoints.size() == 1)
    {
      p.drawLine(this->trianglePoints.at(0), this->getLastPos());
    }
    else if (this->trianglePoints.size() == 2)
    {
      p.drawLine(this->trianglePoints.at(1), this->getLastPos());
      p.drawLine(this->trianglePoints.at(0), this->trianglePoints.at(1));
    }
  }

  if (
    this->isDrawingEnabled() &&
    (this->getTool() == ToolType::Rect || this->getTool() == ToolType::Square ||
     this->getTool() == ToolType::Ellipse))
  {
    QPen previewPen{Qt::DashLine};
    previewPen.setColor(Qt::darkGreen);
    p.setPen(previewPen);
    p.setBrush(QColor(0, 255, 0, 30));
    if (this->getTool() == ToolType::Rect)
    {
      p.drawRect(QRectF{this->getLastPoint(), this->getLastPos()});
    }
    else if (this->getTool() == ToolType::Square)
    {
      const Shape sq{
        this->makeSquareShape(this->getLastPoint(), this->getLastPos())};
      p.drawRect(QRectF{sq.points.at(0), sq.points.at(1)});
    }
    else if (this->getTool() == ToolType::Ellipse)
    {
      const Shape el{
        this->makeEllipseShape(this->getLastPoint(), this->getLastPos())};
      p.drawEllipse(QRectF{el.points.at(0), el.points.at(1)});
    }
  }
}

void PaintCanvas::resizeEvent(QResizeEvent* event)
{
  event->accept();

  if (
    this->width() > this->getImage().width() ||
    this->height() > this->getImage().height())
  {
    const int newWidth{qMax(this->width() + 128, this->getImage().width())};
    const int newHeight{qMax(this->height() + 128, this->getImage().height())};
    this->resizeImage(&this->image, QSize{newWidth, newHeight});
    this->update();
  }

  QWidget::resizeEvent(event);
}

void PaintCanvas::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_D)
  {
    event->accept();

    this->deleteSelected();
    this->update();

    return;
  }

  QWidget::keyPressEvent(event);
}

PaintCanvas::Shape PaintCanvas::makeRectShape(
  const QPointF& a, const QPointF& b, const ToolType& t) const
{
  Shape s{};

  s.type = t;
  QRectF r{a, b};
  s.points = {r.topLeft(), r.bottomRight()};
  s.pen = this->getPenColor();
  s.fill = this->getFillColor();
  s.width = this->getPenWidth();

  return s;
}

PaintCanvas::Shape
PaintCanvas::makeSquareShape(const QPointF& center, const QPointF& cursor) const
{
  Shape s{};

  s.type = ToolType::Square;
  const qreal dist{QLineF{center, cursor}.length()};
  const qreal side{dist / 2.0};
  const qreal half{side / 2.0};
  QRectF r{
    QPointF{center.x() - half, center.y() - half},
    QPointF{center.x() + half, center.y() + half}};
  s.points = {r.topLeft(), r.bottomRight()};
  s.pen = this->getPenColor();
  s.fill = this->getFillColor();
  s.width = this->getPenWidth();

  return s;
}

PaintCanvas::Shape PaintCanvas::makeEllipseShape(
  const QPointF& center, const QPointF& cursor) const
{
  Shape s{};

  s.type = ToolType::Ellipse;
  const qreal radius{QLineF{center, cursor}.length()};
  QRectF r{
    QPointF{center.x() - radius, center.y() - radius},
    QPointF{center.x() + radius, center.y() + radius}};
  s.points = {r.topLeft(), r.bottomRight()};
  s.pen = this->getPenColor();
  s.fill = this->getFillColor();
  s.width = this->getPenWidth();

  return s;
}

PaintCanvas::Shape
PaintCanvas::makeTriangleShape(const QVector<QPointF>& pts) const
{
  Shape s;

  s.type = ToolType::Triangle;
  s.points = pts;
  s.pen = this->getPenColor();
  s.fill = this->getFillColor();
  s.width = this->getPenWidth();

  return s;
}

QPainterPath PaintCanvas::shapePath(const Shape& s) const
{
  QPainterPath path{};

  if (s.type == ToolType::Triangle && s.points.size() == 3)
  {
    path.moveTo(s.points.at(0));
    path.lineTo(s.points.at(1));
    path.lineTo(s.points.at(2));
    path.closeSubpath();
  }
  else if (
    (s.type == ToolType::Rect || s.type == ToolType::Square ||
     s.type == ToolType::Ellipse) &&
    s.points.size() >= 2)
  {
    QRectF r{s.points.at(0), s.points.at(1)};
    r = r.normalized();
    if (s.type == ToolType::Ellipse)
    {
      path.addEllipse(r);
    }
    else
    {
      path.addRect(r);
    }
  }

  const QPointF c{this->shapeCenter(s)};
  QTransform tr{};

  tr.translate(c.x(), c.y());
  tr.rotateRadians(s.rotation);
  tr.translate(-c.x(), -c.y());

  return tr.map(path);
}

QRectF PaintCanvas::shapeBounds(const Shape& s) const
{
  return this->shapePath(s).boundingRect();
}

QPointF PaintCanvas::shapeCenter(const Shape& s) const
{
  if (s.type == ToolType::Triangle && s.points.size() == 3)
  {
    return (s.points.at(0) + s.points.at(1) + s.points.at(2)) / 3.0;
  }
  if (s.points.size() >= 2)
  {
    return QRectF{s.points.at(0), s.points.at(1)}.center();
  }

  return QPointF{};
}

bool PaintCanvas::hitTest(const Shape& s, const QPointF& p) const
{
  return this->shapePath(s).contains(p);
}

void PaintCanvas::clearSelections()
{
  std::ranges::for_each(
    this->shapes,
    [](auto& s)
    {
      s.isSelected = false;
    });
}

void PaintCanvas::selectShape(Shape& s, const bool add)
{
  if (!add)
  {
    this->clearSelections();
  }
  s.isSelected = true;
}

PaintCanvas::Shape* PaintCanvas::topHit(const QPointF& p)
{
  // Some latest C++ pipe syntax
  const auto revView{this->shapes | std::views::reverse};
  const auto it{std::ranges::find_if(
    revView,
    [this, p, &revView](const Shape& s)
    {
      return this->hitTest(s, p);
    })};
  return it == revView.cend() ? nullptr : std::addressof(*it);
}

void PaintCanvas::applySelectionRect(const bool add)
{
  const QRectF rect{this->getSelectionRect().normalized()};
  if (!add)
  {
    this->clearSelections();
  }

  std::ranges::for_each(
    this->shapes,
    [this, &rect = std::as_const(rect)](auto& s)
    {
      if (this->shapePath(s).intersects(rect) || rect.contains(shapeBounds(s)))
      {
        s.isSelected = true;
      }
    });
}

void PaintCanvas::moveSelected(const QPointF& delta)
{
  std::ranges::for_each(
    this->shapes | std::views::filter(
                     [](const Shape& s)
                     {
                       return s.isSelected;
                     }),
    [&delta = std::as_const(delta)](Shape& s)
    {
      std::ranges::for_each(
        s.points,
        [&delta = std::as_const(delta)](QPointF& p)
        {
          p += delta;
        });
    });
}

void PaintCanvas::rotateSelected(const QPointF& start, const QPointF& now)
{
  QVector<Shape*> selectedShapes{};
  QPointF center{};

  std::ranges::for_each(
    this->shapes,
    [this, &selectedShapes, &center](auto& s)
    {
      if (s.isSelected)
      {
        selectedShapes.push_back(&s);
        center += this->shapeCenter(s);
      }
    });

  if (selectedShapes.isEmpty())
  {
    return;
  }

  center /= selectedShapes.size();
  const QLineF a{center, start};
  const QLineF b{center, now};
  const qreal delta = b.angleTo(a) * M_PI / 180.0;

  std::ranges::for_each(
    selectedShapes,
    [&delta = std::as_const(delta)](auto* const s)
    {
      s->rotation += +delta;
    });
}

void PaintCanvas::cloneSelected()
{
  this->clones.clear();
  QVector<Shape> newClones{};

  std::ranges::for_each(
    this->shapes | std::views::filter(
                     [](const Shape& s)
                     {
                       return s.isSelected;
                     }),
    [&newClones](auto& s)
    {
      Shape copy{s};
      copy.isSelected = true;
      s.isSelected = false;
      newClones.push_back(copy);
    });

  if (newClones.isEmpty())
  {
    return;
  }

  this->clones = newClones;
  this->shapes.append(newClones);
}

void PaintCanvas::deleteSelected()
{
  erase_if(
    this->shapes,
    [](const Shape& s)
    {
      return s.isSelected;
    });
}

void PaintCanvas::clearAll()
{
  this->shapes.clear();
  this->clones.clear();
  this->trianglePoints.clear();
  this->setSelected(false);
  this->setMoved(false);
  this->setRotated(false);
  this->setCloned(false);
  this->setClonesCreated(false);
  this->setSelectionRect(QRectF{});
  this->update();
}

QJsonObject PaintCanvas::shapeToJson(const PaintCanvas::Shape& s) const
{
  QJsonObject obj{};
  obj["type"] = static_cast<int>(s.type);
  obj["rotation"] = s.rotation;
  obj["width"] = s.width;
  obj["pen"] = s.pen.name(QColor::HexArgb);
  obj["fill"] = s.fill.name(QColor::HexArgb);

  QJsonArray pts{};

  std::ranges::for_each(
    s.points,
    [&pts](const auto& p)
    {
      QJsonArray pt;
      pt.append(p.x());
      pt.append(p.y());
      pts.append(pt);
    });

  obj["points"] = pts;
  return obj;
}

PaintCanvas::Shape PaintCanvas::jsonToShape(const QJsonObject& obj) const
{
  PaintCanvas::Shape s{};
  s.type = static_cast<PaintCanvas::ToolType>(obj["type"].toInt());
  s.rotation = obj["rotation"].toDouble();
  s.width = obj["width"].toInt();
  s.pen = QColor{obj["pen"].toString()};
  s.fill = QColor{obj["fill"].toString()};

  QJsonArray pts{obj["points"].toArray()};

  std::ranges::for_each(
    pts,
    [&s](const auto& v)
    {
      const auto arr{v.toArray()};
      if (arr.size() == 2)
      {
        s.points.push_back(QPointF{arr.at(0).toDouble(), arr.at(1).toDouble()});
      }
    });

  return s;
}

QString PaintCanvas::toSerialized() const
{
  QJsonArray arr{};
  std::ranges::for_each(
    this->shapes,
    [this, &arr](const auto& s)
    {
      arr.append(this->shapeToJson(s));
    });

  QJsonObject root{};
  root["shapes"] = arr;
  root["fill"] = this->getFill();
  root["penColor"] = this->getPenColor().name(QColor::HexArgb);
  root["fillColor"] = this->getFillColor().name(QColor::HexArgb);
  root["penWidth"] = this->getPenWidth();

  return QString::fromUtf8(QJsonDocument{root}.toJson(QJsonDocument::Compact));
}

void PaintCanvas::loadFromSerialized(const QString& json)
{
  this->shapes.clear();
  const QJsonDocument doc{QJsonDocument::fromJson(json.toUtf8())};
  if (doc.isObject())
  {
    const QJsonObject root{doc.object()};
    const QJsonArray arr{root["shapes"].toArray()};

    std::ranges::for_each(
      arr,
      [this](const auto& v)
      {
        if (v.isObject())
        {
          this->shapes.push_back(this->jsonToShape(v.toObject()));
        }
      });

    if (root.contains("fill"))
    {
      this->setFill(root["fill"].toBool(this->getFill()));
    }
    if (root.contains("penColor"))
    {
      this->setPenColor(QColor{root["penColor"].toString()});
    }
    if (root.contains("fillColor"))
    {
      this->setFillColor(QColor{root["fillColor"].toString()});
    }
    if (root.contains("penWidth"))
    {
      this->setPenWidth(root["penWidth"].toInt(this->getPenWidth()));
    }
  }
}

bool PaintCanvas::isMoved() const
{
  return this->moved;
}

void PaintCanvas::setMoved(const bool isMoved)
{
  this->moved = isMoved;
}

QPointF PaintCanvas::getDragStart() const
{
  return this->dragStart;
}

void PaintCanvas::setDragStart(const QPointF& newDragStart)
{
  this->dragStart = newDragStart;
}

bool PaintCanvas::isSelected() const
{
  return this->selected;
}

void PaintCanvas::setSelected(const bool isSelected)
{
  this->selected = isSelected;
}

QPointF PaintCanvas::getSelectionStart() const
{
  return this->selectionStart;
}

void PaintCanvas::setSelectionStart(const QPointF& newSelectionStart)
{
  this->selectionStart = newSelectionStart;
}

QRectF PaintCanvas::getSelectionRect() const
{
  return this->selectionRect;
}

void PaintCanvas::setSelectionRect(const QRectF& newSelectionRect)
{
  this->selectionRect = newSelectionRect;
}

bool PaintCanvas::isRotated() const
{
  return this->rotated;
}

void PaintCanvas::setRotated(const bool isRotated)
{
  this->rotated = isRotated;
}

QPointF PaintCanvas::getRotateAnchor() const
{
  return this->rotateAnchor;
}

void PaintCanvas::setRotateAnchor(const QPointF& newRotateAnchor)
{
  this->rotateAnchor = newRotateAnchor;
}

bool PaintCanvas::isCloned() const
{
  return this->cloned;
}

void PaintCanvas::setCloned(const bool isCloned)
{
  this->cloned = isCloned;
}

bool PaintCanvas::isClonesCreated() const
{
  return this->clonesCreated;
}

void PaintCanvas::setClonesCreated(const bool isClonesCreated)
{
  this->clonesCreated = isClonesCreated;
}

QPointF PaintCanvas::getLastPos() const
{
  return this->lastPos;
}

void PaintCanvas::setLastPos(const QPointF& newLastPos)
{
  this->lastPos = newLastPos;
}

QImage PaintCanvas::toImage() const
{
  QImage img{size(), QImage::Format_ARGB32_Premultiplied};
  img.fill(Qt::white);
  QPainter p{&img};
  p.setRenderHint(QPainter::Antialiasing, true);

  std::ranges::for_each(
    this->shapes,
    [this, &p](const auto& s)
    {
      const QPainterPath path{shapePath(s)};
      QPen pen{s.pen, static_cast<qreal>(s.width)};
      pen.setCapStyle(Qt::RoundCap);
      pen.setJoinStyle(Qt::RoundJoin);
      p.setPen(pen);
      if (this->getFill())
      {
        p.setBrush(s.fill);
      }
      else
      {
        p.setBrush(Qt::NoBrush);
      }
      p.drawPath(path);
    });

  return img;
}
