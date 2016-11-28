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
  return vec.new(-a.t, -a.x, -a.y, -a.z)
end

function vec.__eq(a, b)
  return a.t == b.t and a.x == b.x and a.y == b.y and a.z == b.z
end

function vec.__tostring(a)
  return "(" .. a.t .. ", " .. a.x .. ", " .. a.y .. ", " .. a.z .. ")"
end

function vec.__index(self, i)
  if     i == 0 then return self.t
  elseif i == 1 then return self.x
  elseif i == 2 then return self.y
  elseif i == 3 then return self.z
  else return vec[i]
  end
end

function vec.__newindex(self, i, a)
  if     i == 0 then self.t = a
  elseif i == 1 then self.x = a
  elseif i == 2 then self.y = a
  elseif i == 3 then self.z = a
  else error("vec: index out of bounds: " .. a, -1)
  end
end

vec.__class  = "vec"
vec.__module = "lorentz"

function vec.new(t, x, y, z)
  return setmetatable({ t = t or 0, x = x or 0, y = y or 0, z = z or 0 }, vec)
end

function vec.m_e_phi_eta(m, e, phi, eta)
  local p = math.sqrt(e^2 - m^2)
  local pt = p / math.cosh(eta)
  return vec.new(e, pt * math.cos(phi), pt * math.sin(phi), p * math.tanh(eta))
end

function vec.m_x_y_z(m, x, y, z)
  local r2 = x^2 + y^2 + z^2
  local t = math.sqrt(m^2 + r2)
  return vec.new(t, x, y, z)
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
