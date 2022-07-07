#include <iostream>

#include "utils.h"
#include "serial.h"
#include "gps.h"

using namespace std;

int main()
{
    Gps gps("gps1");
    serial ser("/dev/ttyACM1");

    ser.open_port();

    string line;
    while (true)
    {
        line = ser.read_line();
        if (line == "")
            continue;

        int code = parse_gps(&gps, get_timestamp(), line);
        if (code != ParseError::ParseOk)
        {
            cout << ParseErrorString[code] << "\t" << line << endl;
        }
    }

    return 0;
}