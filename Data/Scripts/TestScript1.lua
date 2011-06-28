TestBox1 = GameObject:Derive()

function TestBox1:OnCreate()
	self:Subscribe("EveryFrame")
	self:Subscribe("KeyDown")
end

function TestBox1:OnEveryFrame()
	local x, y = self.pos:GetPosition()

	if x >= GetWindowWidth() then
		self:SwitchScript()
	end

	self.pos:SetPosition(x + 150 * GetDeltaTime(), y)
end

function TestBox1:OnKeyDown()
	self:SwitchScript()
end

function TestBox1:SwitchScript()
	self:SetClassName("TestBox2")
	self:SetScript(GetObject("TestScript2"))
end
