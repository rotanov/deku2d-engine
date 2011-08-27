EventSender = GameObject:Derive()

function EventSender:OnCreate()
	self.Timer = Create("TimerComponent", "EventSendTimer")
	self.Timer:SetInterval(5)
	self.Timer:SetEnabled(true)
	self:Subscribe("TimerTick")
end

function EventSender:OnDestroy()
	self:Unsubscribe("TimerTick")
end

function EventSender:OnTimerTick(e)
	if e.Name ~= "EventSendTimer" then
		return
	end

	ne = {
		Text = "testing Lua-sent events..."
	}

	TriggerEvent("SomeCustomEvent", self.object, ne)
end

EventReceiver = GameObject:Derive()

function EventReceiver:OnCreate()
	self:Subscribe("SomeCustomEvent")
end

function EventReceiver:OnDestroy()
	self:Unsubscribe("SomeCustomEvent")
end

function EventReceiver:OnSomeCustomEvent(e)
	Log("EVENTTEST", e.Text);
end
