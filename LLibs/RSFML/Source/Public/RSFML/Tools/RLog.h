#pragma once

#include <string_view>
#include <format>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#define PREPROCESSOR_TO_STRING(x) PREPROCESSOR_TO_STRING_INNER(x)
#define PREPROCESSOR_TO_STRING_INNER(x) #x
#define L_PATH __FILE__ "(" PREPROCESSOR_TO_STRING(__LINE__) "):\n"

class RLog
{
public:
	enum class ESeverity : uint8_t
	{
		Display = 0,
		Warning = 1,
		Error = 2,
		Exception = 3
	};
public:
	RLog(const RLog&) = delete;
	RLog& operator=(const RLog&) = delete;

private:
	RLog() = default;

public:
	inline static void LogDisplay(std::string_view InDisplayText) { Log(RLog::ESeverity::Display, InDisplayText); };
	inline static void LogWarning(std::string_view InWarningText) { Log(RLog::ESeverity::Warning, InWarningText); };
	inline static void LogError(std::string_view InErrorText) { Log(RLog::ESeverity::Error, InErrorText); };
	inline static void LogException(std::string_view InExceptionText) { Log(RLog::ESeverity::Exception, InExceptionText); };

public:
	static void Log(ESeverity EInSeverity, const std::string_view InDisplayError);

public:
	template<typename... Args >
	void static Log(ESeverity EInSeverity, std::format_string<Args...> InConstexpFormatText, Args&&... InArgs)
	{
		RLog::Log(EInSeverity, std::format(InConstexpFormatText, std::forward<Args>(InArgs)...));
	};

};
constexpr RLog::ESeverity LOG_DISPLAY = RLog::ESeverity::Display;
constexpr RLog::ESeverity LOG_WARNING = RLog::ESeverity::Warning;
constexpr RLog::ESeverity LOG_ERROR = RLog::ESeverity::Error;
constexpr RLog::ESeverity LOG_EXCEPTION = RLog::ESeverity::Exception;
const std::string& ToString(RLog::ESeverity EInSeverity);

namespace RSfml
{
	template <typename T>
	std::string _tostring(const sf::Vector2<T> InVector)
	{
		return std::format("[ x : {}, y : {}]", InVector.x, InVector.y);
	}
	template <typename T>
	std::string _tostring(const sf::Rect<T> InRect)
	{
		return std::format("[ topLeft : {}, size : {}]", _tostring(InRect.position), _tostring(InRect.size));
	}
}