#include "Panels.h"

Panels::Panels()
    : _xmargins({0.15, 0.9}), _ymargins({0.15, 0.9}), _columns(1), _rows(1) {
  type = Type::I_Panels;
  _items.reserve(2);
}
Panels::Panels(Panels const &other) {
  type = other.type;

  _xmargins = other._xmargins;
  _ymargins = other._ymargins;
  _columns = other._columns;
  _rows = other._rows;

  for (auto &item : other._items) {
    _items.emplace_back(item->clone());
  }
}
Panels *Panels::clone() const { return new Panels(*this); }

void Panels::add(CanvasItem &item) { _items.emplace_back(item.clone()); }

void Panels::add(std::vector<Plot> &plots) {

  for (auto &plot : plots) {

    _items.emplace_back(plot.clone());
  }
}