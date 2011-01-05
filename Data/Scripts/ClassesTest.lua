ParentRecolorer = ParentRecolorer or { }

function ParentRecolorer:OnCreate()
	self.Test = 0
	SubscribeToEvent("Attached", self.object)
	SubscribeToEvent("KeyDown", self.object)
	SubscribeToEvent("EveryFrame", self.object)
end

function ParentRecolorer:OnAttached(event)
	if GetEventData(event, "Name") ~= GetName(self.object) then
		return
	end

	self.Test = self.Test + 1
	SetColor(GetParent(self.object), 1, 0, 0, 1)
end

function ParentRecolorer:OnKeyDown()
	Log("TEST", GetName(self.object) .. " " .. self.Test)
end

function RotateScriptable:OnEveryFrame()
	SetAngle(GetParent(GetParent(self.object)), GetAngle(GetParent(GetParent(self.object))) + 200.0 * GetDeltaTime())
end
