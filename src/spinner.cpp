#ifdef _WIN32
#include <Windows.h>

struct WinUTF8Setup
{
    WinUTF8Setup() { SetConsoleOutputCP(CP_UTF8); }
} _winUTF8Setup; // Runs before `main()`
#endif

#include "spinner.hpp"
#include <iostream>

const std::vector<std::string> brailSpinner = {"⣷", "⣯", "⣟", "⡿", "⢿", "⣻", "⣽", "⣾"};
const std::vector<std::string> dotSpinner = {"⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁"};

namespace spinner
{
    Spinner::Spinner(spinnerType type)
    {
        this->type = type;
    }

    void Spinner::setText(std::string text)
    {
        this->text = text;
    }

    void Spinner::setTimeSep(int timeSep)
    {
        this->timeSep = timeSep;
    }

    void Spinner::start()
    {
        hideCursor();

        this->active = true;
        int index = 0;
        std::vector<std::string> currentType = (type == brail) ? brailSpinner : dotSpinner;

        while (active)
        {
            if (index == currentType.size())
            {
                index = 0;
            }
            std::cout << currentType[index] << " " << this->text << "\r" << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(timeSep));
            index++;
        }
    }

    void Spinner::end()
    {
        this->active = false;

        showCursor();
    }
}