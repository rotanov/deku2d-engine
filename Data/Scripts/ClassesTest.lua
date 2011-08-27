A = GameObject:Derive()

function A:OnCreate()
	self.depth = 1
	self.life = 5
	self.lifesettled = false
	self:Subscribe("EveryFrame")
end

function A:OnAttached()
	self.model:SetColor(1, 0, 0, 1)
end

function A:OnEveryFrame()
	-- Here until order of attach and creation will not be Ok.
	if self.A then
		self.A.depth = self.depth + 1
		if not self.lifesettled then
			--self.A.life = self.life - 0.2
			self.lifesettled = true
		end
	end

	if self.parentProto then
		self.parentProto.model:SetColor(Random_Float(0, 1), Random_Float(0, 1), Random_Float(0, 1), 1)
	end

	self.pos:SetAngle( self.pos:GetAngle() + self.depth * 100.0 * GetDeltaTime())
	self.life = self.life - GetDeltaTime()
	--[[if self.life <= 0 then
		self:DestroyFull()
	end]]
end

--------------------------------------------------------------------------------

BlackNinja = GameObject:Derive()

function BlackNinja:OnCreate()
	self:Subscribe('EveryFrame')
	self:Subscribe('KeyDown')
	self:Subscribe('KeyUp')
	self.v = Vector2(0, 0)
	self.vn = self.v
	self.v_c = 
	{
		[ SDLK._LEFT ] = Vector2( -1, 0 ),
		[ SDLK._RIGHT ] = Vector2( 1, 0 ),
		[ SDLK._UP ] = Vector2( 0, 1 ),
		[ SDLK._DOWN ] = Vector2( 0, -1 ),
	}
end

function BlackNinja:OnKeyDown(event)
	local sym = tonumber(event.Sym)
	local char = event.Char
	local inc = self.v_c[sym]
	if inc then
		self.v = self.v + inc
		local vLen = math.sqrt( self.v.x * self.v.x + self.v.y * self.v.y )
		self.vn = Vector2( self.v.x / vLen, self.v.y / vLen )		
	end
	if sym == SDLK._z then
		self.shooting = true
	end

	if sym == SDLK._RETURN then
		ev = {
			Char = '',
			Sym = SDLK._SPACE,
		}
		TriggerEvent("KeyDown", nil, ev)
	end
end

function BlackNinja:OnKeyUp(event)
	local sym = tonumber(event.Sym)
	local char = event.Char
	local inc = self.v_c[sym]
	if inc then
		self.v = self.v - inc
		--local vLen = math.sqrt( self.v.x * self.v.x + self.v.y * self.v.y )
		--self.vn = Vector2( self.v.x / vLen, self.v.y / vLen )		
	elseif sym == SDLK._z then
		self.shooting = false
	end
end

function BlackNinja:OnEveryFrame()
	local x, y = self.pos:GetPosition()
	--[[
	if self.vn.x < 0.1 then
		self.vn.x = self.vn.x * 0.99
	end
	if self.vn.y < 0.1 then
		self.vn.y = self.vn.y * 0.99
	end	
	--]]
	self.pos:SetPosition(Vector2(x, y) + self.v * 100 * GetDeltaTime())
	
	if self.shooting then
		local projectile = Create('ProjectileProto', '')
		self.parent:Attach(projectile)
		projectile.pos:SetPosition(self.pos:GetPosition())
		--self.pos:Attach(projectile)
		local mc = GetObject('Mouse cursor')
		local mcx, mcy = mc.pos:GetPosition()
		local x, y = self.pos:GetPosition()
		projectile.v = -Vector2(x- mcx, y - mcy)	
	end
	
end

--------------------------------------------------------------------------------

Projectile = GameObject:Derive()

function Projectile:OnCreate()
	self.age = 0.5
	self.v = Vector2(200, 200)
	self:Subscribe('EveryFrame')
end

function Projectile:OnEveryFrame()
	local x, y = self.pos:GetPosition()
	self.pos:SetPosition(Vector2(x, y) + self.v * GetDeltaTime())
	self.age = self.age - GetDeltaTime()
	if self.age <= 0 then
		self:DestroyFull()
	end
end

--------------------------------------------------------------------------------

--[[
TempEditScript = TempEditScript or { }

function TempEditScript:OnCreate()
	SubscribeToEvent("KeyDown", self.object)
	SubscribeToEvent("Attached", self.object)
end

function TempEditScript:OnAttached()
	self.edit = GetParent(self.object)
end

function TempEditScript:OnKeyDown(event)
	local sym = event.Sym
	local char = event.Char
	local old_text = GetText(self.edit)

	if sym == "8" then
		SetText(self.edit, string.sub(GetText(self.edit), 1, -2))
	elseif sym == "13" or sym == "10" then
		return
	else
		SetText(GetParent(self.object), GetText(GetParent(self.object)) .. char)
	end
end
--]]
