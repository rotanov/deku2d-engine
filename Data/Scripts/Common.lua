function dostring( source )
  return loadstring( source )()
  -- assert?
end

function LogHelper(...)
  local t = {...}
  local tempString = ''
  for i = 1,#t do 
    tempString = tempString .. tostring(t[i]) .. "\t"
  end
  Log( "LUA OUT", tempString)
end

print = LogHelper

function DumpGlobals()
	for k, v in pairs(_G) do
		print(k, v)
	end
end

function DumpA()
	for k, v in pairs(A) do
		print(k, v)
	end
end

-- 2do: add memorization pattern in order to remember produced results
-- The thing below is very stupid, so it doesn't any lexical analisys
-- just simple plaint text iterative gsub
Preprocessor = Preprocessor or 
{
  [ 'Set' ] = function ( self, name, text, ... )
    local info =
    {
      [ 'text' ] = text
    }
    local args =  { ... }
    info.templates = {}
    for i = 1, #args do
      info.templates[ i ] = args[ i ]
    end
    
    self[ name ] = function ( self, ... )
      return self:Execute( name, ... )
    end
    
    self[ '__' .. name ] = info
  end,
  
  [ 'Get' ] = function ( self, name, ... )
    local info = self[ '__' .. name ]
    if not info then
      error( 'Specified function not found in preprocessor' )
    end
    local args = { ... }
    if #args ~= #info.templates then
      error( 'Supported arguments count doesn\'t correspond template specification' )
    end
    local substRes = info.text
    for i = 1, #args do
      substRes = substRes:gsub( info.templates[ i ], args[ i ] )
    end
    return loadstring( substRes )
  end,
  
  [ 'Execute' ] = function ( self, name, ... )
    return self.Get( self, name, ... )()
  end,
}

FSA = FSA or
{
  [ 'Execute' ] = function( self, ... )
    if not self.state then
      error( 'FSA: state unspecified' )
    elseif not self[ self.state ] then
      error( string.format( 'State \'%s\' not found in FSA', self.state ) )
    end
    self[ self.state ]( ... )
  end,
  [ 'state' ] = nil,
}