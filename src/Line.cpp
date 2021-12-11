#include "Line.h"

Line::Line()
    : _startcoordinates({3.0, 0.0, 0.0}), _endcoordinates({3.0, 0.0, 0.0}) {}

void Line::setStartCoordinates(double x, double y, double z) {

  _startcoordinates = {x, y, z};
}

void Line::setEndCoordinates(double x, double y, double z) {

  _endcoordinates = {x, y, z};
}