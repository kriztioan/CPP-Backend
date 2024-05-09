#ifndef _CANVASITEM_H_
#define _CANVASITEM_H_

class CanvasItem {
public:
  enum class Type : char {
    I_None,
    I_Point,
    I_Line,
    I_Curve,
    I_Plot,
    I_Text,
    I_Panels
  };
  virtual ~CanvasItem() = default;
  virtual CanvasItem *clone() const { return new CanvasItem(*this); };
  Type type;
};

#endif /* _CANVASITEM_H_ */
