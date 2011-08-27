LuaPerfCounter = GameObject:Derive()

function LuaPerfCounter:OnCreate()
	self:Subscribe("TimerTick")
end

function LuaPerfCounter:OnTimerTick(event)
	if event.Name ~= "LuaPerfCounterTimer" then
		return
	end

	self.text:SetText("Lua Memory: " .. GetMemoryUsage())
end
