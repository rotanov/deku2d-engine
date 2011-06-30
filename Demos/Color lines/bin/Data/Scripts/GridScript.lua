GridScript = GridScript or { }

function GridScript:OnCreate()
	SubscribeToEvent(self.object, "MouseDown")
	SubscribeToEvent(self.object, "EveryFrame")
end

function GridScript:OnMouseDown(event)
	--newball = Create("BallProto", "")
	--Attach(self.object, newball)
	--SetPosition(newball, Random_Int(0, 288), Random_Int(0, 288))
end

function GridScript:OnEveryFrame()
end

