#include <fstream>
#include <iostream>

#include <QApplication>

#include "histogram.h"
#include "histogram-qt.h"
#include "main_window.h"
#include "parser.h"
#include "run.h"
#include "run_config.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  // For strtod
  std::setlocale(LC_ALL, "C");

  // Histograms
  run r;
  hist::linear_axis<double> mass_axis = hist::linear_axis<double>(0, 4, 200);
  r.add_fill("mass", mass_axis, [](const event &e) {
    return e.p.norm();
  });

  hist::linear_axis<double> pt_axis = hist::linear_axis<double>(0, 1, 100);
  r.add_fill("pt^2 ~= -t", pt_axis, [](const event &e) {
    return lorentz::pt2(e.p);
  });
  r.add_fill("pt", pt_axis, [](const event &e) {
    return lorentz::pt(e.p);
  });

  hist::linear_axis<double> energy_axis = hist::linear_axis<double>(0, 2, 100);
  r.add_fill("E_pi[0]", energy_axis, [](const event &e) {
    return e.tracks[0].p.t();
  });
  r.add_fill("E_pi[1]", energy_axis, [](const event &e) {
    return e.tracks[1].p.t();
  });

  hist::linear_axis<double> y_axis = hist::linear_axis<double>(-10, 10, 100);
  r.add_fill("y", y_axis, [](const event &e) {
    return lorentz::rapidity(e.p);
  });
  r.add_fill("y_pi[0]", y_axis, [](const event &e) {
    return lorentz::rapidity(e.tracks[0].p);
  });
  r.add_fill("y_pi[1]", y_axis, [](const event &e) {
    return lorentz::rapidity(e.tracks[1].p);
  });

  hist::linear_axis<double> eta_axis = hist::linear_axis<double>(-10, 10, 100);
  r.add_fill("eta", eta_axis, [](const event &e) {
    return lorentz::eta(e.p);
  });

  r.add_fill("eta_pi[0]", eta_axis, [](const event &e) {
    return lorentz::eta(e.tracks[0].p);
  });
  r.add_fill("eta_pi[1]", eta_axis, [](const event &e) {
    return lorentz::eta(e.tracks[1].p);
  });

  hist::linear_axis<double> phi_axis = hist::linear_axis<double>(-3.1415, 3.1415, 90);
  r.add_fill("phi", phi_axis, [](const event &e) {
    return lorentz::phi(e.p);
  });

  r.add_fill("phi_pi[0]", phi_axis, [](const event &e) {
    return lorentz::phi(e.tracks[0].p);
  });
  r.add_fill("phi_pi[1]", phi_axis, [](const event &e) {
    return lorentz::phi(e.tracks[1].p);
  });

  r.add_fill("pt_pi[0]", pt_axis, [](const event &e) {
    return lorentz::pt(e.tracks[0].p);
  });
  r.add_fill("pt_pi[1]", pt_axis, [](const event &e) {
    return lorentz::pt(e.tracks[1].p);
  });
  r.add_fill("pt_pi[max]", pt_axis, [](const event &e) {
    return std::max(lorentz::pt(e.tracks[0].p), lorentz::pt(e.tracks[0].p));
  });
  r.add_fill("pt_pi[min]", pt_axis, [](const event &e) {
    return std::min(lorentz::pt(e.tracks[0].p), lorentz::pt(e.tracks[0].p));
  });

  hist::linear_axis<double> match_axis = hist::linear_axis<double>(-0.5, 1.5, 20);
  r.add_fill("match[pi+ = 0]", match_axis, [](const event &e) {
    return e.tracks[0].matched < 0 ? 0 : e.tracks[0].matched;
  });
  r.add_fill("match[pi- = 1]", match_axis, [](const event &e) {
    return e.tracks[1].matched < 0 ? 1 : e.tracks[1].matched;
  });
  r.add_fill("unique match", match_axis, [](const event &e) {
    return e.tracks[0].matched == e.tracks[1].matched;
  });

  r.add_fill("eta[pi+ match]", eta_axis, [](const event &e) {
    int match = e.tracks[0].matched;
    if (match < 0) { // gen
      return lorentz::eta(e.tracks[0].p);
    }
    for (unsigned i = 0; i < e.tracks.size(); ++i) {
      if (e.tracks[i].matched == 0) {
        return lorentz::eta(e.tracks[i].p);
      }
    }
    return -10.;
  });
  r.add_fill("eta[pi- match]", eta_axis, [](const event &e) {
    int match = e.tracks[0].matched;
    if (match < 0) { // gen
      return lorentz::eta(e.tracks[1].p);
    }
    for (unsigned i = 0; i < e.tracks.size(); ++i) {
      if (e.tracks[i].matched == 1) {
        return lorentz::eta(e.tracks[i].p);
      }
    }
    return 10.;
  });

  // Cuts
  run_config *rc = new run_config;
  rc->add_cut("2 tracks", [](const event &e) {
    return e.tracks.size() == 2;
  });
  rc->add_cut("4 tracks", false, [](const event &e) {
    return e.tracks.size() == 4;
  });
  rc->add_cut("pt(pi) > 0.2", [](const event &e) {
    return lorentz::pt(e.tracks[0].p) > .2 && lorentz::pt(e.tracks[1].p) > .2;
  });
  rc->add_cut("M(pi pi) > 0.5", [](const event &e) {
    return e.p.norm() > .5;
  });
  rc->add_cut("|eta(pi)| < 2.4", [](const event &e) {
    bool ok = true;
    for (int i = 0; i < 2; ++i) {
      double eta = lorentz::eta(e.tracks[i].p);
      ok &= std::abs(eta) < 2.4;
    }
    return ok;
  });
  rc->add_cut("|y(pi)| > 2", false, [](const event &e) {
    bool ok = true;
    for (int i = 0; i < 2; ++i) {
      double y = lorentz::rapidity(e.tracks[i].p);
      ok &= std::abs(y) > 2;
    }
    return ok;
  });

  hlt_parser hparser("/home/louis/Documents/ULB/MA1/Mémoire/data/out.root");
  root_parser rparser("/home/louis/Documents/ULB/MA1/Mémoire/starlight/data/pipiv_GR_pipi_m_pt_y.root");
  starlight_parser parser("/home/louis/Documents/ULB/MA1/Mémoire/starlight/data/slight.rho.out");

  // Loop over events
  main_window *win = new main_window(r, rc, &hparser);
  win->showMaximized();

  return app.exec();
}
