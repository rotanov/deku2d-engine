GameObject = Class:Derive()

GameObject_mt = getmetatable(GameObject)

GameObject_mt.__index = function(t, k)
	local result

	if k == "parent" then
		result = GetParent(t.object)
	elseif k == "proto" then
		result = GetPrototype(t.object)
	elseif k == "protoParent" then
		result = GetParentProtype(t.object)
	else
		result = GetObjectByLocalName(t.object, k)
	end

	return GetObjectTable(result)
end

setmetatable(GameObject, GameObject_mt)

function GameObject:OnCreate()
end

function GameObject:Attach(other)
	if type(other) == "table" then
		Attach(self.object, other.object)
	elseif type(other) == "userdata" then
		Attach(self.object, other)
	end
end

function GameObject:Detach(other)
	if type(other) == "table" then
		Detach(self.object, other.object)
	elseif type(other) == "userdata" then
		Detach(self.object, other)
	end
end

function GameObject:Destroy()
	Destroy(self.object)
end

function GameObject:DestroyFull()
	DestroySubtree(self.object)
end

function GameObject:Clone()
	return GetObjectTable(Clone(self.object))
end

function GameObject:CloneTree()
	return GetObjectTable(CloneTree(self.object))
end

function GameObject:GetName()
	return GetName(self.object)
end

function GameObject:Subscribe(event)
	SubscribeToEvent(self.object, event)
end

function GameObject:GetParent()
	return GetObjectTable(GetParent(self.object))
end

function GameObject:Unsubscribe(eventName)
	UnsubscribeFromEvent(self.object, eventName)
end

function GameObject:TriggerEvent(eventName)
	TriggerEvent(eventName, self.object)
end

function GameObject:SetScript(script)
	SetScript(self.object, script)
end

function GameObject:SetClassName(className)
	SetClassName(self.object, className)
end

function GameObject:FindFirstOfClass(className, exceedPrototype)
	return GetObjectTable(FindFirstOfClass(self.object, className, exceedPrototype))
end

--------------------------------------------------------------------------------

RenderableComponent = GameObject:Derive()

function RenderableComponent:SetColor(r, g, b, a)
	SetColor(self.object, r, g, b, a)
end

--------------------------------------------------------------------------------

PlaceableComponent = GameObject:Derive()

function PlaceableComponent:OnCreate()
end

function PlaceableComponent:SetPosition(x, y)
	if type(x) == "table" then
		SetPosition(self.object, x.x, x.y)
	else
		SetPosition(self.object, x, y)
	end
end

function PlaceableComponent:GetPosition()
	return GetPosition(self.object)
end

function PlaceableComponent:SetAngle(angle)
	SetAngle(self.object, angle)
end

function PlaceableComponent:GetAngle()
	return GetAngle(self.object)
end

function PlaceableComponent:GetBox()
	return GetBox(self.object)
end

--------------------------------------------------------------------------------

GeometricComponent = GameObject:Derive()

--------------------------------------------------------------------------------
	
Text = RenderableComponent:Derive()

function Text:SetText(text)
	SetText(self.object, text)
end

function Text:GetText()
	return GetText(self.object)
end

function Text:SetTextFont(font)
	if type(font) == "userdata" then
		SetTextFont(self.object, font)
	elseif type(font) == "string" and GetObject(font) then
		SetTextFont(self.object, GetObject(font))
	end
end

--------------------------------------------------------------------------------

TimerComponent = GameObject:Derive()

function TimerComponent:SetInterval(interval)
	SetInterval(self.object, interval)
end

function TimerComponent:SetEnabled(enabled)
	SetTimerEnabled(self.object, enabled)
end

--------------------------------------------------------------------------------
