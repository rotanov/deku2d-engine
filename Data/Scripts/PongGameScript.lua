PongGame = { }

function PongGame:OnCreate()
	self.Score = {
		PlayerOne = 0,
		PlayerTwo = 0,
	}

	self.MemUsageText = CreateNewText("MemUsageText")
	SetPosition(self.MemUsageText, 10, 30)
	SetText(self.MemUsageText, "not updated yet")
	Attach(GetObject("RootGameObject"), self.MemUsageText);

	SubscribeToEvent("PlayerOneScored", self.object)
	SubscribeToEvent("PlayerTwoScored", self.object)
	SubscribeToEvent("TimerTick", self.object)
end

function PongGame:OnPlayerOneScored()
	self.Score.PlayerOne = self.Score.PlayerOne + 1
	self:LogScore()
end

function PongGame:OnPlayerTwoScored()
	self.Score.PlayerTwo = self.Score.PlayerTwo + 1
	self:LogScore()
end

function PongGame:OnEveryFrame()
end

function PongGame:OnTimerTick(event)
	if GetEventSender(event) ~= GetObject("MemoryUsageUpdateTimer") then
		return
	end

	SetText(self.MemUsageText, "Lua memory usage: " .. GetMemoryUsage())
end

function PongGame:LogScore()
	Log("GAME", "Score: " .. self.Score.PlayerOne .. ":" .. self.Score.PlayerTwo)
end
