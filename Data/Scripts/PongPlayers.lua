PONG_PLAYER_WIDTH = 24
PONG_PLAYER_HEIGHT = 96
PONG_MARGIN = 10

PongPlayer = GameObject:Derive()

function PongPlayer:OnCreate()
	self.Position = Vector2()
	self.Velocity = Vector2()
	self.Acceleration = Vector2()
	self.MovingUp = false
	self.MovingDown = false
	self:Subscribe("EveryFrame")
	self:Subscribe("KeyDown")
	self:Subscribe("KeyUp")
end

function PongPlayer:MoveUp()
	self.MovingUp = true
end

function PongPlayer:MoveDown()
	self.MovingDown = true

end

function PongPlayer:Stop()
	self.MovingUp = false
	self.MovingDown = false
end

function PongPlayer:OnEveryFrame()
	if self.MovingUp then
		self.Acceleration.y = 3200
	end
	if self.MovingDown then
		self.Acceleration.y = -3200
	end
	self.Position = self.Position + self.Velocity * GetDeltaTime()
	self.Velocity = self.Velocity + self.Acceleration * GetDeltaTime()
	self.Velocity = self.Velocity * 0.9995
	self.Acceleration = Vector2(0, 0)

	if (self.Position.y - 48) < 0 then
		self.Position.y= 96/2
		self.Velocity.y = -self.Velocity.y * 0.5
	end
	if (self.Position.y + PONG_PLAYER_HEIGHT/2) > GetWindowHeight() then
		self.Position.y = GetWindowHeight() - PONG_PLAYER_HEIGHT/2
		self.Velocity.y = -self.Velocity.y * 0.5
	end

	self.parent.parent:SetPosition(self.Position.x, self.Position.y)
end

function PongPlayer:GetBox()
	return CBox(self.Position, self.Position + Vector2(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT))
end

PongPlayerOne = PongPlayer:Derive()

function PongPlayerOne:OnCreate()
	PongPlayer.OnCreate(self)
	self.Position = Vector2(PONG_MARGIN + 12, GetWindowHeight()/2)
	self.OnPlayerOneMoveUp = self.MoveUp
	self.OnPlayerOneMoveDown = self.MoveDown
	self.OnPlayerOneStop = self.Stop
end

function PongPlayerOne:OnKeyDown(event)
	if IsBound(event.Sym, 'Player1', 'MoveUp') then
		self:MoveUp()
	elseif IsBound(event.Sym, 'Player1', 'MoveDown') then
		self:MoveDown()
	end
end

function PongPlayerOne:OnKeyUp(event)
	if IsBound(event.Sym, 'Player1', 'MoveUp') or IsBound(event.Sym, 'Player1', 'MoveDown') then
		self:Stop()
	end
end

PongPlayerTwo = PongPlayer:Derive()

function PongPlayerTwo:OnCreate()
	PongPlayer.OnCreate(self)
	self.Position = Vector2(GetWindowWidth() - PONG_MARGIN - PONG_PLAYER_WIDTH/2, PONG_MARGIN)
	self.OnPlayerTwoMoveUp = self.MoveUp
	self.OnPlayerTwoMoveDown = self.MoveDown
	self.OnPlayerTwoStop = self.Stop
end

function PongPlayerTwo:OnKeyDown(event)
	if IsBound(event.Sym, 'Player2', 'MoveUp') then
		self:MoveUp()
	elseif IsBound(event.Sym, 'Player2', 'MoveDown') then
		self:MoveDown()
	end
end

function PongPlayerTwo:OnKeyUp(event)
	if IsBound(event.Sym, 'Player2', 'MoveUp') or IsBound(event.Sym, 'Player2', 'MoveDown') then
		self:Stop()
	end
end
