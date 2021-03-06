#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>

#include <QApplication>

#include "cached_event_source.h"
#include "histogram.h"
#include "histogram-qt.h"
#include "main_window.h"
#include "parsers.h"
#include "run.h"
#include "run_config.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  // For strtod
//  std::setlocale(LC_ALL, "C");

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

  hist::linear_axis<double> chi2_axis = hist::linear_axis<double>(0, 15, 100);
  r.add_fill("chi2/ndof[0]", chi2_axis, [](const event &e) {
    return e.tracks[0].chi2 / e.tracks[0].ndof;
  });
  r.add_fill("chi2/ndof[1]", chi2_axis, [](const event &e) {
    return e.tracks[1].chi2 / e.tracks[1].ndof;
  });

  hist::linear_axis<double> x_axis = hist::linear_axis<double>(0, 0.1, 100);
  r.add_fill("|vec_x[0] - vec_x[1]|", x_axis, [](const event &e) {
    return std::abs((e.tracks[0].p - e.tracks[1].p).norm());
  });

  hist::linear_axis<double> castor_energy_axis =
      hist::linear_axis<double>(-15, 30, 100);
  r.add_fill("Castor E", castor_energy_axis, [](const event &e) {
    return e.castor_status.energy();
  });

  hist::linear_axis<double> hcal_energy_axis =
      hist::linear_axis<double>(0, 5, 100);
  r.add_fill("HCal Emax (b+)", hcal_energy_axis, [](const event &e) {
    return e.hcal.barrel.plus;
  });
  r.add_fill("HCal Emax (b-)", hcal_energy_axis, [](const event &e) {
    return e.hcal.barrel.minus;
  });
  r.add_fill("HCal Emax (e+)", hcal_energy_axis, [](const event &e) {
    return e.hcal.endcap.plus;
  });
  r.add_fill("HCal Emax (e-)", hcal_energy_axis, [](const event &e) {
    return e.hcal.endcap.minus;
  });
  r.add_fill("HCal Emax (f+)", hcal_energy_axis, [](const event &e) {
    return e.hcal.forward.plus;
  });
  r.add_fill("HCal Emax (f-)", hcal_energy_axis, [](const event &e) {
    return e.hcal.forward.minus;
  });

  hist::linear_axis<double> ecal_energy_axis =
      hist::linear_axis<double>(0, 5, 100);
  r.add_fill("ECal Emax (b+)", ecal_energy_axis, [](const event &e) {
    return e.ecal.barrel.plus;
  });
  r.add_fill("ECal Emax (b-)", ecal_energy_axis, [](const event &e) {
    return e.ecal.barrel.minus;
  });
  r.add_fill("ECal Emax (e+)", ecal_energy_axis, [](const event &e) {
    return e.ecal.endcap.plus;
  });
  r.add_fill("ECal Emax (e-)", ecal_energy_axis, [](const event &e) {
    return e.ecal.endcap.minus;
  });

  // Cuts
  run_config *rc = new run_config;
  rc->add_lua_cut("Castor E < 9", "return e.castor_energy < 9");
  rc->add_lua_cut("HE Emax < 1.92", "return e.hcal.ep < 1.92 and e.hcal.em < 1.92");
  rc->add_lua_cut("2 tracks", "return e.tracks.n == 2");
  rc->add_lua_cut("4 tracks", "return e.tracks.n == 4", false);
  rc->add_lua_cut("neutral event",
                  "local c = 0;"
                  "for i = 0, e.tracks.n - 1 do c = c + e.tracks[i].q; end;"
                  "return c == 0");
  rc->add_cut("2 tracks with |M - 775| < 100", false, [](const event &e) {
    for (auto &t1 : e.tracks) {
      for (auto &t2 : e.tracks) {
        if (std::abs((t1.p + t2.p).norm() - 0.775) < 0.1) {
          return true;
        }
      }
    }
    return false;
  });
  rc->add_cut("2 tracks with |M - 775| < 100, neutral", false, [](const event &e) {
    for (auto &t1 : e.tracks) {
      for (auto &t2 : e.tracks) {
        if (std::abs((t1.p + t2.p).norm() - 0.775) < 0.1 &&
            t1.charge + t2.charge == 0) {
          return true;
        }
      }
    }
    return false;
  });
  rc->add_lua_cut("chi2/ndof < 10",
                  "for i = 0, e.tracks.n - 1 do "
                    "if e.tracks[i].chi2 / e.tracks[i].ndof >= 10 then "
                      "return false;"
                    "end;"
                  "end;"
                  "return true");
  rc->add_lua_cut("pt > .2",
                  "for i = 0, e.tracks.n - 1 do "
                    "if math.sqrt(e.tracks[i].p.x^2 + e.tracks[i].p.y^2) <= .2 then "
                      "return false;"
                    "end;"
                  "end;"
                  "return true");
  rc->add_cut("M(pi pi) > 0.5", [](const event &e) {
    return e.p.norm() > .5;
  });
  rc->add_cut("|eta(pi)| < 2.4", [](const event &e) {
    return std::all_of(e.tracks.begin(), e.tracks.end(), [](const track &trk) {
      return std::abs(lorentz::eta(trk.p)) < 2.4;
    });
  });
  rc->add_cut("|y(pi)| > 2", false, [](const event &e) {
    return std::all_of(e.tracks.begin(), e.tracks.end(), [](const track &trk) {
      return std::abs(lorentz::rapidity(trk.p)) > 2;
    });
  });

  hlt_parser hparser("/home/louis/Documents/ULB/MA1/Mémoire/data/out.root");
  root_parser rparser("/home/louis/Documents/ULB/MA1/Mémoire/starlight/data/pipiv_GR_pipi_m_pt_y.root");
  starlight_parser parser("/home/louis/Documents/ULB/MA1/Mémoire/starlight/data/slight.rho.out");

  // Loop over events
  main_window *win = new main_window(r, rc,
                                     new cached_event_source(&hparser, 300000));
  win->showMaximized();

  return app.exec();
}
