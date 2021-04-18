# gm\_logging

[![Build Status](https://metamann.visualstudio.com/GitHub%20danielga/_apis/build/status/danielga.gm_logging?branchName=master)](https://metamann.visualstudio.com/GitHub%20danielga/_build/latest?definitionId=19&branchName=master)

⚠️ Currently only works on the `x86-64` branch of Garry's Mod!

A Garry's Mod module that copies the log messages and passes them to Lua. Also able to manipulate logging channels.

## API reference

When return types or parameters are enclosed in square brackets, they are nillable returns or optional parameters and will contain the default value, in the case of parameters.

### Types

```lua
Color = { -- Table with the RGBA elements of a color.
    r = number, -- Amount of red.
    g = number, -- Amount of green.
    b = number, -- Amount of blue.
    a = number -- Amount of alpha.
}
```

```lua
Message = { -- Contains a log message and all its metadata.
    Timestamp = number, -- Timestamp of the message.
    Severity = number, -- Represents the severity level of the message.
    ChannelID = number, -- Channel ID of the channel where this message was logged into.
    Color = Color, -- Message color.
    Flags = number, -- Flags that this message has.
    Message = string -- Contains the log message.
}
```

```lua
Messages = {Message} -- 'Messages' is an array of 'Message'
```

```lua
LoggingChannel = { -- Represents a logging channel.
    GetTable = function(), -- Returns a table associated with this object, where users can place data.
    GetChannelID = function(), -- Returns a number, representing the ID of this channel.
    GetName = function(), -- Returns a string, representing the name of this channel.
    GetTags = function(), -- Returns a table of strings, with all the tags associated to this channel.
    IsSeverityEnabled = function(severity), -- Returns a boolean, where a value of 'true' means that this channel will log messages with that 'severity' (number), 'false' otherwise.
    GetMinimumSeverity = function(), -- Returns a number, representing the minimum severity of log messages that this channel accepts.
    SetMinimumSeverity = function(severity), -- Sets the minimum logging 'severity' (number) that this channel should accept.
    GetColor = function(), -- Returns a Color, with the RGBA elements of the default log color.
    SetColor = function(color), -- Sets the default message 'color' (Color).
    GetFlags = function(), -- Returns a number, indicating what flags this channel has.
    SetFlags = function(flags), -- Sets the channel 'flags' (number).
}
```

```lua
LoggingChannels = {LoggingChannel} -- 'LoggingChannels' is an array of 'LoggingChannel'
```

### Constants

```lua
-- Holds the 'logging' module version in a string form.
-- Example: "logging 1.0.0"
string logging.Version
```

```lua
-- Holds the 'logging' module version in a numeric form, LuaJIT style.
-- Example: 10000
number logging.VersionNum
```

```lua
-- Type enum for simple messages.
number logging.SEVERITY_MESSAGE
```

```lua
-- Type enum for warning messages.
number logging.SEVERITY_WARNING
```

```lua
-- Type enum for assert messages.
number logging.SEVERITY_ASSERT
```

```lua
-- Type enum for error messages.
number logging.SEVERITY_ERROR
```

```lua
-- Flags enum for console only messages.
number logging.FLAGS_CONSOLE_ONLY
```

```lua
-- Flags enum for messages that shouldn't be printed (e.g. log file output only).
number logging.FLAGS_DO_NOT_ECHO
```

### Functions

```lua
-- Get up to the specified number of messages from the queue.
-- NOTE: Avoid retrieving messages one by one in loops.
-- 'count' is a number, containing the number of messages to retrieve.
-- Returns an array of 'Message' if messages are available, otherwise returns 'nil'.
[Messages] logging.Get([count = 1])
```

```lua
-- Get all the messages in the queue.
-- Returns an array of 'Message' if messages are available, otherwise returns 'nil'.
[Messages] logging.GetAll()
```

```lua
-- Sets the maximum log queue size.
-- After this amount of logs, older messages are dequeued.
-- 'max_queue_size' is a number, representing the maximum log queue size.
logging.SetMaximumQueueSize(max_queue_size)
```

```lua
-- Returns the maximum log queue size.
-- After this amount of logs, older messages are dequeued.
number logging.GetMaximumQueueSize()
```

```lua
-- Returns the number of registered logging channels.
number logging.GetChannelCount()
```

```lua
-- Get the logging channel with the provided channel ID.
-- 'channelID' is a number, representing the channel ID, a number between 0 and (logging.GetChannelCount() - 1).
-- Returns a logging channel with the provided channel ID, or nil if it's an invalid ID.
[LoggingChannel] logging.GetChannel(channelID)
```

```lua
-- Returns an array of all the registed logging channels.
LoggingChannels logging.GetChannels()
```

```lua
-- Get the logging channel with the provided channel name.
-- 'name' is a string, representing the channel name.
-- Returns a logging channel with the provided channel name, or nil if it doesn't exist.
[LoggingChannel] logging.FindChannel(name)
```

```lua
-- Sets the minimum logging severity of the channel with the provided name.
-- 'name' is a string, representing the channel name.
-- 'severity' is a number, representing the minimum logging severity.
logging.SetChannelMinimumSeverityByName(name, severity)
```

```lua
-- Sets the minimum logging severity of the channel with the provided tag.
-- 'tag' is a string, representing the channel tag.
-- 'severity' is a number, representing the minimum logging severity.
logging.SetChannelMinimumSeverityByTag(tag, severity)
```

```lua
-- Sets the minimum logging severity of all the registered channels.
-- 'severity' is a number, representing the minimum logging severity.
logging.SetGlobalMinimumSeverity(severity)
```

These functions are only available when the Lua extension module `loggingx` is required.

```lua
-- Enables the Lua hook "LogMessage".
-- logging.Get and logging.GetAll will be unreliable when this hook is enabled, as it uses these same functions to work, causing the log queue to be consumed.
-- Uses the "Think" hook to dispatch log messages, hence it will seem to be disabled when used in a dedicated server with no players. This limitation can be bypassed by setting the "sv_hibernate_think" convar to 1.
logging.EnableHook()
```

```lua
-- Disables the Lua hook "LogMessage".
logging.DisableHook()
```

### Hooks

This hook is only available when the Lua extension module `loggingx` is required.

```lua
-- Hook called when a message is logged.
-- 'timestamp' is a number representing the time it was logged at.
-- 'channelID' is a number, representing the ID of the channel this message was logged into.
-- 'severity' is a number, equal to one of the severity enums.
-- 'flags' is a number, containing the flags of this log message.
-- 'color' is a Color, representing the color of this log message.
-- 'message' is a string, containing the log message.
-- Example: hook.Add("LogMessage", "logger", function(...) end)
LogMessage(timestamp, channelID, severity, flags, color, message)
```

## Compiling

The only supported compilation platform for this project on Windows is **Visual Studio 2017**. However, it's possible it'll work with *Visual Studio 2015* and *Visual Studio 2019* because of the unified runtime.

On Linux, everything should work fine as is.

For macOS, any **Xcode (using the GCC compiler)** version *MIGHT* work as long as the **Mac OSX 10.7 SDK** is used.

These restrictions are not random; they exist because of ABI compatibility reasons.

If stuff starts erroring or fails to work, be sure to check the correct line endings (`\n` and such) are present in the files for each OS.

## Requirements

This project requires [garrysmod\_common][1], a framework to facilitate the creation of compilations files (Visual Studio, make, XCode, etc). Simply set the environment variable `GARRYSMOD_COMMON` or the premake option `--gmcommon=path` to the path of your local copy of [garrysmod\_common][1].

We also use [SourceSDK2013][2]. The links to [SourceSDK2013][2] point to my own fork of VALVe's repo and for good reason: Garry's Mod has lots of backwards incompatible changes to interfaces and it's much smaller, being perfect for automated build systems like Azure Pipelines.

  [1]: https://github.com/danielga/garrysmod_common
  [2]: https://github.com/danielga/sourcesdk-minimal
