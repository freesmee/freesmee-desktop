#ifndef REGULAREXPRESSION_H
#define REGULAREXPRESSION_H
#include "TRexpp.h"
#include <string>
namespace libJackSMS{
    class RegularExpression
    {
    private:
        TRexpp r;
    public:
        RegularExpression();
        void setRegExpr(const std::string &_regularExpression);
        bool Match(const std::string &str);
        std::string getSubExp(int n);
    };
}
#endif // REGULAREXPRESSION_H
