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
		MoveLeft = function(self)
			--self.MovingUp = true
			self.Velocity.x = self.Velocity.x - 10
		end,
		MoveRight = function(self)
			--self.MovingDown = true
			self.Velocity.x = self.Velocity.x + 10
		end,
		Stop = function(self)
			self.MovingUp = false
			self.MovingDown = false
		end,
		OnEveryFrame = function(self)
			if self.MovingUp then
				self.Acceleration.x = -1600
			end
			if self.MovingDown then
				self.Acceleration.x = 1600
			end
			self.Position = self.Position + self.Velocity * GetDeltaTime()
			self.Velocity = self.Velocity + self.Acceleration * GetDeltaTime()
			self.Velocity = self.Velocity * 0.9995
			self.Acceleration = Vector2(0, 0)


			SetPosition(GetParent(self.object), self.Position.x, self.Position.y)
		end,
	}

	return t
end

function PongPlayerOne()
	local t = PongPlayer()	-- inheritance
	t.Position = Vector2(GetWindowWidth()/2 , 100)
	t.OnPlayerMoveLeft = t.MoveLeft
	t.OnPlayerMoveRight = t.MoveRight
	t.OnCreate = function(self)
		PongPlayer().OnCreate(self)	-- calling superclass method
		SubscribeToEvent("PlayerMoveLeft", self.object)
		SubscribeToEvent("PlayerMoveRight", self.object)
	end
	return t
end

PlayerOneScriptable = PongPlayerOne()	-- instancing

