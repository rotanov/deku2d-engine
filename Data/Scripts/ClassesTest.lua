A = A or { }

RecCount = RecCount or 0

function A:OnCreate()
	self.Test = 0
	RecCount = RecCount + 1
	self.velcoef = RecCount
	SubscribeToEvent(self.object, "Attached")
	SubscribeToEvent(self.object, "EveryFrame")
end

function A:OnAttached(event)
	if GetEventData(event, "Name") ~= GetName(self.object) then
		return
	end

	self.Test = self.Test + 1
	--SetColor(GetParent(self), 1, 0, 0, 1)
	SetColor(self.model, 1, 0, 0, 1)
end

function A:OnEveryFrame()
	SetAngle(self.pos, GetAngle(self.pos) + self.velcoef * 10.0 * GetDeltaTime())
end

TempEditScript = TempEditScript or { }

function TempEditScript:OnCreate()
	SubscribeToEvent("KeyDown", self.object)
	SubscribeToEvent("Attached", self.object)
end

function TempEditScript:OnAttached()
	self.edit = GetParent(self.object)
end

function TempEditScript:OnKeyDown(event)
	local sym = GetEventData(event, "Sym")
	local char = GetEventData(event, "Char")
	local old_text = GetText(self.edit)

	if sym == "8" then
		SetText(self.edit, string.sub(GetText(self.edit), 1, -2))
	elseif sym == "13" or sym == "10" then
		return
	else
		SetText(GetParent(self.object), GetText(GetParent(self.object)) .. char)
	end
end
