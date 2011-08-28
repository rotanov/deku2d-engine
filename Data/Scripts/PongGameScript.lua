PongGame = GameObject:Derive()

function PongGame:OnCreate()
	self.Score = {
		PlayerOne = 0,
		PlayerTwo = 0,
	}

	self.MemUsageText = {
		text = Create("Text", "MemUsageText"),
		pos = Create("PlaceableComponent", "MemUsageTextPlace"),
	}

	self.MemUsageText.pos:SetPosition(10, 30)
	self.MemUsageText.text:SetText("not updated yet")
	self.MemUsageText.pos:Attach(self.MemUsageText.text)
	Root:Attach(self.MemUsageText.pos)

	self:Subscribe("PlayerOneScored")
	self:Subscribe("PlayerTwoScored")
	self:Subscribe("TimerTick")
end

function PongGame:OnPlayerOneScored()
	self.Score.PlayerOne = self.Score.PlayerOne + 1

	GetObject("PlayerOneScoreText"):SetText(self.Score.PlayerOne)
	self:LogScore()
end

function PongGame:OnPlayerTwoScored()
	self.Score.PlayerTwo = self.Score.PlayerTwo + 1
	GetObject("PlayerTwoScoreText"):SetText(self.Score.PlayerTwo)
	self:LogScore()
end

function PongGame:OnTimerTick(event)
	if event.sender ~= GetObject("MemoryUsageUpdateTimer").object then
		return
	end

	self.MemUsageText.text:SetText("Lua memory usage: " .. GetMemoryUsage())
end

function PongGame:LogScore()
	Log("GAME", "Score: " .. self.Score.PlayerOne .. ":" .. self.Score.PlayerTwo)
end
