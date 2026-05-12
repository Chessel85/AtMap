//soundType.h
#pragma once

enum class SoundType
{
    standard,
    city,
    capitalCity,
    town,
    village,
    hamlet,
    railwayStation,
    latitudeLine,
    longitudeLine
};

// Enum to categorize our reusable "Virtual Speakers"
enum class SourceCategory
{
    spatial,    // 3D/HRTF for map objects
    stereo,  // Panned/Stereo for lines and UI
    stream //long files played centre pan 
};