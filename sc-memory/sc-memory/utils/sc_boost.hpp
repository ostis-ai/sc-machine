#pragma once

#include <boost/filesystem.hpp>

namespace boost
{
namespace filesystem
{

static boost::filesystem::path relativePath(boost::filesystem::path parent, boost::filesystem::path to)
{
   // Start at the root path and while they are the same then do nothing then when they first
   // diverge take the remainder of the two path and replace the entire parent path with ".."
   // segments.
   boost::filesystem::path::const_iterator fromIter = parent.begin();
   boost::filesystem::path::const_iterator toIter = to.begin();

   // Loop through both
   while (fromIter != parent.end() && toIter != to.end() && (*toIter) == (*fromIter))
   {
      ++toIter;
      ++fromIter;
   }

   boost::filesystem::path finalPath;
   while (fromIter != parent.end())
   {
      finalPath /= "..";
      ++fromIter;
   }

   while (toIter != to.end())
   {
      finalPath /= *toIter;
      ++toIter;
   }

   return finalPath;
}

} // filesystem
} // namespace boost