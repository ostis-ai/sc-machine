/* ----------------------------------------------------------------------------
** © 201x Austin Brunkhorst, All Rights Reserved.
**
** ReflectionOptions.h
** --------------------------------------------------------------------------*/

#pragma once

struct ReflectionOptions
{
    std::string targetName;
    std::string inputPath;
    std::string outputPath;
    std::string buildDirectory;
	std::string generatorPath;

    std::vector<std::string> arguments;
};