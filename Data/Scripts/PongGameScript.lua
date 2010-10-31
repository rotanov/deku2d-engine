PongGame = { }

function PongGame:OnCreate()
	self.Score = {
		PlayerOne = 0,
		PlayerTwo = 0,
	}

	SubscribeToEvent("PlayerOneScored", self.object)
	SubscribeToEvent("PlayerTwoScored", self.object)
end

function PongGame:OnPlayerOneScored()
	self.Score.PlayerOne = self.Score.PlayerOne + 1
	self:LogScore()
end

function PongGame:OnPlayerTwoScored()
	self.Score.PlayerTwo = self.Score.PlayerTwo + 1
	self:LogScore()
end

function PongGame:LogScore()
	Log("GAME", "Score: " .. self.Score.PlayerOne .. ":" .. self.Score.PlayerTwo)
end
