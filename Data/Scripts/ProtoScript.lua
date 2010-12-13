ProtoScriptable = { }

function ProtoScriptable:OnCreate()
	SubscribeToEvent("TimerTick", self.object)
end

function ProtoScriptable:OnTimerTick(event)
	if GetEventData(event, "Name") ~= "ProtoTimer" then
		return
	end

	SetText(GetObject("ScriptableComponentTestText"), GetText(GetObject("ScriptableComponentTestText")) .. "!")
end
