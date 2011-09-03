Vector2 = Vector2 or { }

local Vector2_mt = {
	__add = function(a, b)
		return Vector2(a.x + b.x, a.y + b.y)
	end,
	__sub = function(a, b)
		return Vector2(a.x - b.x, a.y - b.y)
	end,
	__mul = function(a, b)
		return Vector2(a.x * b, a.y * b)
	end,
	__div = function(a, b)
		return Vector2(a.x / b, a.y / b)
	end,
	__unm = function(a)
		return Vector2(-a.x, -a.y)
	end,
	__eq = function(a, b)
		return a.x == b.x and a.y == b.y
	end,
}

function Vector2(ax, ay)
	local t_ = {
		x = ax or 0,
		y = ay or 0
	}
	return setmetatable(t_, Vector2_mt)
end
