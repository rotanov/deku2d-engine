-- эти функции будут вызваны из программы
function outerFunction1()
  print('outerFunction1 called!')
end

function outerFunction2()
  print('outerFunction2 called!')

  return 54321
end

function outerFunction3()
  print('outerFunction3 called!')

  return 'Hello from button1.lua!'
end

-- вызываем внутренние функции программы
print('innerFunction1() returns ' .. (innerFunction1() or 'nil'))
print('innerFunction2() returns ' .. (innerFunction2() or 'nil'))
print('innerFunction3() returns ' .. (innerFunction3() or 'nil'))