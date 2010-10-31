PONG_PLAYER_WIDTH = 24
PONG_PLAYER_HEIGHT = 96
PONG_MARGIN = 10

function PongPlayer()
	local t = {
		Position = Vector2(),
		Velocity = Vector2(),
		Acceleration = Vector2(),
		MovingUp = false,
		MovingDown = false,
		OnCreate = function(self)
			SubscribeToEvent("EveryFrame", self.object)
		end,
		MoveUp = function(self)
			self.MovingUp = true
		end,
		MoveDown = function(self)
			self.MovingDown = true
		end,
		Stop = function(self)
			self.MovingUp = false
			self.MovingDown = false
		end,
		OnEveryFrame = function(self)
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

			if self.Position.y < 0 then
				self.Position.y= 0
				self.Velocity.y = -self.Velocity.y * 0.5
			end
			if (self.Position.y + PONG_PLAYER_HEIGHT) > GetWindowHeight() then
				self.Position.y = GetWindowHeight() - PONG_PLAYER_HEIGHT
				self.Velocity.y = -self.Velocity.y * 0.5
			end

			SetPosition(GetParent(self.object), self.Position.x, self.Position.y)
		end,
		GetBox = function(self)
			return CBox(self.Position, self.Position + Vector2(PONG_PLAYER_WIDTH, PONG_PLAYER_HEIGHT))
		end,
	}

	return t
end

function PongPlayerOne()
	local t = PongPlayer()	-- inheritance
	t.Position = Vector2(PONG_MARGIN, PONG_MARGIN)
	t.OnPlayerOneMoveUp = t.MoveUp
	t.OnPlayerOneMoveDown = t.MoveDown
	t.OnPlayerOneStop = t.Stop
	t.OnCreate = function(self)
		PongPlayer().OnCreate(self)	-- calling superclass method
		SubscribeToEvent("PlayerOneMoveUp", self.object)
		SubscribeToEvent("PlayerOneMoveDown", self.object)
		SubscribeToEvent("PlayerOneStop", self.object)
	end
	return t
end

function PongPlayerTwo()
	local t = PongPlayer()
	t.Position = Vector2(GetWindowWidth() - PONG_MARGIN - PONG_PLAYER_WIDTH, PONG_MARGIN)
	t.OnPlayerTwoMoveUp = t.MoveUp
	t.OnPlayerTwoMoveDown = t.MoveDown
	t.OnPlayerTwoStop = t.Stop
	t.OnCreate = function(self)
		PongPlayer().OnCreate(self)
		SubscribeToEvent("PlayerTwoMoveUp", self.object)
		SubscribeToEvent("PlayerTwoMoveDown", self.object)
		SubscribeToEvent("PlayerTwoStop", self.object)
	end
	return t
end

PlayerOneScriptable = PongPlayerOne()	-- instancing

PlayerTwoScriptable = PongPlayerTwo()

