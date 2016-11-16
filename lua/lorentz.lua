vec = {}
vec.__index = vec

function vec.__add(a, b)
  if type(a) == "number" then
    return vec.new(b.t + a, b.x + a, b.y + a, b.z + a)
  elseif type(b) == "number" then
    return vec.new(a.t + b, a.x + b, a.y + b, a.z + b)
  else
    return vec.new(a.t + b.t, a.x + b.x, a.y + b.y, a.z + b.z)
  end
end

function vec.__sub(a, b)
  if type(a) == "number" then
    return vec.new(a - b.t, a - b.x, a - b.y, a - b.z)
  elseif type(b) == "number" then
    return vec.new(a.t - b, a.x - b, a.y - b, a.z - b)
  else
    return vec.new(a.t - b.t, a.x - b.x, a.y - b.y, a.z - b.z)
  end
end

function vec.__mul(a, b)
  if type(a) == "number" then
    return vec.new(b.t * a, b.x * a, b.y * a, b.z * a)
  elseif type(b) == "number" then
    return vec.new(a.t * b, a.x * b, a.y * b, a.z * b)
  else
    return a.t * a.t - a.x * b.x - a.y * b.y - a.z * b.z
  end
end

function vec.__div(a, b)
  if type(a) == "number" then
    return vec.new(a / b.x, a / b.y)
  elseif type(b) == "number" then
    return vec.new(a.x / b, a.y / b)
  else
    error()
  end
end

function vec.__unm(a)
  return vec.new(a.t, a.x, a.y, a.z)
end

function vec.__eq(a, b)
  return a.t == b.t and a.x == b.x and a.y == b.y and a.z == b.z
end

function vec.__tostring(a)
  return "(" .. a.t .. ", " .. a.x .. ", " .. a.y .. ", " .. a.z .. ")"
end

function vec.__index(i)
  if     i == 0 then return t
  elseif i == 1 then return x
  elseif i == 1 then return y
  elseif i == 1 then return z
  else error()
  end
end

function vec.__newindex(i, a)
  if     i == 0 then t = a
  elseif i == 1 then x = a
  elseif i == 1 then y = a
  elseif i == 1 then z = a
  else error()
  end
end

function vec.new(t, x, y, z)
  return setmetatable({ t = t or 0, x = x or 0, y = y or 0, z = z or 0 }, vec)
end

function vec:clone()
  return vec.new(self.t, self.x, self.y, self.z)
end

function vec:norm()
  return math.sqrt(self * self)
end

function vec:norm2()
  return self * self
end

setmetatable(vec, { __call = function(_, ...) return vec.new(...) end })
