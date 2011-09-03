PONG_BALL_SIZE = 24

BallScriptable = GameObject:Derive()

function BallScriptable:OnCreate()
	self.BallSpeed = 250
	self:Subscribe("EveryFrame")
	self:Subscribe("KeyDown")
	self:Initialize()
end

function BallScriptable:OnAttached()
end

function BallScriptable:Initialize()
	local Section = Random_Int(0 ,1)
	local Angle
	if Section == 1 then
		Angle = Random_Float(0, PI * 0.25)
	else
		Angle = Random_Float(PI * 1.75, PI * 2.0)
	end
	local Sign = Random_Int(0, 1)
	if Sign == 1 then
		Angle = PI - Angle
	end

	self.Velocity = Vector2(cos(Angle), sin(Angle)) * self.BallSpeed
	self.Position = Vector2(GetWindowWidth() * 0.5, GetWindowHeight() * 0.5)

	--SetPosition(self.Place, self.Position.x, self.Position.y)
end

function BallScriptable:OnEveryFrame()
	self.Position = self.Position + self.Velocity * GetDeltaTime()

	if self.Position.y < 0.0 then
		self.Position.y = 0.0
		self.Velocity.y = -self.Velocity.y * 0.5
	end

	if (self.Position.y + PONG_BALL_SIZE) > GetWindowHeight() then
		self.Position.y = GetWindowHeight() - PONG_BALL_SIZE
		self.Velocity.y = -self.Velocity.y * 0.5
	end

	local BallBox = CBox(self.Position - Vector2(PONG_BALL_SIZE) / 2, self.Position + Vector2(PONG_BALL_SIZE, PONG_BALL_SIZE) / 2)
	local CollidedPlayer
	local CollideFlag = 0

	if BallBox:Intersect(PlayerOneScriptable:GetBox()) then
		self.Position.x = self.Position.x + 0.5 * PONG_BALL_SIZE
		CollidedPlayer = PlayerOneScriptable
		CollideFlag = 0.5
	end

	if BallBox:Intersect(PlayerTwoScriptable:GetBox()) then
		self.Position.x = self.Position.x - 0.5 * PONG_BALL_SIZE
		CollidedPlayer = PlayerTwoScriptable
		CollideFlag = -0.5
	end

	if CollideFlag ~= 0 then
		self.Velocity.x = -self.Velocity.x + CollideFlag * Abs(CollidedPlayer.Velocity.y)
		self.Velocity.y = self.Velocity.y + Abs(CollidedPlayer.Velocity.y) * 0.01
		PlaySound("chord")
	end

	if self.Position.x < 0.0 then
		self:Initialize()
		TriggerEvent("PlayerTwoScored", self.object)
	end

	if (self.Position.x + PONG_BALL_SIZE) > GetWindowWidth() then
		self:Initialize()
		TriggerEvent("PlayerOneScored", self.object)
	end
	
	self.parent.parent:SetPosition(self.Position.x, self.Position.y)
end

function BallScriptable:OnKeyDown(event)
	if IsBound(event.Sym, 'General', 'GameReset') then
		self:Initialize()
	end
end
