#ifndef _PLOT_H_
#define _PLOT_H_

#include "Wrapper.h"

#include "Line.h"

#include "LineProperties.h"

#include "Curve.h"

#include "Axis.h"

#include <array>
#include <string>
#include <string_view>
#include <vector>

class Plot : public LineProperties {

public:
  typedef std::vector<Wrapper>::iterator iterator;

  Plot();

  Wrapper &operator[](std::size_t idx);

  iterator begin() noexcept { return (_wrappers.begin()); }

  iterator end() noexcept { return (_wrappers.end()); }

  void addPoint(Point &point);

  void addPoints(std::vector<Point> &points);

  void addLine(Line &line);

  void addLines(std::vector<Line> &lines);

  void addCurve(Curve &curve);

  void addCurves(std::vector<Curve> &curves);

  void addText(Text &text);

  void clear();

  void erase(std::vector<Wrapper>::iterator begin,
             std::vector<Wrapper>::iterator end);

  void setXLimits(const std::array<double, 2> &limits);

  std::array<double, 2> const &getXLimits() const;

  void setYLimits(const std::array<double, 2> &limits);

  std::array<double, 2> const &getYLimits() const;

  void setZLimits(const std::array<double, 2> &limits);

  std::array<double, 2> const &getZLimits() const;

  void setXMargins(const std::array<double, 2> &margins);

  std::array<double, 2> const &getXMargins() const;

  void setYMargins(const std::array<double, 2> &margins);

  std::array<double, 2> const &getYMargins() const;

  void setZMargins(const std::array<double, 2> &margins);

  std::array<double, 2> const &getZMargins() const;

  void setTitle(std::string_view title);

  std::string_view getTitle() const;

  void setDrawHorizontalGrid(bool on);

  LineProperties &getHorizontalGrid();

  bool isDrawHorizontalGrid() const;

  void setDrawVerticalGrid(bool on);

  LineProperties &getVerticalGrid();

  bool isDrawVerticalGrid() const;

  void setDrawHorizontalFineGrid(bool on);

  bool isDrawHorizontalFineGrid() const;

  void setDrawVerticalFineGrid(bool on);

  bool isDrawVerticalFineGrid() const;

  void setAdvance(bool on);

  bool isAdvance() const;

  Axis &getXAxis();

  Axis &getYAxis();

  Axis &getZAxis();

  void setFontSize(double size);

  const double &getFontSize() const;

  void setMajorTickLength(double length);

  const double &getMajorTickLength() const;

  void setMinorTickLength(double length);

  const double &getMinorTickLength() const;

private:
  std::vector<Wrapper> _wrappers;

  std::array<double, 2> _xlimits;

  std::array<double, 2> _ylimits;

  std::array<double, 2> _zlimits;

  std::array<double, 2> _xmargins;

  std::array<double, 2> _ymargins;

  std::array<double, 2> _zmargins;

  std::string _title;

  LineProperties _horizontalgrid;

  LineProperties _verticalgrid;

  Axis _xaxis;

  Axis _yaxis;

  Axis _zaxis;

  double _fontsize;

  double _majorticklength;

  double _minorticklength;

  bool _drawhorizontalgrid;

  bool _drawverticalgrid;

  bool _drawhorizontalfinegrid;

  bool _drawverticalfinegrid;

  bool _advance;
};

inline Wrapper &Plot::operator[](std::size_t idx) { return (_wrappers[idx]); }

inline void Plot::clear() { _wrappers.clear(); }

inline void Plot::erase(std::vector<Wrapper>::iterator begin,
                        std::vector<Wrapper>::iterator end) {
  _wrappers.erase(begin, end);
}

inline void Plot::setXLimits(const std::array<double, 2> &limits) {
  _xlimits = limits;
}

inline void Plot::setYLimits(const std::array<double, 2> &limits) {
  _ylimits = limits;
}

inline void Plot::setZLimits(const std::array<double, 2> &limits) {
  _zlimits = limits;
}

inline void Plot::setXMargins(const std::array<double, 2> &margins) {
  _xmargins = margins;
}

inline void Plot::setYMargins(const std::array<double, 2> &margins) {
  _ymargins = margins;
}

inline void Plot::setZMargins(const std::array<double, 2> &margins) {
  _zmargins = margins;
}

inline void Plot::setTitle(std::string_view title) { _title = title; }

inline std::string_view Plot::getTitle() const { return (_title); }

inline std::array<double, 2> const &Plot::getXLimits() const {
  return (_xlimits);
}

inline std::array<double, 2> const &Plot::getYLimits() const {
  return (_ylimits);
}

inline std::array<double, 2> const &Plot::getZLimits() const {
  return (_zlimits);
}

inline std::array<double, 2> const &Plot::getXMargins() const {
  return (_xmargins);
}

inline std::array<double, 2> const &Plot::getYMargins() const {
  return (_ymargins);
}

inline std::array<double, 2> const &Plot::getZMargins() const {
  return (_zmargins);
}

inline void Plot::setDrawHorizontalGrid(bool on = true) {
  _drawhorizontalgrid = on;
}

inline LineProperties &Plot::getHorizontalGrid() { return (_horizontalgrid); }

inline bool Plot::isDrawHorizontalGrid() const { return (_drawhorizontalgrid); }

inline void Plot::setDrawVerticalGrid(bool on = true) {
  _drawverticalgrid = on;
}

inline LineProperties &Plot::getVerticalGrid() { return (_verticalgrid); }

inline bool Plot::isDrawVerticalGrid() const { return (_drawverticalgrid); }

inline void Plot::setDrawHorizontalFineGrid(bool on = true) {
  _drawhorizontalfinegrid = on;
}

inline bool Plot::isDrawHorizontalFineGrid() const {
  return (_drawhorizontalfinegrid);
}

inline void Plot::setDrawVerticalFineGrid(bool on = true) {
  _drawverticalfinegrid = on;
}

inline bool Plot::isDrawVerticalFineGrid() const {
  return (_drawverticalfinegrid);
}

inline bool Plot::isAdvance() const { return (_advance); }

inline void Plot::setAdvance(bool on = true) { _advance = on; }

inline void Plot::setFontSize(double fontsize) { _fontsize = fontsize; }

inline const double &Plot::getFontSize() const { return (_fontsize); }

inline void Plot::setMajorTickLength(double length) {
  _majorticklength = length;
}

inline const double &Plot::getMajorTickLength() const {
  return (_majorticklength);
}

inline void Plot::setMinorTickLength(double length) {
  _minorticklength = length;
}

inline const double &Plot::getMinorTickLength() const {
  return (_minorticklength);
}

inline Axis &Plot::getXAxis() { return (_xaxis); }

inline Axis &Plot::getYAxis() { return (_yaxis); }

inline Axis &Plot::getZAxis() { return (_zaxis); }

#endif /* _PLOT_H_ */
