#pragma once

#include <Flux/Core/Memory/UniquePointer.h>
#include <Flux/Core/Tools/Console/CommandTree.h>

#include <thread>
#include <iomanip>
#include <fstream>

namespace Flux {

    class ConsoleColor {

    public:
        
        static const ConsoleColor black;
        static const ConsoleColor red;
        static const ConsoleColor green;
        static const ConsoleColor yellow;
        static const ConsoleColor blue;
        static const ConsoleColor magenta;
        static const ConsoleColor cyan;
        static const ConsoleColor lightGray;
        static const ConsoleColor darkGray;
        static const ConsoleColor lightRed;
        static const ConsoleColor lightGreen;
        static const ConsoleColor lightYellow;
        static const ConsoleColor lightBlue;
        static const ConsoleColor lightMagenta;
        static const ConsoleColor lightCyan;
        static const ConsoleColor reset;

        NODISCARD const char* get() const { return color; }

        ConsoleColor() = delete;

    private:
        
        ConsoleColor(const char* c) : color(c) {}

        const char* color;

    };

    class Console final {

    public:

        Console();

        ~Console();

        FORCEINLINE static void exec(String const& command) { instance->execute(command); }

        FORCEINLINE static void registerCommand(SharedPointer<CommandNode> const& node) {
            instance->registerNode(node);
        }

        FORCEINLINE static Console* getInstance() { return instance; }

        /*!
         *  Logs a debug message.
         *
         *  \param[in] format The format string
         *  \param[in] args The variable arguments for the format string
         */
        template <typename ... Args>
        static void logDebug(String const& format, Args&&... args) {
            getInstance()->log(ConsoleColor::darkGray, "DEBUG", format, args...);
        }
        
        /*!
         *  Logs a runtime info message.
         *
         *  \param[in] format The format string
         *  \param[in] args The variable arguments for the format string
         */
        template <typename ... Args>
        static void logRuntime(String const& format, Args&&... args) {
            getInstance()->log(ConsoleColor::lightMagenta, "RUNTIME", format, args...);
        }
        
        /*!
         *  Logs a status info message.
         *
         *  \param[in] format The format string
         *  \param[in] args The variable arguments for the format string
         */
        template <typename ... Args>
        static void logStatus(String const& format, Args&&... args) {
            getInstance()->log(ConsoleColor::lightCyan, "STATUS", format, args...);
        }

        /*!
         *  Logs an info message.
         *
         *  \param[in] format The format string
         *  \param[in] args The variable arguments for the format string
         */
        template <typename ... Args>
        static void logInfo(String const& format, Args&&... args) {
            getInstance()->log(ConsoleColor::lightGray, "INFO", format, args...);
        }

        /*!
         *  Logs a success message.
         *
         *  \param[in] format The format string
         *  \param[in] args The variable arguments for the format string
         */
        template <typename ... Args>
        static void logSuccess(String const& format, Args&&... args) {
            getInstance()->log(ConsoleColor::lightGreen, "SUCCESS", format, args...);
        }

        /*!
         *  Logs a warning message.
         *
         *  \param[in] format The format string
         *  \param[in] args The variable arguments for the format string
         */
        template <typename ... Args>
        static void logWarning(String const& format, Args&&... args) {
            getInstance()->log(ConsoleColor::lightYellow, "WARNING", format, args...);
        }

        /*!
         *  Logs an error message.
         *
         *  \param[in] format The format string
         *  \param[in] args The variable arguments for the format string
         */
        template <typename ... Args>
        static void logError(String const& format, Args&&... args) {
            getInstance()->log(ConsoleColor::lightRed, "ERROR", format, args...);
        }

        /*!
         *  Logs a fatal error message and stops the program.
         *
         *  \param[in] format The format string
         *  \param[in] args The variable arguments for the format string
         */
        template <typename ... Args>
        static void logFatal(String const& format, Args&&... args) {
            getInstance()->log(ConsoleColor::red, "FATAL", format, args...);
            std::cout.flush();
        }

        void init();

    private:
        
        /*!
         * Logs a message to the console.
         *
         *  \param[in] color The text color
         *  \param[in] type The log category to display
         *  \param[in] format The format string
         *  \param[in] args The variable arguments for the format string
         */
        template <typename ... Args>
        void log(ConsoleColor const& color, const char* type, String const& format, Args&&...args) const {

            String formatted = String::format(format, args...);
            
            // Print the formatted string.
            std::cout << String::format("\r{}[{}] | {}{}\n", color.get(), type, formatted, ConsoleColor::reset.get());

            // Get the time and put it into a string stream (cannot convert _Put_time directly to string).
            const auto now = std::chrono::system_clock::now();
            const std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::stringstream dateStream;
            std::tm tm{};

#ifdef _WIN64
            localtime_s(&tm, &t);
#else
            tm = *std::localtime(&t);
#endif
        
            dateStream << std::put_time(&tm, "%c");

            // Write to the log file.
            file << String::format("[{}] [{}] | {}\n", dateStream.str(), type, formatted);

            // Flush the data to the file.
            file.flush();

       }
        
        void consoleLoop() const;

        void consoleHelp(const CommandContext* context) const;

        void registerNode(SharedPointer<CommandNode> const& node) const;

        void execute(String const& command) const;

        bool runPrompt = false;
        
        std::thread consoleThread;

        UniquePointer<CommandTree> commandTree;

        inline static std::ofstream file = std::ofstream("latest.log");
        inline static Console* instance = nullptr;

        
    };

}
