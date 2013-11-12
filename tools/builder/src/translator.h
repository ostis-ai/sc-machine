#ifndef _translator_h_
#define _translator_h_

#include "types.h"

/*! Interface for translators
 */
class iTranslator
{
public:
    explicit iTranslator() {};
    virtual ~iTranslator() {};

    /*! Translate specified file into memory
     * @param filename Name of file to translate
     * @return If file translated without any errors, then returns true; otherwise returns false.
     */
    virtual bool translate(const std::string &filename) = 0;

    //! Returns supported file extension
    virtual const std::string& getFileExt() const = 0;
};

/*! Interface for translators factory
 */
class iTranslatorFactory
{
public:
    explicit iTranslatorFactory() {};
    virtual ~iTranslatorFactory() {};

    /*! Function to create translator instance
     */
    virtual iTranslator* createInstance() = 0;

    //! Returns supported file extension
    virtual const std::string& getFileExt() const = 0;
};

#endif // _translator_h_
