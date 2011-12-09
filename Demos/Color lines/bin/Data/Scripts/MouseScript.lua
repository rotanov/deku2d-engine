Mouse = GameObject:Derive()

function Mouse:OnCreate()
	self:Subscribe("MouseMove")
end

function Mouse:OnMouseMove(event)
	local x = GetEventData(event, 'X')
	local y = GetEventData(event, 'Y')
	self.pos:SetPosition(x, y)
end