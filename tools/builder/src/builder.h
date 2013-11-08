#ifndef _builder_h_
#define _builder_h_

#include "types.h"

#include "parser/scsLexer.h"
#include "parser/scsParser.h"

extern "C"
{
#include "sc_memory.h"
#include "sc_helper.h"
}

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


    /*! Create sc-link with specified data string.
     * String will be encoded into utf-8.
     * @param text Text value to setup as content
     */
    sc_addr createLink(const std::string &str);

    //! Dump antl tree to console
    void dumpTree(pANTLR3_BASE_TREE tree, int level);

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
