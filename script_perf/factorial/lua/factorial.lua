#!/usr/bin/lua

function recur_fact(n)
	if n == 0 then
		return 1
	else 
		return n*recur_fact(n-1)
	end
end

function factorial(n)
	if n == 0 then
		return 1
	else
		ret = 1
		for i = 1, n do
			ret = ret * i
		end
		return ret
	end
end

if not arg[1] then
	print("Please specify the number which need to calculat its factorial")
	return
end

loops = tonumber(arg[2])
n = tonumber(arg[1])

if not loops then 
	loops = 1
end


for i = 1, loops do
	ret = factorial(n)
end




