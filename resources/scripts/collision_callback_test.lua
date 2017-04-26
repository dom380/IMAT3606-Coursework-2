collision_callback_test = {}
collision_callback_test.msgCount = 0
collision_callback_test.dt = 0
collision_callback_test.ran = false

-- process message function
collision_callback_test.RecieveMessage = function (logicComponent, msg, msgType)
	if msgType == "COLLISION" then
		collision_callback_test.dt = collision_callback_test.dt + msg.dt
		--print("delta time: " .. collision_callback_test.dt)
		if collision_callback_test.ran == false then
			collision_callback_test.msgCount = collision_callback_test.msgCount+1
			print(collision_callback_test.msgCount)
			collision_callback_test.ran = true
		elseif collision_callback_test.dt > 0.25 then
			collision_callback_test.ran = false
			collision_callback_test.dt = 0
		end
	end
end