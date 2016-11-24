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

-- If we find a bad track, we exclude the event
for i, track in ipairs(e.tracks) do
  -- Fit quality
  if track.chi2 / track.ndof > 10 then
    return false
  end
  -- Pseudorapidity cut
  track.eta = eta(track.p)
  if math.abs(track.eta) > 2.4 then
    return false
  end
  -- Pt cut
  track.pt = math.sqrt(sumsq(track.p.x, track.p.y))
  if track.pt < 0.2 then
    return false
  end
end
