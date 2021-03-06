#include "tdp_qt_widgets/BusyAnimationGenerator.h"

#include <QColor>
#include <QPainter>

#include <cmath>

namespace tdp_qt_widgets
{
//##################################################################################################
struct BusyAnimationGenerator::Private
{
  QColor color{Qt::blue};
  float maxAngle{0.5f};
  float lineWidth{0.2f};
  int repeat{2};
  bool clockwise{true};
};

//##################################################################################################
BusyAnimationGenerator::BusyAnimationGenerator():
  d(new Private())
{

}

//##################################################################################################
BusyAnimationGenerator::~BusyAnimationGenerator()
{
  delete d;
}

//##################################################################################################
QImage BusyAnimationGenerator::generateImage(int size)const
{
  QImage resultImage(size, size, QImage::Format_ARGB32);
  QImage mask = QImage(600, 600, QImage::Format_ARGB32);
  QImage color = QImage(600, 600, QImage::Format_ARGB32);

  resultImage.fill(0);
  mask.fill(0);
  color.fill(0);

  float center = float(size)/2.0f;

  {
    float width = d->lineWidth*100.0f;
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(int(width));
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    QPolygonF shape;
    int radius = int(center) - ((int(width)/2)+1);

    int iMax = int(d->maxAngle*360.0f) * 4;
    for(int i=0; i<iMax; i++)
    {
      float degrees = float(i) / 4.0f;
      float radians = degrees * (3.14159265359f/180.0f);

      float x = center + (float(sin(double(radians))) * float(radius));
      float y = center + (float(cos(double(radians))) * float(radius));

      shape.append(QPointF(qreal(x), qreal(y)));
    }

    painter.drawPolyline(shape);
  }

  {
    QPainter painter(&color);
    painter.setRenderHint(QPainter::Antialiasing, false);

    int r = d->color.red();
    int g = d->color.green();
    int b = d->color.blue();

    float div = 16;

    int iMax = int(d->maxAngle*360.0f)*div;
    float factor = 1.0f / float(iMax*iMax*iMax);
    for(int i=0; i<iMax; i++)
    {
      float degrees = float(i) / div;
      float radians = degrees * (3.14159265359f/180.0f);

      float x = center + (std::sin(radians) * 1200);
      float y = center + (std::cos(radians) * 1200);

      auto a = float(float(i*i*i)*factor);

      painter.setPen(QColor(r, g, b, a*255.0f));
      painter.drawLine(center, center, x, y);
    }

    int hMax = iMax + (359*div - iMax)/2;
    for(int i=iMax; i<hMax; i++)
    {
      float degrees = float(i) / div;
      float radians = degrees * (3.14159265359f/180.0f);

      float x = center + (std::sin(radians) * 1200);
      float y = center + (std::cos(radians) * 1200);

      painter.setPen(d->color);
      painter.drawLine(center, center, x, y);
    }

  }


  {
    for(int y=0; y<size; y++)
    {
      for(int x=0; x<size; x++)
      {
        QRgb c = color.pixel(x, y);
        QRgb a = mask.pixel(x, y);

        float alpha = (float(qAlpha(a)) / 255) * float(qAlpha(c));

        resultImage.setPixel(x, y, qRgba(qRed(c), qGreen(c), qBlue(c), alpha));
      }
    }
  }

  //Draw the repeat
  if(d->repeat>1)
  {
    QImage original = resultImage;

    QPainter painter(&resultImage);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    float repeatAngle = (360.0f/float(d->repeat));
    for(int i=1; i<d->repeat; i++)
    {
      painter.save();
      painter.translate(center, center);
      painter.rotate(repeatAngle * float(i));
      painter.drawImage(-center, -center, original);
      painter.restore();
    }
  }

  //Mirror the image if it is clockwise
  if(d->clockwise)
    resultImage = resultImage.mirrored(true, false);

  return resultImage;
}

//##################################################################################################
void BusyAnimationGenerator::setColor(const QColor& color)
{
  d->color = color;
}

//##################################################################################################
void BusyAnimationGenerator::setMaxAngle(float maxAngle)
{
  d->maxAngle = maxAngle;
}

//##################################################################################################
void BusyAnimationGenerator::setLineWidth(float lineWidth)
{
  d->lineWidth = lineWidth;
}

//##################################################################################################
void BusyAnimationGenerator::setClockwise(bool clockwise)
{
  d->clockwise = clockwise;
}

//##################################################################################################
void BusyAnimationGenerator::setRepeat(int repeat)
{
  d->repeat = repeat;
}

}
