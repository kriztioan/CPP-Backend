#ifndef _BASICPROPERTIES_H_
#define _BASICPROPERTIES_H_

#include <string>
#include <string_view>

class LineProperties {

public:
  enum class Style : char {
    Continuous = 1,
    Dashed,
    LongDashed,
    LongDashedShortGaps,
    DashedLongGasp
  };

  LineProperties();

  void setColor(std::string_view color);

  std::string_view getColor() const;

  void setLineWidth(int width);

  const int &getLineWidth() const;

  void setLineStyle(Style style);

  const Style &getLineStyle() const;

private:
  std::string _color;

  int _linewidth;

  Style _linestyle;
};

inline void LineProperties::setColor(std::string_view color) { _color = color; }

inline std::string_view LineProperties::getColor() const { return (_color); }

inline void LineProperties::setLineStyle(LineProperties::Style style) {
  _linestyle = style;
}

inline const LineProperties::Style &LineProperties::getLineStyle() const {
  return (_linestyle);
}

inline void LineProperties::setLineWidth(int width) { _linewidth = width; }

inline const int &LineProperties::getLineWidth() const { return (_linewidth); }

#endif /* _BASICPROPERTIES_H_ */
