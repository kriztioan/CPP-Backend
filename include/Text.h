#ifndef _TEXT_H_
#define _TEXT_H_

#include <string>
#include <string_view>

#include <array>

#include <sstream>

#include "CanvasItem.h"

class Text : public CanvasItem {

public:
  enum class CoordinateSystem : char { DATA, NORMAL };

  static constexpr double LeftJustification = 0.0;

  static constexpr double CenterJustification = 0.5;

  static constexpr double RightJustification = 1.0;

  static std::string formatChemicalFormula(std::string_view formula);

  Text();
  Text(std::string_view text);
  virtual Text *clone() const;

  void setCoordinates(const std::array<double, 3> &coordinates,
                      CoordinateSystem system = CoordinateSystem::DATA);

  void setCoordinates(double x, double y, double z,
                      CoordinateSystem system = CoordinateSystem::DATA);

  std::array<double, 3> const &getCoordinates() const;

  const CoordinateSystem &getSystem() const;

  void setText(std::string_view text);

  std::string_view getText() const;

  void setColor(std::string_view color);

  std::string_view getColor() const;

  void setSize(double size);

  const double &getSize() const;

  void setAngle(double angle);

  const double &getAngle() const;

  void setJustification(double justification);

  const double &getJustification() const;

private:
  std::array<double, 3> _coordinates;

  std::string _text;

  std::string _color;

  double _size;

  double _angle;

  double _justification;

  CoordinateSystem _system;
};

inline void Text::setCoordinates(const std::array<double, 3> &coordinates,
                                 CoordinateSystem system) {
  _coordinates = coordinates;
  _system = system;
}

inline const std::array<double, 3> &Text::getCoordinates() const {
  return (_coordinates);
}

inline const Text::CoordinateSystem &Text::getSystem() const {
  return (_system);
}

inline void Text::setColor(std::string_view color) { _color = color; }

inline std::string_view Text::getColor() const { return (_color); }

inline void Text::setText(std::string_view text) { _text = text; }

inline std::string_view Text::getText() const { return (_text); }

inline void Text::setAngle(double angle) { _angle = angle; }

inline const double &Text::getAngle() const { return (_angle); }

inline void Text::setSize(double size) { _size = size; }

inline const double &Text::getSize() const { return (_size); }

inline void Text::setJustification(double justification) {
  _justification = justification;
}

inline const double &Text::getJustification() const { return (_justification); }

#endif /* _TEXT_H_ */
