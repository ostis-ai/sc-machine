#ifndef _builder_h_
#define _builder_h_

#include "types.h"

class Builder
{
public:
    explicit Builder();
    virtual ~Builder();


    /*! Run building process
      * @param inputPath Path to source directory
      * @param outputPath Path to output directory
      * @param clearOutput Flag to clear output directory before build
      * @returns If build finished, then returns ture; otherwise returns false
      */
    bool run(const String &inputPath, const String &outputPath, bool clearOutput);

protected:
    //! Process string data
    bool processString(const String &data);

    //! Process specified file
    bool processFile(const String &filename);

    //! Collecting files for process
    void collectFiles();

private:
    //! Input directory path
    String mInputPath;
    //! Output directory path
    String mOutputPath;
    //! Flag to clear output
    bool mClearOutput;

    //! Set of files to process
    typedef std::set< String > tFileSet;
    tFileSet mFileSet;
};

#endif
