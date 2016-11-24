local e = e
local H = H

local sumsq = function(...)
  local sum = 0
  for i, v in ipairs({...}) do
    sum = sum + v^2
  end
  return sum
end

if e.rho then
  H.ecal_bp:fill(e.ecal.bp.t)
  H.ecal_bm:fill(e.ecal.bm.t)
  H.ecal_ep:fill(e.ecal.ep.t)
  H.ecal_em:fill(e.ecal.em.t)

  H.hcal_bp:fill(e.hcal.bp.t)
  H.hcal_bm:fill(e.hcal.bm.t)
  H.hcal_ep:fill(e.hcal.ep.t)
  H.hcal_em:fill(e.hcal.em.t)
  H.hcal_fp:fill(e.hcal.fp.t)
  H.hcal_fm:fill(e.hcal.fm.t)

  H.castor:fill(e.castor_energy)

  H.rho_mass:fill(e.rho.p:norm())
  H.rho_pt2:fill(sumsq(e.rho.p.x, e.rho.p.y))
end
