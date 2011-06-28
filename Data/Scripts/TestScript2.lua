TestBox2 = GameObject:Derive()

function TestBox2:OnCreate()
	self:Subscribe("EveryFrame")
	self:Subscribe("KeyDown")
end

function TestBox2:OnEveryFrame()
	local x, y = self.pos:GetPosition()

	if x <= 0 then
		self:SwitchScript()
	end

	self.pos:SetPosition(x - 150 * GetDeltaTime(), y)
end

function TestBox2:OnKeyDown()
	self:SwitchScript()
end

function TestBox2:SwitchScript()
	self:SetClassName("TestBox1")
	self:SetScript(GetObject("TestScript1"))
end
