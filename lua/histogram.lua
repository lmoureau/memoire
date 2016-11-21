histogram = {}
histogram.__index = histogram

function histogram.new()
  return setmetatable({}, histogram)
end

function histogram:fill(value, weight)
  weight = weight or 1
  if self[value] then
    self[value] = self[value] + weight
  else
    self[value] = weight
  end
end

setmetatable(histogram,
             { __call = function(...) return histogram.new(...) end })

histogram_list = {}

function histogram_list.new()
  return setmetatable({}, histogram_list)
end

function histogram_list.__index(self, name)
  self[name] = histogram.new()
  return self[name]
end

setmetatable(histogram_list,
             { __call = function(...) return histogram_list.new(...) end })
