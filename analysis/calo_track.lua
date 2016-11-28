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

local phi = function(p)
  return math.atan2(p.y, p.x)
end

if not e.rho then return end

-- hcal
for _, track in ipairs(e.tracks) do
  local track_eta = eta(track.p)
  local track_phi = phi(track.p)
  local min_deta = 10^10
  local min_dphi = 10^10
  local min_dR2 = 10^10
  for __, vect in pairs(e.hcal) do
    local deta = math.abs(track_eta - eta(vect))
    if deta < min_deta then
      min_deta = deta
    end
    local dphi = math.abs(track_phi - phi(vect))
    if dphi < min_dphi then
      min_dphi = dphi
    end
    local dR2 = sumsq(deta, dphi)
    if dR2 < min_dR2 then
      min_dR2 = dR2
    end
  end
  H.hcal_min_deta:fill(min_deta)
  H.hcal_min_dphi:fill(min_dphi)
  H.hcal_min_dR:fill(math.sqrt(min_dR2))
end

-- ecal
for _, track in ipairs(e.tracks) do
  local track_eta = eta(track.p)
  local track_phi = phi(track.p)
  local min_deta = 10^10
  local min_dphi = 10^10
  local min_dR2 = 10^10
  for __, vect in pairs(e.ecal) do
    local deta = math.abs(track_eta - eta(vect))
    if deta < min_deta then
      min_deta = deta
    end
    local dphi = math.abs(track_phi - phi(vect))
    if dphi < min_dphi then
      min_dphi = dphi
    end
    local dR2 = sumsq(deta, dphi)
    if dR2 < min_dR2 then
      min_dR2 = dR2
    end
  end
  H.ecal_min_deta:fill(min_deta)
  H.ecal_min_dphi:fill(min_dphi)
  H.ecal_min_dR:fill(math.sqrt(min_dR2))
end
