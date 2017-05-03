end_screen_trigger = {};

end_screen_trigger.trigger = function(gameObject, engineObj)
	local tag = gameObject:getTag()
	if tag == "player" then
		engineObj:replaceScreen("EndScreen");
	end
end
