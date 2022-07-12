#include "Canvas.h"

Canvas::Canvas()
    : _color("000000"), _size({2, 300}), _defaultcharacterheight(2.25),
      _defaultticklength(3.0) {
  _wrappers.reserve(4);
}

void Canvas::addPlot(Plot &plot) {

  _wrappers.emplace_back(Wrapper::Type::W_Plot,
                         static_cast<Wrapper::Data>(&plot));
}

void Canvas::addPlots(std::vector<Plot> &plots) {

  for (auto &p : plots) {

    _wrappers.emplace_back(Wrapper::Type::W_Plot,
                           static_cast<Wrapper::Data>(&p));
  }
}

void Canvas::addText(Text &text) {

  _wrappers.emplace_back(Wrapper::Type::W_Text,
                         static_cast<Wrapper::Data>(&text));
}

void Canvas::addPanels(Panels &panels) {

  _wrappers.emplace_back(Wrapper::Type::W_Panels,
                         static_cast<Wrapper::Data>(&panels));
}

PLINT Canvas::lookUpColor(std::string_view hex) {

  auto index = std::find(_colormap.begin(), _colormap.end(), hex);

  PLINT entry = std::distance(_colormap.begin(), index);

  if (index == _colormap.end()) {

    _colormap.emplace_back(hex);

    std::stringstream ss;

    ss << std::hex << hex.substr(0, 6);

    int rgb;

    ss >> rgb;

    int a = 255;

    if (hex.size() == 8) {

      ss.str(std::string());

      ss.clear();

      ss << std::hex << hex.substr(6);

      ss >> a;
    }

    _plstream->scol0a(entry, (rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff,
                      static_cast<float>(a) / 255.0f);
  }

  return (entry);
}

void Canvas::paintOnScreen() {

  _plstream = std::make_unique<plstream>();

  _plstream->sdev("xwin");

  _plstream->spause(true);

  _plstream->spage(72.0, 72.0, _size.at(0), _size.at(1), 0.0, 0.0);

  paint();
}

void Canvas::paintOnPostscript(std::string_view filename) {

  _plstream = std::make_unique<plstream>();

  _plstream->sdev("epscairo");

  std::string f(filename);

  _plstream->sfnam(f.append(".ps").c_str());

  _plstream->spause(false);

  _plstream->spage(0.0, 0.0, _size.at(0), _size.at(1), 0.0, 0.0);

  //_plstream->sdiori(1);

  _plstream->setopt("aspect", "1");

  paint();
}

void Canvas::paintOnPNG(std::string_view filename) {

  _plstream = std::make_unique<plstream>();

  _plstream->sdev("pngcairo");

  std::string f(filename);

  _plstream->sfnam(f.append(".png").c_str());

  _plstream->spause(false);

  _plstream->spage(0.0, 0.0, _size.at(0), _size.at(1), 0.0, 0.0);

  _defaultcharacterheight *= 1.5;

  paint();
}

void Canvas::paintOnJPEG(std::string_view filename) {

  _plstream = std::make_unique<plstream>();

  _plstream->sdev("cairojpeg");

  std::string f(filename);

  _plstream->sfnam(f.append(".jpeg").c_str());

  _plstream->spause(false);

  _plstream->spage(0.0, 0.0, _size.at(0), _size.at(1), 0.0, 0.0);

  _defaultcharacterheight *= 1.5;

  paint();
}

void Canvas::paint() {

  bool hadfirstpage = false;

  _plstream->scmap0n(64);

  _colormap.clear();

  lookUpColor(_color);

  _plstream->init();

  _plstream->fontld(1);

  _plstream->font(2);

  _plstream->schr(_defaultcharacterheight, 1.0);

  Panels *panels;

  Plot *plot;

  Text *text;

  std::array<double, 2> margins;

  for (auto &w : _wrappers) {

    switch (w.getType()) {

    case Wrapper::Type::W_Plot: {

      plot = static_cast<Plot *>(w.getData());

      if (plot->isAdvance() || !hadfirstpage) {

        _plstream->adv(0);

        hadfirstpage = true;
      }

      drawPlot(plot);
    } break;

    case Wrapper::Type::W_Panels: {
      if (!hadfirstpage) {

        _plstream->adv(0);

        hadfirstpage = true;
      }

      panels = static_cast<Panels *>(w.getData());

      margins[0] = panels->getYMargins().at(0);

      margins[1] = panels->getYMargins().at(0) +
                   (panels->getYMargins().at(1) - panels->getYMargins().at(0)) /
                       static_cast<double>(panels->getRows());

      for (auto &panel : *panels) {

        plot = static_cast<Plot *>(panel.getData());

        plot->setYMargins(margins);

        if (plot->isAdvance()) {

          margins[0] = margins[1];

          margins[1] = margins[1] + (panels->getYMargins().at(1) -
                                     panels->getYMargins().at(0)) /
                                        static_cast<double>(panels->getRows());
        }

        drawPlot(plot);
      }

    } break;
    case Wrapper::Type::W_Text: {

      text = static_cast<Text *>(w.getData());

      _plstream->vpor(0.0, 1.0, 0.0, 1.0);

      _plstream->wind(0.0, 1.0, 0.0, 1.0);

      _plstream->col0(lookUpColor(text->getColor()));

      _plstream->schr(_defaultcharacterheight, text->getSize());

      _plstream->ptex(text->getCoordinates().at(0),
                      text->getCoordinates().at(1),
                      cosf(M_PI * text->getAngle() / 180.0),
                      sinf(M_PI * text->getAngle() / 180.0),
                      text->getJustification(), text->getText().data());
    } break;
    default: {

      throw(Exception("Unsupported graphics type"));
    } break;
    }
  }
}

void Canvas::drawPlot(Plot *plot) {

  Point *point;

  Line *line;

  Curve *curve;

  Text *text;

  _plstream->vpor(plot->getXMargins().at(0), plot->getXMargins().at(1),
                  plot->getYMargins().at(0), plot->getYMargins().at(1));

  float xmin, xmax, ymin, ymax;

  xmin = plot->getXLimits().at(0);

  xmax = plot->getXLimits().at(1);

  ymin = plot->getYLimits().at(0);

  ymax = plot->getYLimits().at(1);

  _plstream->wind(xmin, xmax, ymin, ymax);

  _plstream->width(plot->getLineWidth());

  _plstream->lsty(static_cast<PLINT>(plot->getLineStyle()));

  if (plot->isDrawHorizontalGrid() || plot->isDrawHorizontalFineGrid()) {

    _plstream->col0(lookUpColor(plot->getHorizontalGrid().getColor()));

    _plstream->lsty(
        static_cast<PLINT>(plot->getHorizontalGrid().getLineStyle()));

    _plstream->width(plot->getHorizontalGrid().getLineWidth());

    if (plot->isDrawHorizontalFineGrid()) {

      _plstream->box("gh", 0.0, 0.0, "", 0.0, 0.0);
    } else {

      _plstream->box("g", 0.0, 0.0, "", 0.0, 0.0);
    }
  }

  if (plot->isDrawVerticalGrid() || plot->isDrawVerticalFineGrid()) {

    _plstream->col0(lookUpColor(plot->getVerticalGrid().getColor()));

    _plstream->lsty(static_cast<PLINT>(plot->getVerticalGrid().getLineStyle()));

    _plstream->width(plot->getVerticalGrid().getLineWidth());

    if (plot->isDrawVerticalFineGrid()) {

      _plstream->box("", 0, 0, "gh", 0, 0);
    } else {

      _plstream->box("", 0, 0, "g", 0, 0);
    }
  }

  for (auto &p : *plot) {

    switch (p.getType()) {

    case Wrapper::Type::W_Point: {

      point = static_cast<Point *>(p.getData());

      _plstream->col0(lookUpColor(point->getColor()));

      _plstream->width(plot->getLineWidth());

      _plstream->lsty(static_cast<PLINT>(plot->getLineStyle()));

      _plstream->ssym(_defaultcharacterheight, point->getSize());

      PLFLT px = point->getCoordinates().at(0);

      PLFLT py = point->getCoordinates().at(1);

      _plstream->sym(1, &px, &py, point->getSymbol());
    } break;
    case Wrapper::Type::W_Line: {

      line = static_cast<Line *>(p.getData());

      _plstream->col0(lookUpColor(line->getColor()));

      _plstream->lsty(static_cast<PLINT>(line->getLineStyle()));

      _plstream->width(line->getLineWidth());

      _plstream->join(
          line->getStartCoordinates().at(0), line->getStartCoordinates().at(1),
          line->getEndCoordinates().at(0), line->getEndCoordinates().at(1));
    } break;
    case Wrapper::Type::W_Curve: {

      curve = static_cast<Curve *>(p.getData());

      if (curve->getX().size() == 0) {

        continue;
      }

      const PLFLT *x = curve->getX().data();

      const PLFLT *xerr = curve->getXErr().data();

      const PLFLT *y = curve->getY().data();

      const PLFLT *yerr = curve->getYErr().data();

      size_t n = curve->getX().size();

      if (xerr || yerr) {

        _plstream->lsty(static_cast<PLINT>(curve->getLineStyle()));

        _plstream->width(curve->getLineWidth());

        _plstream->col0(lookUpColor(curve->getColor()));

        std::vector<PLFLT> min(n);

        std::vector<PLFLT> max(n);

        if (xerr) {

          std::transform(curve->getX().begin(), curve->getX().end(),
                         curve->getXErr().begin(), min.begin(),
                         std::minus<PLFLT>());

          std::transform(curve->getX().begin(), curve->getX().end(),
                         curve->getXErr().begin(), max.begin(),
                         std::plus<PLFLT>());

          _plstream->errx(n, x, min.data(), max.data());
        }
        if (yerr) {

          std::transform(curve->getY().begin(), curve->getY().end(),
                         curve->getYErr().begin(), min.begin(),
                         std::minus<PLFLT>());

          std::transform(curve->getY().begin(), curve->getY().end(),
                         curve->getYErr().begin(), max.begin(),
                         std::plus<PLFLT>());

          _plstream->erry(n, x, min.data(), max.data());
        }
      }

      if (curve->isFill() && curve->getSymbol() == 0) {

        _plstream->col0(lookUpColor(curve->getFillColor()));

        if (y[0] == 0.0f && y[n - 1] == 0.0f) {

          _plstream->fill(n, x, y);
        } else {

          std::vector<PLFLT> a(curve->getX().begin(), curve->getX().end());

          std::vector<PLFLT> b(curve->getY().begin(), curve->getY().end());

          if (y[n - 1] != 0.0f) {

            a.push_back(x[n - 1]);

            b.push_back(0.0f);
          }

          if (y[0] != 0.0f) {

            a.insert(a.begin(), x[0]);

            b.insert(b.begin(), 0.0f);
          }

          _plstream->fill(a.size(), a.data(), b.data());
        }
      }

      _plstream->col0(lookUpColor(curve->getColor()));

      if (curve->getSymbol() == 0) {

        _plstream->lsty(static_cast<PLINT>(curve->getLineStyle()));

        _plstream->width(curve->getLineWidth());

        _plstream->line(n, x, y);
      } else {

        _plstream->ssym(_defaultcharacterheight, curve->getSymbolSize());

        _plstream->poin(n, x, y, curve->getSymbol());
      }
    } break;
    case Wrapper::Type::W_Text: {

      text = static_cast<Text *>(p.getData());

      _plstream->col0(lookUpColor(text->getColor()));

      _plstream->width(plot->getLineWidth());

      _plstream->lsty(static_cast<PLINT>(plot->getLineStyle()));

      _plstream->schr(_defaultcharacterheight, text->getSize());

      if (text->getSystem() == Text::CoordinateSystem::DATA) {

        _plstream->ptex(text->getCoordinates().at(0),
                        text->getCoordinates().at(1),
                        (xmax - xmin) * cosf(M_PI * text->getAngle() / 180.0),
                        (ymax - ymin) * sinf(M_PI * text->getAngle() / 180.0),
                        text->getJustification(), text->getText().data());
      } else if (text->getSystem() == Text::CoordinateSystem::NORMAL) {

        _plstream->ptex(xmin + (xmax - xmin) * text->getCoordinates().at(0),
                        ymin + (ymax - ymin) * text->getCoordinates().at(1),
                        (xmax - xmin) * cosf(M_PI * text->getAngle() / 180.0),
                        (ymax - ymin) * sinf(M_PI * text->getAngle() / 180.0),
                        text->getJustification(), text->getText().data());
      }
    } break;
    case Wrapper::Type::W_Panels:
    default:

      throw(Exception("Unsupported graphics type"));
      break;
    }
  }

  _plstream->col0(lookUpColor(plot->getColor()));

  _plstream->lsty(static_cast<PLINT>(plot->getLineStyle()));

  _plstream->width(plot->getLineWidth());

  _plstream->schr(_defaultcharacterheight, plot->getFontSize());

  _plstream->smaj(_defaultticklength, plot->getMajorTickLength());

  _plstream->smin(_defaultticklength, plot->getMinorTickLength());

  for (auto &ax : plot->getXAxis()) {

    _plstream->sxax(ax.getMaxDigits(), ax.getPrecision());

    if (ax.getAxisOptString().find('n') != std::string::npos &&
        ax.getStyle() == Axis::Style::WritePowerInFrame) {

      float order;

      order = floor(log10(xmax));

      if (std::abs(order) > 1.0) {

        std::ostringstream ostr;

        ostr << "(x10#u" << order << "#d)";

        _plstream->ptex(0.9 * (xmax - xmin) + xmin, 0.05 * (ymax - ymin) + ymin,
                        xmax, 0.0, 0.0, ostr.str().c_str());

        xmin /= pow(10, order);

        xmax /= pow(10, order);
      }
    }

    if (ax.getStyle() != Axis::Style::Default) {

      _plstream->wind(xmin, xmax, ymin, ymax);
    }

    if (ax.getTickFinder()) {

      std::string axisoptstr(ax.getAxisOptString());

      const std::string opts("mnst");

      std::string::size_type i;
      for (const char opt : opts) {

        if ((i = axisoptstr.find(opt)) != std::string::npos) {

          axisoptstr.erase(i, 1);
        }
      }

      _plstream->axes(0.0, 0.0, axisoptstr.data(), 0, 0, "", 0, 0);

      PLFLT p_xmin, p_xmax, p_ymin, p_ymax;

      _plstream->gspa(p_xmin, p_xmax, p_ymin, p_ymax);

      PLFLT dx = abs(xmax - xmin);

      double dy = (axisoptstr.find('i') == std::string::npos ? -1.0 : 1.0) *
                  (ymax - ymin) * _defaultticklength *
                  plot->getMajorTickLength() /
                  ((plot->getYMargins().at(1) - plot->getYMargins().at(0)) *
                   (p_ymax - p_ymin));

      PLFLT ticks[16];

      PLINT nticks = ax.getTickFinder()(PL_X_AXIS, sizeof(ticks), ticks, xmin,
                                        xmax, nullptr);

      for (PLINT i = 0; i < nticks; i++) {

        _plstream->join(ticks[i], ymax + dy, ticks[i], ymax);

        double pos = (ticks[i] - xmin) / dx;

        if (xmin > xmax) {

          pos = (xmin - ticks[i]) / dx;
        }

        char label[16];

        if (ax.getLabelFormatter()) {

          ax.getLabelFormatter()(PL_X_AXIS, ticks[i], label, sizeof(label),
                                 nullptr);
        } else {

          snprintf(label, sizeof(label), "%g", ticks[i]);
        }

        _plstream->mtex("t", 1.5, pos, 0.5, label);
      }
    } else {

      _plstream->slabelfunc(ax.getLabelFormatter(), nullptr);

      _plstream->axes(0.0, 0.0, ax.getAxisOptString().data(), 0, 0, "", 0, 0);
    }

    if (ax.getAxisOptString().find('m') != std::string::npos) {

      _plstream->mtex("t", 3.5, 0.5, 0.5, ax.getTitle().data());
    } else {

      _plstream->mtex("b", 3.5, 0.5, 0.5, ax.getTitle().data());
    }
  }

  for (auto &ax : plot->getYAxis()) {

    _plstream->syax(ax.getMaxDigits(), ax.getPrecision());

    if (ax.getAxisOptString().find('n') != std::string::npos &&
        ax.getStyle() == Axis::Style::WritePowerInFrame) {

      float order;

      order = floor(log10(ymax));

      if (std::abs(order) > 1.0) {

        std::ostringstream ostr;

        ostr << "(x10#u" << order << "#d)";

        _plstream->ptex(0.05 * (xmax - xmin) + xmin, 0.9 * (ymax - ymin) + ymin,
                        (xmax - xmin), 0.0, 0.0, ostr.str().c_str());

        ymin /= pow(10, order);

        ymax /= pow(10, order);
      }
    }

    if (ax.getStyle() != Axis::Style::Default) {

      _plstream->wind(xmin, xmax, ymin, ymax);
    }

    if (ax.getTickFinder()) {

      std::string axisoptstr(ax.getAxisOptString());

      const std::string opts("mnst");

      std::string::size_type i;
      for (const char opt : opts) {

        if ((i = axisoptstr.find(opt)) != std::string::npos) {

          axisoptstr.erase(i, 1);
        }
      }

      _plstream->axes(0.0, 0.0, axisoptstr.data(), 0, 0, "", 0, 0);

      PLFLT p_xmin, p_xmax, p_ymin, p_ymax;

      _plstream->gspa(p_xmin, p_xmax, p_ymin, p_ymax);

      double dx = (axisoptstr.find('i') == std::string::npos ? -1.0 : 1.0) *
                  (xmax - xmin) * _defaultticklength *
                  plot->getMajorTickLength() /
                  ((plot->getXMargins().at(1) - plot->getXMargins().at(0)) *
                   (p_xmax - p_xmin));

      PLFLT dy = abs(ymax - ymin);

      PLFLT ticks[16];

      PLINT nticks = ax.getTickFinder()(PL_Y_AXIS, sizeof(ticks), ticks, ymin,
                                        ymax, nullptr);

      for (PLINT i = 0; i < nticks; i++) {

        _plstream->join(xmax + dx, ticks[i], xmax, ticks[i]);

        double pos = (ticks[i] - ymin) / dy;

        if (ymin > ymax) {

          pos = (ymin - ticks[i]) / dy;
        }

        char label[16];

        if (ax.getLabelFormatter()) {

          ax.getLabelFormatter()(PL_Y_AXIS, ticks[i], label, sizeof(label),
                                 nullptr);
        } else {

          snprintf(label, sizeof(label), "%g", ticks[i]);
        }

        _plstream->mtex("l", 1.5, pos, 0.5, label);
      }
    } else {

      _plstream->slabelfunc(ax.getLabelFormatter(), nullptr);

      _plstream->axes(0.0, 0.0, "", 0, 0, ax.getAxisOptString().data(), 0, 0);
    }

    if (ax.getAxisOptString().find('m') != std::string::npos) {

      _plstream->mtex("r", 3.5, 0.5, 0.5, ax.getTitle().data());
    } else {

      _plstream->mtex("l", 3.5, 0.5, 0.5, ax.getTitle().data());
    }
  }
}
