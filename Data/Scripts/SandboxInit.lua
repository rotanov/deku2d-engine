
for i = 0, 3 do
	local a = Create("A", "")
	GetObjectTable(a.object).pos:SetPosition(0, 0)
	--a.pos:SetPosition(0, 0)
	local nextA = a
	while nextA do
		nextA.pos:SetAngle( 90 * i )
		nextA = nextA.A
	end	
	Attach(GetParent(GetObject("Mouse cursor")), a.object)
end

Dump(_G)

---[[
--Attach(GetObject("RootGameObject"), Create("SandboxProto", "SandboxInst").object)
--Attach(GetParent(GetObject("Mouse cursor")), GetObject("Magic square"))
--]]
--Attach(GetParent(GetObject("Mouse cursor")), Create("Poly", ""))
--SetColor(GetObject("TestText"), 1, 0, 0, 1)
--DebugPrintComponentTree()

--[[
	Надо чтобы всё говно возвращало таблицы, а не сраные лайтюзердата.
	Лайтюзердата у нас будет в {}.object
	
]]