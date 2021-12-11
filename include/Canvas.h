#ifndef _CANVAS_H_
#define _CANVAS_H_

#define PL_DOUBLE

#include <plplot/plstream.h>

#include "Exception.h"

#include "Wrapper.h"

#include <cmath>

#include <algorithm>
#include <array>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

class Canvas {

public:
  typedef std::vector<Wrapper>::iterator iterator;

  Canvas();

  Wrapper &operator[](std::size_t idx);

  iterator begin() noexcept { return (_wrappers.begin()); }

  iterator end() noexcept { return (_wrappers.end()); }

  void addPlot(Plot &plot);

  void addPlots(std::vector<Plot> &plots);

  void addText(Text &text);

  void addPanels(Panels &panels);

  void erase();

  void erase(std::vector<Wrapper>::iterator begin,
             std::vector<Wrapper>::iterator end);

  void setSize(const std::array<int, 2> size);

  void setColor(const std::string_view color);

  void setDefaultCharacterHeight(double height);

  std::array<int, 2> const &getSize() const;

  std::string_view getColor() const;

  const double &getDefaultCharacterHeight() const;

  void paintOnScreen();

  void paintOnPostscript(std::string_view filename);

  void paintOnPNG(std::string_view filename);

  void paintOnJPEG(std::string_view filename);

private:
  std::unique_ptr<plstream> _plstream;

  std::string _color;

  std::vector<std::string> _colormap;

  std::vector<Wrapper> _wrappers;

  std::array<int, 2> _size;

  double _defaultcharacterheight;

  double _defaultticklength;

  int lookUpColor(std::string_view hex);

  void paint();

  void drawPlot(Plot *plot);
};

inline Wrapper &Canvas::operator[](std::size_t idx) { return (_wrappers[idx]); }

inline void Canvas::erase() {
  _wrappers.erase(_wrappers.begin(), _wrappers.end());
}

inline void Canvas::erase(std::vector<Wrapper>::iterator begin,
                          std::vector<Wrapper>::iterator end) {
  _wrappers.erase(begin, end);
}

inline void Canvas::setSize(const std::array<int, 2> size) { _size = size; }

inline void Canvas::setColor(const std::string_view color) { _color = color; }

inline void Canvas::setDefaultCharacterHeight(double height) {
  _defaultcharacterheight = height;
}

inline std::array<int, 2> const &Canvas::getSize() const { return (_size); }

inline std::string_view Canvas::getColor() const { return (_color); }

inline const double &Canvas::getDefaultCharacterHeight() const {
  return (_defaultcharacterheight);
}

#endif /* _CANVAS_H_ */
