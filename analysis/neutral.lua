local charge = 0
for i = 1, e.tracks.n do
  charge = charge + e.tracks[i].q
end
return charge == 0
