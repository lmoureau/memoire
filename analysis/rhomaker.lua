local math = math
local e = e

local track1 = 0
local track2 = 0

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
  e.rho = {
    q = track1.q + track2.q,
    p = track1.p + track2.p
  }
end
