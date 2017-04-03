ui_button_callbacks = {};

ui_button_callbacks.switchScreen = function(engineObj, params)
	engineObj:switchScreen(params["screenid"]);
end

ui_button_callbacks.replaceScreen = function(engineObj, params)
	engineObj:replaceScreen(params["screenid"]);
end

ui_button_callbacks.exit = function(engineObj)
	engineObj:exit();
end

ui_button_callbacks.doNothing = function()
end

