#ifndef _WRAPPER_H_
#define _WRAPPER_H_

class Point;

class Line;

class Curve;

class Panels;

class Plot;

class Text;

class Wrapper {

public:
  enum class Type : char {
    W_None,
    W_Point,
    W_Line,
    W_Curve,
    W_Plot,
    W_Text,
    W_Panels
  };

  typedef void *Data;

  Wrapper();

  Wrapper(Type type, Data data);

  ~Wrapper();

  Wrapper(const Wrapper &other);

  const Type &getType() const;

  Data getData();

private:
  void _wrap(Type type, Data data);

  Type _type;

  Data _data;
};

#include "Point.h"

#include "Line.h"

#include "Curve.h"

#include "Plot.h"

#include "Text.h"

#include "Panels.h"

inline const Wrapper::Type &Wrapper::getType() const { return (_type); }

inline Wrapper::Data Wrapper::getData() { return (_data); }

#endif /* _WRAPPER_H_ */