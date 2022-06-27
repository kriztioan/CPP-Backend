#include "Plot.h"

Plot::Plot()
    : _xlimits({2.0, 0.0}), _ylimits({2.0, 0.0}), _zlimits({2.0, 0.0}),
      _xmargins({0.15, 0.9}), _ymargins({0.15, 0.85}), _zmargins({0.15, 0.9}),
      _title(""), _fontsize(1.0), _majorticklength(1.0),
      _minorticklength(2.0 / 3.0), _drawhorizontalgrid(false),
      _drawverticalgrid(false), _drawhorizontalfinegrid(false),
      _drawverticalfinegrid(false), _advance(true) {

  _wrappers.reserve(4);
  _xaxis.emplace_back();
  _yaxis.emplace_back();
  _zaxis.emplace_back();
}

void Plot::addPoint(Point &point) {

  _wrappers.emplace_back(Wrapper::Type::W_Point,
                         static_cast<Wrapper::Data>(&point));
}

void Plot::addPoints(std::vector<Point> &points) {

  for (auto &point : points) {

    _wrappers.emplace_back(Wrapper::Type::W_Point,
                           static_cast<Wrapper::Data>(&point));
  }
}

void Plot::addLine(Line &line) {

  _wrappers.emplace_back(Wrapper::Type::W_Line,
                         static_cast<Wrapper::Data>(&line));
}

void Plot::addLines(std::vector<Line> &lines) {

  for (auto &line : lines) {

    _wrappers.emplace_back(Wrapper::Type::W_Line,
                           static_cast<Wrapper::Data>(&line));
  }
}

void Plot::addCurve(Curve &curve) {

  _wrappers.emplace_back(Wrapper::Type::W_Curve,
                         static_cast<Wrapper::Data>(&curve));
}

void Plot::addCurves(std::vector<Curve> &curves) {

  for (auto &curve : curves) {

    _wrappers.emplace_back(Wrapper::Type::W_Curve,
                           static_cast<Wrapper::Data>(&curve));
  }
}

void Plot::addText(Text &text) {

  _wrappers.emplace_back(Wrapper::Type::W_Text,
                         static_cast<Wrapper::Data>(&text));
}