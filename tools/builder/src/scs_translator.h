#ifndef _scs_translator_h_
#define _scs_translator_h_

#include "translator.h"

#include "parser/scsLexer.h"
#include "parser/scsParser.h"

class SCsTranslator : public iTranslator
{
public:
    explicit SCsTranslator();
    virtual ~SCsTranslator();

    //! @copydoc iTranslator::translate
    bool translate(const std::string &filename);
    //! @copydoc iTranlstor::getFileExt
    const std::string& getFileExt() const;

private:
    //! Process string data
    bool processString(const String &data);
    /*! Builds sc-text based on parsed antlr tree
     * @param Pointer to root antlr tree node
     * @returns If tree parsed without errors, then return true; otherwise returns false.
     */
    bool buildScText(pANTLR3_BASE_TREE tree);
};

// -----------------

class SCsTranslatorFactory : public iTranslatorFactory
{
public:
    explicit SCsTranslatorFactory();
    virtual ~SCsTranslatorFactory();

    //! @copydoc iTranslatorFactory::createInstance
    iTranslator* createInstance();
    //! @copydoc iTranslatorFactory::getFileExt
    const std::string& getFileExt() const;
};

#endif // _scs_translator_h_
