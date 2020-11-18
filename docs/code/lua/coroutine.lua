co = coroutine.create( function()
    for i = 1, 3 do 
        coroutine.yield(i)
    end
end
)


print(coroutine.status(co)) --协程的状态挂起
for i = 1,5 do
    local flag,res = coroutine.resume(co)
    if flag then
        print(res) --i=4时执行到最后，没有res
    else
        --在一个已死亡的协程上调用resume会返回false，并返回一个提示字符串
        print(coroutine.status(co))
        print(res) 
    end
end




function producer()
    while true do 
        local x = io.read()
        send(x)
    end
end

pco = coroutine.create( producer )

function consumer()
    while true do 
        local x = receive()
        io.write(x, "\n")
    end
end

function receive()
    local status, value = coroutine.resume(pco)
    return value
end

function send(x)
    coroutine.yield( x )
end

consumer()