TextMovementScriptable = TextMovementScriptable or { }

function TextMovementScriptable:OnCreate()
	self.dir = {x = 1, y = 1}

	SubscribeToEvent(self.object, "EveryFrame")
	SubscribeToEvent(self.object, "KeyDown")
	SubscribeToEvent(self.object, "TimerTick")
end

function TextMovementScriptable:OnEveryFrame()
	local cur_x, cur_y
	cur_x, cur_y = GetPosition(self.TextPos)
	local box = GetBox(self.TextPos)

	if box.Min.x < 0 or box.Max.x > GetWindowWidth() then
		self.dir.x = -self.dir.x
	end
	if box.Min.y < 0 or box.Max.y > GetWindowHeight() then
		self.dir.y = -self.dir.y
	end

	SetPosition(self.TextPos, cur_x + self.dir.x * GetDeltaTime() * 100, cur_y + self.dir.y * GetDeltaTime() * 100)
end

function TextMovementScriptable:OnTimerTick(event)
	if GetEventData(event, "Name") ~= "TextChangeTimer" then
		return
	end

	SetText(self.TestText, GetText(self.TestText) .. "!")
end

RotateScriptable = RotateScriptable or { }

function RotateScriptable:OnCreate()
	SubscribeToEvent(self.object, "EveryFrame")
end

function RotateScriptable:OnEveryFrame()
	SetAngle(GetObject("Magic square"), GetAngle(GetObject("Magic square")) + 200.0 * GetDeltaTime())
	SetAngle(GetParent(GetObject("Mouse cursor")), GetAngle(GetParent(GetObject("Mouse cursor"))) + 100.0 * GetDeltaTime())
	SetAngle(GetObject("Magic circle"), GetAngle(GetObject("Magic circle")) + 100.0 * GetDeltaTime())
end

SandboxInst = SandboxInst or {}

function SandboxInst:OnCreate()
  SubscribeToEvent(self.object, "EveryFrame")
  SubscribeToEvent(self.object, "TimerTick")
end

function SandboxInst:OnEveryFrame()
  SetColor( self['TestText'] , 1, 1, 0, 1)
end

function SandboxInst:OnTimerTick(event)
	if GetEventData(event, "Name") ~= "TextChangeTimer" then
		return
	end

	SetText(self.TestText, GetText(self.TestText) .. "?")
end

DumpGlobals()