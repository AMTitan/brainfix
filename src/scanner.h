// Generated by Flexc++ V2.07.07 on Mon, 10 Jan 2022 18:17:24 +0100

#ifndef Scanner_H_INCLUDED_
#define Scanner_H_INCLUDED_

// $insert baseclass_h
#include "scannerbase.h"
#include "compilerbase.h"

// $insert classHead
class Scanner: public ScannerBase
{
public:
    explicit Scanner(std::istream &in = std::cin,
                     std::ostream &out = std::cout);

    Scanner(std::string const &infile, std::string const &outfile);
    using ScannerBase::pushStream;
        
    // $insert lexFunctionDecl
    int lex();

private:
    int lex_();
    int executeAction_(size_t ruleNr);

    void print();
    void preCode();     // re-implement this function for code that must 
    // be exec'ed before the patternmatching starts

    void postCode(PostEnum_ type);    
    // re-implement this function for code that must 
    // be exec'ed after the rules's actions.

    static std::string escape(char c);
    static std::string escape(std::string const &matched);
};

// $insert scannerConstructors
inline Scanner::Scanner(std::istream &in, std::ostream &out)
    :
    ScannerBase(in, out)
{}

inline Scanner::Scanner(std::string const &infile, std::string const &outfile)
    :
    ScannerBase(infile, outfile)
{}

// $insert inlineLexFunction
inline int Scanner::lex()
{
    return lex_();
}

inline void Scanner::preCode() 
{
    // optionally replace by your own code
}

inline void Scanner::postCode([[maybe_unused]] PostEnum_ type) 
{
    // optionally replace by your own code
}

inline void Scanner::print() 
{
    print_();
}

#endif // Scanner_H_INCLUDED_

