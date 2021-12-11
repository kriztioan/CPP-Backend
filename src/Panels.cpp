#include "Panels.h"

Panels::Panels()
    : _xmargins({0.15, 0.9}), _ymargins({0.15, 0.9}), _columns(0), _rows(0) {
  _wrappers.reserve(2);
}

void Panels::addPlot(Plot &plot) {

  _wrappers.emplace_back(Wrapper::Type::W_Plot,
                         static_cast<Wrapper::Data>(&plot));
}

void Panels::addPlots(std::vector<Plot> &plots) {

  for (auto &p : plots) {

    _wrappers.emplace_back(Wrapper::Type::W_Plot,
                           static_cast<Wrapper::Data>(&p));
  }
}