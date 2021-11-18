#pragma once
/*
* Cristiano Strobbe - Feb 2020
* Cool console debug messages
*
* Changelog:
* v1.0 - Simple header files for cool terminal messages.
* v1.1 - Added wait message and clear window functionality.
* v2.0 - No more single header file. Added message system time.
         Fix class style.
*/

#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <ctime>
#include <math.h>
#include <sys/time.h> // gettimeofday

#include "terminal_commands.h"

using namespace std;

namespace Debug
{

  class Console
  {
    public:

      /**
       * @brief Construct a new Console object
       *
       */
      Console();

      /**
      * @brief Construct a new Console:: Console object
      *
      * @param head_color
      * @param head_type
      * @param txt_color
      * @param txt_type
      */
      Console(const int head_color,
              const int head_type,
              const int txt_color,
              const int txt_type);

      /**
      * @brief Destroy the Console:: Console object
      *
      */
      ~Console();

      /**
      * @brief Close log file. Same functionality as class destructor.
      *
      */
      void Close();

      // TODO: Put carriage return at the begin of the line
      void SetDefault(const int head_color, const int head_type, const int txt_color, const int txt_type);

      void SaveAllMessages(string path="");

      // // TODO(@cstrobbe):
      // void InitLogFile(const string path);

      // TODO(@cstrobbe):
      // void close_log(){}


      void DebugMessage(const string header,
                        const int color,
                        const int mode,
                        const string message,
                        const int msg_color,
                        const int msg_mode){}

      /**
       * @brief
       *
       * @param header
       * @param head_color
       * @param text
       * @param txt_color
       */
      void Message(const string header,
                        const int head_color,
                        const string text,
                        const int txt_color);

      /**
       * @brief
       *
       * @param header
       * @param text
       */
      void DebugMessage(const string header,
                        const string text);

      // FIXME - CHECK.
      void DebugMessage(const string text);


      void ErrorMessage(const string header, const string text);

      void ErrorMessage(const string text);



      void WaitMessage(const string header, const string text);

      void WaitMessage(const string text);



      void MessageWithHeader(const string header, const string text, bool reprint=true);

      void StatMessage(const string text);

      void WarnMessage(const string text);

      /**
       * @brief Ring system bell.
       *
       */
      void BellNotification();

      void Test();

      /**
       * @brief Get the Datetime As String object
       *
       * @return string
       */
      string GetDatetimeAsString();

      /**
       * @brief Get the System Time object
       *
       * @return string
       */
      string GetSystemTime();

      // void LogToFile(const string text, bool print_message=false);

    private:

        int header_color = YELLOW;
        int header_type  = BOLD;
        int text_color   = DEFAULT;
        int text_type    = NORMAL;
        bool save_all_console_messages { false };
        time_t start;
        bool is_first_time { true };

        std::ofstream console_messages; // Store all console messages

        /**
         * @brief
         *
         * @param text_mode
         * @param terminal_mode
         * @param terminal_color
         * @param text
         * @return string
         */
        string MakeTextColor(const int text_mode,
                             const int terminal_mode,
                             const int terminal_color,
                             const string text);

        /**
         * @brief
         *
         * @param text_mode
         * @param terminal_mode
         * @param terminal_color
         * @param text
         * @param background_color
         * @return string
         */
        string MakeTextColor(const int text_mode,
                             const int terminal_mode,
                             const int terminal_color,
                             const string text,
                             const int background_color);

        string MakeHeader(string header);

        string GetBgColorCode(int color = DEFAULT);

        void SetBackgorundColor(int color = DEFAULT);

        string MakeNonPrintable(const string printable);

        // TO BE FIXED
        void ClearCurrentLine();

        /**
         * @brief
         *
         * @return double
         */
        double get_dt();

        /**
         * @brief
         *
         * @param lines
         */
        void MoveCursorUp(const int lines);

        /**
         * @brief
         *
         */
        void ClearWindow();

        /**
         * @brief
         *
         * @param lines
         */
        void ClearTerminal(const int lines);

  };

}; // namespace debug
