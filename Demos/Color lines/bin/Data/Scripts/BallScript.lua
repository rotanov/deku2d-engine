BallScript = BallScript or { }

function BallScript:OnCreate()
	SubscribeToEvent(self.object, "Attached")
end

function BallScript:OnAttached(event)
	if GetEventData(event, "Name") ~= GetName(self.object) then
		return
	end
	Attach(GetChild(GetObject("LinesGrid").object, 0), GetParent(self.object))
end

