DraggableComponent = GameObject:Derive()

function DraggableComponent:OnCreate()
	self:Subscribe("MouseDown")
	self:Subscribe("MouseUp")
	self:Subscribe("MouseMove")
	self.Drag = false
end

function DraggableComponent:OnMouseDown(e)
	if not self.parent then
		return
	end

	--Dump(self.parent)
	--print(self.parent:GetName())

	local box = self.parent:GetBox()

	local x = tonumber(GetEventData(e, "X"))
	local y = tonumber(GetEventData(e, "Y"))

	if x > box.Min.x and x < box.Max.x and y > box.Min.y and y < box.Max.y then
		self.Drag = true
	end
end

function DraggableComponent:OnMouseUp()
	self.Drag = false
end

function DraggableComponent:OnMouseMove(e)
	if not self.parent or not self.Drag then
		return
	end

	local x = tonumber(GetEventData(e, "X"))
	local y = tonumber(GetEventData(e, "Y"))

	self.parent:SetPosition(x, y);
end
