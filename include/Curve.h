#ifndef _CURVE_H_
#define _CURVE_H_

#include "LineProperties.h"

#include <string>
#include <string_view>
#include <vector>

class Curve : public LineProperties {

public:
  typedef std::vector<double>::iterator iterator;

  typedef const std::vector<double>::const_iterator const_iterator;

  Curve();

  void setXAndY(const std::vector<double> &x, const std::vector<double> &y);

  void setXAndY(const double &x, const double &y);

  std::vector<double> const &getX() const;

  std::vector<double> const &getY() const;

  void setFillColor(std::string_view color);

  std::string_view getFillColor() const;

  void setFill(bool on);

  bool isFill();

  void setSymbol(char symbol);

  const char &getSymbol() const;

  void setSymbolSize(int size);

  const int &getSymbolSize() const;

private:
  std::vector<double> _x;

  std::vector<double> _y;

  std::string_view _fillcolor;

  int _symbolsize;

  bool _fill;

  char _symbol;
};

inline void Curve::setXAndY(const std::vector<double> &x,
                            const std::vector<double> &y) {
  _x = x;
  _y = y;
}

inline std::vector<double> const &Curve::getX() const { return (_x); }

inline std::vector<double> const &Curve::getY() const { return (_y); }

inline void Curve::setFill(bool on = true) { _fill = on; }

inline bool Curve::isFill() { return (_fill); }

inline void Curve::setFillColor(std::string_view color) { _fillcolor = color; }

inline std::string_view Curve::getFillColor() const { return (_fillcolor); }

inline void Curve::setSymbol(char symbol) { _symbol = symbol; }

inline const char &Curve::getSymbol() const { return (_symbol); }

inline void Curve::setSymbolSize(int size) { _symbolsize = size; }

inline const int &Curve::getSymbolSize() const { return (_symbolsize); }

#endif /* _CURVE_H_ */
