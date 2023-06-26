#include "Plot.h"

Plot::Plot()
    : _xlimits({-1.0, 1.0}), _ylimits({-1.0, 1.0}), _zlimits({-1.0, 1.0}),
      _xmargins({0.15, 0.9}), _ymargins({0.15, 0.85}), _zmargins({0.15, 0.9}),
      _title(""), _fontsize(1.0), _majorticklength(1.0),
      _minorticklength(2.0 / 3.0), _drawhorizontalgrid(false),
      _drawverticalgrid(false), _drawhorizontalfinegrid(false),
      _drawverticalfinegrid(false), _advance(true) {

  type = Type::I_Plot;
  _items.reserve(4);
  _xaxis.emplace_back();
  _yaxis.emplace_back();
  _zaxis.emplace_back();
}
Plot::Plot(Plot const &other) {
  type = other.type;

  _xlimits = other._xlimits;
  _ylimits = other._ylimits;
  _zlimits = other._zlimits;
  _xmargins = other._xmargins;
  _ymargins = other._ymargins;
  _zmargins = other._zmargins;
  _title = other._title;
  _horizontalgrid = other._horizontalgrid;
  _verticalgrid = other._verticalgrid;
  _xaxis = other._xaxis;
  _yaxis = other._yaxis;
  _zaxis = other._zaxis;
  _fontsize = other._fontsize;
  _majorticklength = other._majorticklength;
  _minorticklength = other._minorticklength;
  _drawhorizontalgrid = other._drawhorizontalgrid;
  _drawverticalgrid = other._drawverticalgrid;
  _drawhorizontalfinegrid = other._drawhorizontalfinegrid;
  _drawverticalfinegrid = other._drawverticalfinegrid;
  _advance = other._advance;

  for (auto &item : other._items) {

    _items.emplace_back(item->clone());
  }
}
Plot *Plot::clone() const { return new Plot(*this); }
void Plot::add(CanvasItem &item) { _items.emplace_back(item.clone()); }

void Plot::add(std::vector<CanvasItem> &items) {

  for (auto &item : items) {

    _items.emplace_back(item.clone());
  }
}