ScriptableComponent = { }

function ScriptableComponent:OnCreate()
	self.dir = {x = 1, y = 1}
	self.text_pos = Create("PlaceableComponent", "TextPlace")
	self.text = Create("Text", "ScriptableComponentTestText")
	Attach(self.object, self.text_pos);
	Attach(self.text_pos, self.text);
	SetPosition(self.text_pos, 150, 250)
	SetText(self.text, "Deku2D engine rules the fucking world!")

	SubscribeToEvent("EveryFrame", self.object)
	SubscribeToEvent("KeyDown", self.object)

	Log("LUATEST", "ScriptableComponent created on Lua side")
end

function ScriptableComponent:OnEveryFrame()
	local cur_x, cur_y
	cur_x, cur_y = GetPosition(self.text_pos)

	if cur_x < 0 or cur_x > GetWindowWidth() then
		self.dir.x = -self.dir.x
	end
	if cur_y < 0 or cur_y > GetWindowHeight() then
		self.dir.y = -self.dir.y
	end

	SetPosition(self.text_pos, cur_x + self.dir.x * GetDeltaTime() * 100, cur_y + self.dir.y * GetDeltaTime() * 100)
end

function ScriptableComponent:OnKeyDown()
	DebugPrintComponentTree()
end
