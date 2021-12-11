#include "Wrapper.h"

Wrapper::Wrapper() : _type(Type::W_None), _data(nullptr) {}

Wrapper::Wrapper(Type type, Data data) { _wrap(type, data); }

Wrapper::~Wrapper() {

  switch (_type) {

  case Type::W_Point:
    delete static_cast<const Point *>(_data);
    break;

  case Type::W_Line:
    delete static_cast<const Line *>(_data);
    break;

  case Type::W_Curve:
    delete static_cast<const Curve *>(_data);
    break;

  case Type::W_Plot:
    delete static_cast<const Plot *>(_data);
    break;

  case Type::W_Text:
    delete static_cast<const Text *>(_data);
    break;

  case Type::W_Panels:
    delete static_cast<const Panels *>(_data);
    break;

  case Type::W_None:
  default:
    break;
  }
}

Wrapper::Wrapper(const Wrapper &other) { _wrap(other._type, other._data); }

void Wrapper::_wrap(Type type, Data data) {

  _type = type;

  switch (_type) {

  case Type::W_Point:
    _data = static_cast<Data>(new Point(*static_cast<const Point *>(data)));
    break;

  case Type::W_Line:
    _data = static_cast<Data>(new Line(*static_cast<const Line *>(data)));
    break;

  case Type::W_Curve:
    _data = static_cast<Data>(new Curve(*static_cast<const Curve *>(data)));
    break;

  case Type::W_Plot:
    _data = static_cast<Data>(new Plot(*static_cast<const Plot *>(data)));
    break;

  case Type::W_Text:
    _data = static_cast<Data>(new Text(*static_cast<const Text *>(data)));
    break;

  case Type::W_Panels:
    _data = static_cast<Data>(new Panels(*static_cast<const Panels *>(data)));
    break;

  case Type::W_None:
  default:
    _data = nullptr;
    break;
  }
}