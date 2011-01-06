ParentRecolorer = ParentRecolorer or { }

RecCount = RecCount or 0

function ParentRecolorer:OnCreate()
	self.Test = 0
	RecCount = RecCount + 1
	self.velcoef = RecCount
	SubscribeToEvent("Attached", self.object)
	SubscribeToEvent("EveryFrame", self.object)
end

function ParentRecolorer:OnAttached(event)
	if GetEventData(event, "Name") ~= GetName(self.object) then
		return
	end

	self.Test = self.Test + 1
	SetColor(GetParent(self.object), 1, 0, 0, 1)
end

function ParentRecolorer:OnEveryFrame()
	SetAngle(GetParent(GetParent(self.object)), GetAngle(GetParent(GetParent(self.object))) + self.velcoef * 100.0 * GetDeltaTime())
end
