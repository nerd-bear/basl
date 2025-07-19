#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <thread>

#include "cursor.hpp"

enum spinnerType
{
    brail,
    dot
};

namespace spinner
{
    class Spinner
    {
    private:
        spinnerType type;
        bool active;
        std::string text;
        int timeSep;

    public:
        Spinner(spinnerType type); // Declaration only
        void setText(std::string text);
        void setTimeSep(int timeSep);
        void start();
        void end();
    };
}