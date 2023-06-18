#ifndef _POINT_H_
#define _POINT_H_

#include "CanvasItem.h"

#include <string>
#include <string_view>

#include <array>

class Point : public CanvasItem {

public:
  Point();
  Point *clone() const;

  void setCoordinates(const std::array<double, 3> &coordinates);

  void setCoordinates(double x, double y, double z);

  std::array<double, 3> const &getCoordinates() const;

  void setColor(std::string_view color);

  std::string_view getColor() const;

  void setSymbol(int symbol);

  const int &getSymbol() const;

  void setSize(int size);

  const int &getSize() const;

private:
  std::array<double, 3> _coordinates;

  std::string _color;

  int _size;

  int _symbol;
};

inline void Point::setCoordinates(const std::array<double, 3> &coordinates) {
  _coordinates = coordinates;
}

inline std::array<double, 3> const &Point::getCoordinates() const {
  return (_coordinates);
}

inline void Point::setColor(std::string_view color) { _color = color; }

inline std::string_view Point::getColor() const { return (_color); }

inline void Point::setSymbol(int symbol) { _symbol = symbol; }

inline const int &Point::getSymbol() const { return (_symbol); }

inline void Point::setSize(int size) { _size = size; }

inline const int &Point::getSize() const { return (_size); }

#endif /* _POINT_H_ */