local e = e

if e.castor_energy > 9 then
  return false
end
if e.hcal.fm.t > 3 or e.hcal.bp.t > 3 then
  return false
end
if e.hcal.em.t > 1.95 or e.hcal.ep.t > 1.95 then
  return false
end
if e.hcal.bm.t > 1.18 or e.hcal.bp.t > 1.18 then
  return false
end
if e.zdc.plus > 500 or e.zdc.minus > 500 then
  return false
end

return true
