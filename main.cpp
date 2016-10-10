#include <fstream>
#include <iostream>

#include <QApplication>

#include "histogram.h"
#include "histogram-qt.h"
#include "main_window.h"
#include "parser.h"
#include "run.h"
#include "run_config.h"

double get_mass(const event &e) {
  return (e.tracks[0].p + e.tracks[1].p).norm();
}

bool cut_pt(const event &e) {
  double pt2_1 = e.tracks[0].p.x() * e.tracks[0].p.x() + e.tracks[0].p.y() * e.tracks[0].p.y();
  double pt2_2 = e.tracks[1].p.x() * e.tracks[1].p.x() + e.tracks[1].p.y() * e.tracks[1].p.y();
  return pt2_1 > .3 * .3 && pt2_2 > .3 * .3;
}

bool cut_m_pi_pi(const event &e) {
  return (e.tracks[0].p + e.tracks[1].p).norm() > .5;
}

bool cut_eta(const event &e) {
  lorentz::vec rho = e.tracks[0].p + e.tracks[1].p;
  lorentz::vec fake = lorentz::vec::mxyz(0, rho.x(), rho.y(), rho.z());
  double eta = std::atanh(fake.z() / fake.t());
  return std::abs(eta) < 2.5;
}

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  // For strtod
  std::setlocale(LC_ALL, "C");

  // Histograms
  run r;
  hist::linear_axis<double> mass_axis = hist::linear_axis<double>(0, 1.5, 75);
  r.add_fill("mass", mass_axis, &get_mass);

  hist::linear_axis<double> pt_axis = hist::linear_axis<double>(0, 1, 100);
  r.add_fill("pt^2 ~= -t", pt_axis, [](const event &e) {
    lorentz::vec rho = e.tracks[0].p + e.tracks[1].p;
    return rho.x() * rho.x() + rho.y() * rho.y();
  });

  hist::linear_axis<double> y_axis = hist::linear_axis<double>(-10, 10, 100);
  r.add_fill("y", y_axis, [](const event &e) {
    lorentz::vec rho = e.tracks[0].p + e.tracks[1].p;
    return 0.5 * std::log((rho.t() + rho.z()) / (rho.t() - rho.z()));
  });

  hist::linear_axis<double> eta_axis = hist::linear_axis<double>(-10, 10, 100);
  r.add_fill("eta", eta_axis, [](const event &e) {
    return lorentz::eta(e.tracks[0].p + e.tracks[1].p);
  });

  r.add_fill("eta_pi[0]", eta_axis, [](const event &e) {
    return lorentz::eta(e.tracks[0].p);
  });
  r.add_fill("eta_pi[1]", eta_axis, [](const event &e) {
    return lorentz::eta(e.tracks[1].p);
  });

  hist::linear_axis<double> phi_axis = hist::linear_axis<double>(-3.1415, 3.1415, 180);
  r.add_fill("phi", phi_axis, [](const event &e) {
    return lorentz::phi(e.tracks[0].p + e.tracks[1].p);
  });

  r.add_fill("phi_pi[0]", phi_axis, [](const event &e) {
    return lorentz::phi(e.tracks[0].p);
  });
  r.add_fill("phi_pi[1]", phi_axis, [](const event &e) {
    return lorentz::phi(e.tracks[1].p);
  });

  r.add_fill("pt_pi[0]^2", pt_axis, [](const event &e) {
    lorentz::vec pi = e.tracks[0].p;
    return pi.x() * pi.x() + pi.y() * pi.y();
  });
  r.add_fill("pt_pi[1]^2", pt_axis, [](const event &e) {
    lorentz::vec pi = e.tracks[1].p;
    return pi.x() * pi.x() + pi.y() * pi.y();
  });

  hist::linear_axis<double> match_axis = hist::linear_axis<double>(-0.5, 1.5, 20);
  r.add_fill("match[pi+ = 0]", match_axis, [](const event &e) {
    return e.tracks[0].matched < 0 ? 0 : e.tracks[0].matched;
  });
  r.add_fill("match[pi- = 1]", match_axis, [](const event &e) {
    return e.tracks[1].matched < 0 ? 1 : e.tracks[1].matched;
  });

  hist::histogram2d h2d(y_axis, eta_axis);
  h2d.bin(hist::bin2d(0, 0));

  // Cuts
  run_config *rc = new run_config;
  rc->add_cut("pt(pi) > 0.3", &cut_pt);
  rc->add_cut("M(pi pi) > 0.5", &cut_m_pi_pi);
  rc->add_cut("|eta(rho)| < 2.5", &cut_eta);
  rc->add_cut("|eta(pi)| < 3", [](const event &e) {
    bool ok = true;
    for (int i = 0; i < 2; ++i) {
      double eta = lorentz::eta(e.tracks[i].p);
      ok &= std::abs(eta) < 3;
    }
    return ok;
  });

  root_parser rparser("/user/lmoureaux/memoire/build/pipiv_GR_pipi_m_pt_y.root");

  // Loop over events
  starlight_parser parser("/user/lmoureaux/memoire/build/slight.rho.out");
  main_window *win = new main_window(r, rc, &rparser);
  win->showMaximized();

  return app.exec();
}
