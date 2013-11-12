#ifndef _builder_h_
#define _builder_h_

#include "types.h"

class iTranslator;
class iTranslatorFactory;

class Builder
{
public:
    explicit Builder();
    virtual ~Builder();

    //! Initialize builder. Create and register translator factories
    void initialize();

    /*! Run building process
      * @param inputPath Path to source directory
      * @param outputPath Path to output directory
      * @param clearOutput Flag to clear output directory before build
      * @returns If build finished, then returns ture; otherwise returns false
      */
    bool run(const String &inputPath, const String &outputPath, bool clearOutput);

    /*! Register new translator factory
     * @param factory Pointer to translator factory to register
     */
    void registerTranslator(iTranslatorFactory *factory);
    /*! Check if translator for specified file extension exists
     * @param ext File extension to check
     * @param If translator for specified file extension registered, then returns true; otherwise returns false
     */
    bool hasTranslator(const std::string &ext) const;

protected:


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

    //! Translator factories map
    typedef std::map<std::string, iTranslatorFactory*> tTranslatorFactories;
    tTranslatorFactories mTranslatorFactories;

};

#endif
