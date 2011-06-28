--[[for i = 0, 3 do
	local a = Create("A", "")
	a.pos:SetPosition(0, 0)
	local nextA = a
	while nextA do
		nextA.pos:SetAngle( 90 * i )
		nextA = nextA.A
	end	
	GetObject("Mouse cursor").pos:Attach(a)
end]]

local lpc = Create("LuaPerfCounter", "")
Attach(GetObject("RootGameObject"), lpc.object)

--[[local ninja = Create("BlackNinja", "")
ninja.pos:SetPosition(320, 240)
Attach(GetObject("RootGameObject"), ninja.object)]]

--[[for i = 0, 10 do
	local ninja = Create("BlackNinja", "")
	ninja.pos:SetPosition(i * 50, 240)
	Attach(GetObject("RootGameObject"), ninja.object)
end

for i = 0, 10 do
	local ninja = Create("BlackNinja", "")
	ninja.pos:SetPosition(i * 50, 100)
	Attach(GetObject("RootGameObject"), ninja.object)
end

for i = 0, 10 do
	local ninja = Create("BlackNinja", "")
	ninja.pos:SetPosition(i * 50, 380)
	Attach(GetObject("RootGameObject"), ninja.object)
end]]

--local another_ninja = Create("BlackNinja", "")
--another_ninja.pos:SetPosition(350, 100)
--Attach(GetObject("RootGameObject"), another_ninja.object)

--Attach(GetObject("RootGameObject"), Create("SandboxProto", "SandboxInst").object)
--Attach(GetParent(GetObject("Mouse cursor")), GetObject("Magic square"))

Attach(GetObject("RootGameObject"), Create("TestBox", "TestBoxInst").object)

DebugPrintComponentTree()
