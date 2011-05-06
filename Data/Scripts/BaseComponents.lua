RenderableComponent = RenderableComponent or
{
	[ 'SetColor' ] = function(self, r, g, b, a)
		SetColor( self.object, r, g, b, a)
	end,
	[ 'OnCreate' ] = function(self)
	end,
}

PlaceableComponent = PlaceableComponent or
{
	[ 'OnCreate' ] = function(self)
	end,
	
	[ 'SetPosition' ] = function(self, x, y)
		SetPosition( self.object, x, y )
	end,
}

GeometricComponent = GeometricComponent or
{
}
	
GameObject = GameObject or
{
}

Text = Text or
{
}

TimerComponent = TimerComponent or
{
}
