A = GameObject:Derive()

function A:OnCreate()
	self.depth = 1
	self:Subscribe("Attached")
	self:Subscribe("EveryFrame")
end

function A:OnAttached(event)
	if GetEventData(event, "Name") ~= self:GetName() then
		return
	end
	self.model:SetColor(1, 0, 0, 1)
end

function A:OnEveryFrame()
	-- Here until order of attach and creation will not be Ok.
	if self.A then
		self.A.depth = self.depth + 1
	end
	self.pos:SetAngle( self.pos:GetAngle() + self.depth * 100.0 * GetDeltaTime())
end

--------------------------------------------------------------------------------

--[[
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
--]]
