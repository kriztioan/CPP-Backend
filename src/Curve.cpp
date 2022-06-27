#include "Curve.h"

Curve::Curve()
    : _fillcolor("ff000099"), _symbolsize(1), _fill(false), _symbol(0) {
  _x.reserve(8);
  _y.reserve(8);
}

void Curve::setXAndY(const double x, const double y) {

  _x.clear();

  _x.push_back(x);

  _y.clear();

  _y.push_back(y);
}

void Curve::setXErr(const double xerr) {

  _xerr.clear();

  _xerr.push_back(xerr);
}

void Curve::setYErr(const double yerr) {

  _yerr.clear();

  _yerr.push_back(yerr);
}
