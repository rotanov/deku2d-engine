TextMovementScriptable = GameObject:Derive()

function TextMovementScriptable:OnCreate()
	self.dir = {x = 1, y = 1}

	self:Subscribe("EveryFrame")
	self:Subscribe("KeyDown")
	self:Subscribe("TimerTick")
end

function TextMovementScriptable:OnEveryFrame()
	local cur_x, cur_y
	cur_x, cur_y = self.TextPos:GetPosition()
	local box = GetBox(self.TextPos.object)

	if box.Min.x < 0 or box.Max.x > GetWindowWidth() then
		self.dir.x = -self.dir.x
	end
	if box.Min.y < 0 or box.Max.y > GetWindowHeight() then
		self.dir.y = -self.dir.y
	end

	self.TextPos:SetPosition(cur_x + self.dir.x * GetDeltaTime() * 100, cur_y + self.dir.y * GetDeltaTime() * 100)
end

function TextMovementScriptable:OnTimerTick(event)
	if GetEventData(event, "Name") ~= "TextChangeTimer" then
		return
	end

	self.TestText:SetText( self.TestText:GetText() .. "!")
end

--------------------------------------------------------------------------------

SandboxInst = GameObject:Derive()

function SandboxInst:OnCreate()
  self:Subscribe("EveryFrame")
  self:Subscribe("TimerTick")
end

function SandboxInst:OnEveryFrame()
  self.TestText:SetColor(1, 1, 0, 1)
end

function SandboxInst:OnTimerTick(event)
	if GetEventData(event, "Name") ~= "TextChangeTimer" then
		return
	end
	self.TestText:SetText(self.TestText:GetText() .. "?")
end

--------------------------------------------------------------------------------