A = {}
A.val = 1

function A:new(o)
    o = o or {}
    for k,v in pairs(self) do
        o[k] = v
    end
    return o
end

a1 = A:new()
a2 = A:new()
print(a1.val)
print(a2.val)
a1.val = 2
a2.val = 3
print(a1.val)
print(a2.val)