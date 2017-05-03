death_trigger = {}

death_trigger.trigger = function(gameObject)
	local logic = gameObject:getLogic()
	if logic then
		logic:resetTransform()
	else 
		print("Logic null")
	end
end --function