TextMovementScriptable = TextMovementScriptable or { }

function TextMovementScriptable:OnCreate()
	self.dir = {x = 1, y = 1}

	SubscribeToEvent("EveryFrame", self.object)
	SubscribeToEvent("KeyDown", self.object)
	SubscribeToEvent("TimerTick", self.object)
end

function TextMovementScriptable:OnEveryFrame()
	local cur_x, cur_y
	cur_x, cur_y = GetPosition(GetObject("TextPos"))

	if cur_x < 0 or cur_x > GetWindowWidth() then
		self.dir.x = -self.dir.x
	end
	if cur_y < 0 or cur_y > GetWindowHeight() then
		self.dir.y = -self.dir.y
	end

	SetPosition(GetObject("TextPos"), cur_x + self.dir.x * GetDeltaTime() * 100, cur_y + self.dir.y * GetDeltaTime() * 100)
end

function TextMovementScriptable:OnKeyDown()
	--DebugPrintComponentTree()
end

function TextMovementScriptable:OnTimerTick(event)
	if GetEventData(event, "Name") ~= "TextChangeTimer" then
		return
	end

	SetText(GetObject("TestText"), GetText(GetObject("TestText")) .. "!")
end

RotateScriptable = RotateScriptable or { }

function RotateScriptable:OnCreate()
	SubscribeToEvent("EveryFrame", self.object)
end

function RotateScriptable:OnEveryFrame()
	SetAngle(GetObject("Magic square"), GetAngle(GetObject("Magic square")) + 200.0 * GetDeltaTime())
	SetAngle(GetParent(GetObject("Mouse cursor")), GetAngle(GetParent(GetObject("Mouse cursor"))) + 100.0 * GetDeltaTime())
	SetAngle(GetObject("Magic circle"), GetAngle(GetObject("Magic circle")) + 100.0 * GetDeltaTime())
end
