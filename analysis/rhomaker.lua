local math = math
local e = e

local track1 = false
local track2 = false

local mass_770 = function(t1, t2)
  return math.abs((t1.p + t2.p):norm() - 0.775)
end

if e.tracks.n == 2 then
  track1 = e.tracks[1]
  track2 = e.tracks[2]
else
  local mini = 0
  local minj = 0
  local mindist = 10^10
  for i = 2, e.tracks.n do
    local should_break = false
    for j = 1, i-1 do
      if (e.tracks[i].q + e.tracks[j].q == 0) then
        local dist = mass_770(e.tracks[i], e.tracks[j])
        if (dist < mindist) then
          mini = i
          minj = j
          mindist = dist
        end
      end
    end
  end
  track1 = e.tracks[mini]
  track2 = e.tracks[minj]
end

if track1 and track2 then
  local m_pi_pi = (track1.p + track2.p):norm()
  local k1 = vec.m_x_y_z(0.497614, track1.p.x, track1.p.y, track1.p.z)
  local k2 = vec.m_x_y_z(0.497614, track2.p.x, track2.p.y, track2.p.z)
  local m_k_k = (k1 + k2):norm()
  if m_pi_pi < 2 and math.abs(m_k_k - 1.019461) > 0.005 then
    -- Don't let the J/psi slip in
    e.rho = {
      q = track1.q + track2.q,
      p = track1.p + track2.p
    }
  else
    return false
  end
end
