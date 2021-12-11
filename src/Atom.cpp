#include "Atom.h"

Atom::Atom(double x, double y, double z, int type) : _x(x), _y(y), _z(z) {

  setType(type);
}

void Atom::setType(int type) {

  _type = type;

  _mass = AtomicMasses[type];

  _size = AtomicSizes[type];

  _cpkcolor = AtomicCPKColors[type];
}