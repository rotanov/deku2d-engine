ScriptableComponent = { }

function ScriptableComponent:OnCreate()
	self.dir = {x = 1, y = 1}
	self.text = CreateNewText("ScriptableComponentTestText")
	Attach(self.object, self.text);
	SetPosition(self.text, 150, 250)
	SetText(self.text, "Deku2D engine rules the fucking world!")

	SubscribeToEvent("EveryFrame", self.object)

	Log("LUATEST", "ScriptableComponent created on Lua side")
end

function ScriptableComponent:OnEveryFrame()
	local cur_x, cur_y
	cur_x, cur_y = GetPosition(self.text)

	if cur_x < 0 or cur_x > GetWindowWidth() then
		self.dir.x = -self.dir.x
	end
	if cur_y < 0 or cur_y > GetWindowHeight() then
		self.dir.y = -self.dir.y
	end

	SetPosition(self.text, cur_x + self.dir.x, cur_y + self.dir.y)
end
