#ifndef _builder_h_
#define _builder_h_

#include "types.h"

class iTranslator;
class iTranslatorFactory;

struct BuilderParams
{
    //! Input directory path
    String inputPath;
    //! Output directory path
    String outputPath;
    //! Flag to clear output
    bool clearOutput;
    //! Flag to generate format information based on file extensions
    bool autoFormatInfo;
    //! Path to configuration file
    String configFile;
};

class Builder
{
public:
    explicit Builder();
    virtual ~Builder();

    //! Initialize builder. Create and register translator factories
    void initialize();

    /*! Run building process
      * @param options Builder options
      * @returns If build finished, then returns ture; otherwise returns false
      */
    bool run(const BuilderParams &options);

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
    //! Set of files to process
    typedef std::set< String > tFileSet;
    tFileSet mFileSet;

    //! Translator factories map
    typedef std::map<std::string, iTranslatorFactory*> tTranslatorFactories;
    tTranslatorFactories mTranslatorFactories;

    //! Builder parameters
    BuilderParams mParams;

};

#endif
