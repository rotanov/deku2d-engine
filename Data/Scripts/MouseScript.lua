Mouse = GameObject:Derive()

function Mouse:OnCreate()
	self:Subscribe("MouseMove")
end

function Mouse:OnMouseMove(event)
	local x = event.X
	local y = event.Y
	self.pos:SetPosition(x, y)
end
