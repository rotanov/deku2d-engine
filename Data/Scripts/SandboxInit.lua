--[[
for i = 0, 3 do
	local a = Create("A", "")
	a.pos:SetPosition(0, 0)
	local nextA = a
	while nextA do
		nextA.pos:SetAngle( 90 * i )
		nextA = nextA.A
	end	
	GetObject("Mouse cursor").pos:Attach(a)
end
--]]

local ninja = Create("BlackNinja", "")
ninja.pos:SetPosition(320, 240)
Attach(GetObject("RootGameObject"), ninja.object)

--Attach(GetObject("RootGameObject"), Create("SandboxProto", "SandboxInst").object)
--Attach(GetParent(GetObject("Mouse cursor")), GetObject("Magic square"))

