CBox = { }

function CBox(AMin, AMax)
	local t = {
		Min = AMin or Vector2(),
		Max = AMax or Vector2(),
		Intersect = function(self, box)
			if box.Min.x >= self.Max.x then
				return false
			end
			if box.Min.y >= self.Max.y then
				return false
			end
			if box.Max.x <= self.Min.x then
				return false
			end
			if box.Max.y <= self.Min.y then
				return false
			end

			return true
		end,
	}
	return t
	--return setmetatable(t, CBox_mt)
end
