BallScript = BallScript or { }

function BallScript:OnCreate()
	SubscribeToEvent(self.object, "Attached")
end

function BallScript:OnAttached()
	Attach(GetChild(GetObject("LinesGrid").object, 0), GetParent(self.object))
end

