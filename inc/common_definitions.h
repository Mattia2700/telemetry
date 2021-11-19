#pragma once

#include <stdio.h>
#include <vector>
#include <string>


struct run_config
{
  int circuit;
  int pilot;
  int race;
  std::string url;
};
static std::vector<std::string> CIRCUITS = std::vector<std::string>({
    "default",
    "Vadena",
    "Varano",
    "Povo",
    "Skio",
});
static std::vector<std::string> PILOTS = std::vector<std::string>({
    "default",
    "Mirco",
    "Giacomo",
    "Filippo",
});
static std::vector<std::string> RACES = std::vector<std::string>({
    "default",
    "TrackDrive",
    "Acceleration",
    "Endurance",
    "Skidpad",
    "SineSteer",
    "RampSteer",
});
