PongGame = { }

function PongGame:OnCreate()
	self.Score = {
		PlayerOne = 0,
		PlayerTwo = 0,
	}

	self.MemUsageText = {
		text = Create("Text", "MemUsageText"),
		pos = Create("PlaceableComponent", "MemUsageTextPlace"),
	}

	SetPosition(self.MemUsageText.pos, 10, 30)
	SetText(self.MemUsageText.text, "not updated yet")
	Attach(self.MemUsageText.pos, self.MemUsageText.text);
	Attach(GetObject("RootGameObject"), self.MemUsageText.pos);

	SubscribeToEvent("PlayerOneScored", self.object)
	SubscribeToEvent("PlayerTwoScored", self.object)
	SubscribeToEvent("TimerTick", self.object)
end

function PongGame:OnPlayerOneScored()
	self.Score.PlayerOne = self.Score.PlayerOne + 1
	SetText(GetObject("PlayerOneScoreText"), self.Score.PlayerOne)
	self:LogScore()
end

function PongGame:OnPlayerTwoScored()
	self.Score.PlayerTwo = self.Score.PlayerTwo + 1
	SetText(GetObject("PlayerTwoScoreText"), self.Score.PlayerTwo)
	self:LogScore()
end

function PongGame:OnEveryFrame()
end

function PongGame:OnTimerTick(event)
	if GetEventSender(event) ~= GetObject("MemoryUsageUpdateTimer") then
		return
	end

	SetText(self.MemUsageText.text, "Lua memory usage: " .. GetMemoryUsage())
end

function PongGame:LogScore()
	Log("GAME", "Score: " .. self.Score.PlayerOne .. ":" .. self.Score.PlayerTwo)
end