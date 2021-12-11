#ifndef _PANELS_H_
#define _PANELS_H_

#include "Wrapper.h"

#include "Axis.h"

#include <array>

class Panels {

public:
  typedef std::vector<Wrapper>::iterator iterator;

  Panels();

  Wrapper &operator[](std::size_t idx);

  iterator begin() noexcept { return _wrappers.begin(); }

  iterator end() noexcept { return _wrappers.end(); }

  void addPlot(Plot &plot);

  void addPlots(std::vector<Plot> &plots);

  void erase();

  void erase(std::vector<Wrapper>::iterator begin,
             std::vector<Wrapper>::iterator end);

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
  std::vector<Wrapper> _wrappers;

  std::array<double, 2> _xmargins;

  std::array<double, 2> _ymargins;

  int _columns;

  int _rows;
};

inline Wrapper &Panels::operator[](std::size_t idx) { return (_wrappers[idx]); }

inline void Panels::erase() {
  _wrappers.erase(_wrappers.begin(), _wrappers.end());
}

inline void Panels::erase(std::vector<Wrapper>::iterator begin,
                          std::vector<Wrapper>::iterator end) {
  _wrappers.erase(begin, end);
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
