require("logging")

local function PassMessagesToHook()
    local messages = logging.GetAll()
    if not messages then
        return
    end

    for i = 1, #messages do
        local msg = messages[i]
        hook.Run("LogMessage", msg.Timestamp, msg.ChannelID, msg.Severity, msg.Flags, Color(msg.Color.r, msg.Color.g, msg.Color.b, msg.Color.a), msg.Message)
    end
end

function logging.EnableHook()
    hook.Add("Think", "logging.PassMessagesToHook", PassMessagesToHook)
end

function logging.DisableHook()
    hook.Remove("Think", "logging.PassMessagesToHook")
end
