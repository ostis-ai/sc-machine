/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#ifndef _builder_h_
#define _builder_h_

#include "types.h"
extern "C"
{
#include "sc_memory_headers.h"
#include "sc_helper.h"
}


class iTranslator;
class iTranslatorFactory;

struct BuilderParams
{
    //! Input directory path
    String inputPath;
    //! Output directory path
    String outputPath;
    //! Path to memory extensions
    String extensionsPath;
    //! Flag to clear output
    bool clearOutput:1;
    //! Flag to generate format information based on file extensions
    bool autoFormatInfo:1;
    //! Path to configuration file
    String configFile;
	//! Flag to show processing file names
	bool showFileNames : 1;
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

    bool hasErrors() const;
    
protected:


    //! Process specified file
    bool processFile(const String &filename);

    //! Collect files in directory
    void collectFiles(const String & path);
    //! Collecting files for process
    void collectFiles();


private:
    //! Set of files to process
    typedef std::set< String > tFileSet;
    tFileSet mFileSet;

    //! Translator factories map
    typedef std::map<std::string, iTranslatorFactory*> tTranslatorFactories;
    tTranslatorFactories mTranslatorFactories;

    //! List of errors
    typedef std::list<std::string> tStringList;
    tStringList mErrors;

    //! Builder parameters
    BuilderParams mParams;

    //! Memory context
    sc_memory_context *mContext;

};

#endif
