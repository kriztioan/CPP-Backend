#ifndef _PANELS_H_
#define _PANELS_H_

#include "Axis.h"
#include "CanvasItem.h"
#include "Plot.h"

#include <array>

class Panels : public CanvasItem {

public:
  typedef std::vector<std::unique_ptr<CanvasItem>>::iterator iterator;

  Panels();
  Panels(Panels const &other);
  Panels *clone() const;
  CanvasItem &operator[](std::size_t idx);

  iterator begin() noexcept { return _items.begin(); }

  iterator end() noexcept { return _items.end(); }

  void add(CanvasItem &item);

  void add(std::vector<Plot> &plot);

  void erase();

  void erase(std::vector<std::unique_ptr<CanvasItem>>::iterator begin,
             std::vector<std::unique_ptr<CanvasItem>>::iterator end);

  void setXMargins(const std::array<double, 2> &margins);

  std::array<double, 2> const &getXMargins() const;

  void setYMargins(const std::array<double, 2> &margins);

  std::array<double, 2> const &getYMargins() const;

  void setLayout(int columns, int rows);

  void setColumns(int columns);

  const int &getColumns() const;

  void setRows(int rows);

  const int &getRows() const;

private:
  std::vector<std::unique_ptr<CanvasItem>> _items;

  std::array<double, 2> _xmargins;

  std::array<double, 2> _ymargins;

  int _columns;

  int _rows;
};

inline CanvasItem &Panels::operator[](std::size_t idx) {
  return (*_items[idx]);
}

inline void Panels::erase() { _items.erase(_items.begin(), _items.end()); }

inline void
Panels::erase(std::vector<std::unique_ptr<CanvasItem>>::iterator begin,
              std::vector<std::unique_ptr<CanvasItem>>::iterator end) {
  _items.erase(begin, end);
}

inline void Panels::setXMargins(const std::array<double, 2> &margins) {
  _xmargins = margins;
}

inline void Panels::setYMargins(const std::array<double, 2> &margins) {
  _ymargins = margins;
}

inline std::array<double, 2> const &Panels::getXMargins() const {
  return (_xmargins);
}

inline std::array<double, 2> const &Panels::getYMargins() const {
  return (_ymargins);
}

inline void Panels::setLayout(int columns, int rows) {
  _columns = columns;
  _rows = rows;
}

inline void Panels::setColumns(int columns) { _columns = columns; }

inline const int &Panels::getColumns() const { return (_columns); }

inline void Panels::setRows(int rows) { _rows = rows; }

inline const int &Panels::getRows() const { return (_rows); }

#endif /* _PANELS_H_ */
