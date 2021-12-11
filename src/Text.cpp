#include "Text.h"

Text::Text()
    : _coordinates({2.0, 0.0, 0.0}), _text(""), _color("000000"), _size(1.0),
      _angle(0.0), _justification(LeftJustification),
      _system(CoordinateSystem::DATA) {}

Text::Text(std::string_view text)
    : _coordinates({2.0, 0.0}), _text(text), _color("000000"), _size(1.0),
      _angle(0.0), _justification(LeftJustification) {}

void Text::setCoordinates(double x, double y, double z,
                          CoordinateSystem system) {

  _coordinates = {x, y, z};

  _system = system;
}

std::string Text::formatChemicalFormula(std::string_view formula) {

  std::ostringstream ostr;

  bool subscript = false;

  bool superscript = false;

  bool charge = false;

  for (const auto &c : formula) {

    if (isdigit(c) && !charge) {

      if (superscript) {

        ostr << "#d";

        superscript = false;
      }

      if (!subscript) {

        ostr << "#d";

        subscript = true;
      }
    } else if (isalpha(c)) {

      if (superscript) {

        ostr << "#d";

        superscript = false;
      }

      if (subscript) {

        ostr << "#u";

        subscript = false;
      }
    } else if (!superscript) {

      if (subscript) {

        ostr << "#u";

        subscript = false;
      }

      ostr << "#u";

      superscript = charge = true;
    }

    ostr << c;
  }

  if (subscript) {

    ostr << "#u";
  }

  if (superscript) {

    ostr << "#d";
  }

  return (ostr.str());
}