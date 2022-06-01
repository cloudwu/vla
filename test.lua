local test = require "vlatest"

local t = test.testlua()
for k,v in pairs(t) do
	print(k,v)
end

test.testheap()


local function foo(ud)
	print (ud)
	for i = 1, 100 do
		ud(i)
	end
	for i = 1, #ud do
		print(ud[i])
	end
	print "OK"
end

local ud = test.testud()

foo(ud)

local ud = test.testud(true)	-- C VLA

foo(ud)