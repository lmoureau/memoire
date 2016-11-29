local e = e
local H = H
local math = math

local sumsq = function(...)
  local sum = 0
  for i, v in ipairs({...}) do
    sum = sum + v^2
  end
  return sum
end

local eta = function(p)
  local norm = math.sqrt(sumsq(p.x, p.y, p.z))
  return - 0.5 * math.log((norm + p.z) / (norm - p.z))
end

-- Same cuts everywhere else
if e.castor_energy > 9 then
  return
end
if e.hcal.fp.t > 3 or e.hcal.bm.t > 3 then
  return
end
if --[[e.hcal.ep.t > 1.95 or]] e.hcal.em.t > 1.95 then
  return false
end
if e.hcal.bp.t > 1.18 or e.hcal.bm.t > 1.18 then
  return
end
if e.zdc.plus > 500 or e.zdc.minus > 500 then
  return false
end

-- Testing the hcal endcap plus cut
for cut = 1.4, 2.05, 0.05 do
  if e.hcal.ep.t > cut and e.hcal.ep.t < cut + 0.2 then
    H["hcal_ep_eta_" .. cut]:fill(eta(e.hcal.ep))
  end
end
