LuaPerfCounter = GameObject:Derive()

function LuaPerfCounter:OnCreate()
	self:Subscribe("TimerTick")
end

function LuaPerfCounter:OnTimerTick(event)
	if GetEventData(event, "Name") ~= "LuaPerfCounterTimer" then
		return
	end

	self.text:SetText("Lua Memory: " .. GetMemoryUsage())
end
