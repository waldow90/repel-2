#define BOOST_TEST_MODULE FOLLexer 
#define BOOST_TEST_MAIN
#include "../src/config.h"
#ifdef USE_DYNAMIC_UNIT_TEST
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#else
#include <boost/test/included/unit_test.hpp>
#endif
#include "logic/FOLLexer.h"
#include "logic/FOLToken.h"
#include <iostream>
#include <string>
#include <vector>

BOOST_AUTO_TEST_CASE( parser_test )
{
    std::istringstream stream("flying(a,theAir) @ [1,11] # this is a test of a comment \n");

    std::vector<FOLToken> tokens = FOLParse::tokenize(stream);
    BOOST_CHECK_EQUAL(tokens.size(), 13);
    BOOST_CHECK_EQUAL(tokens[0].type(), FOLParse::Identifier);
    BOOST_CHECK_EQUAL(tokens[0].contents(), "flying");
    BOOST_CHECK_EQUAL(tokens[1].type(), FOLParse::OpenParen);
    BOOST_CHECK_EQUAL(tokens[2].type(), FOLParse::Identifier);
    BOOST_CHECK_EQUAL(tokens[2].contents(), "a");
    BOOST_CHECK_EQUAL(tokens[3].type(), FOLParse::Comma);
    BOOST_CHECK_EQUAL(tokens[4].type(), FOLParse::Identifier);
    BOOST_CHECK_EQUAL(tokens[4].contents(), "theAir");
    BOOST_CHECK_EQUAL(tokens[5].type(), FOLParse::CloseParen);
    BOOST_CHECK_EQUAL(tokens[6].type(), FOLParse::At);
    BOOST_CHECK_EQUAL(tokens[7].type(), FOLParse::OpenBracket);
    BOOST_CHECK_EQUAL(tokens[8].type(), FOLParse::Number);
    BOOST_CHECK_EQUAL(tokens[8].contents(), "1");
    BOOST_CHECK_EQUAL(tokens[9].type(), FOLParse::Comma);
    BOOST_CHECK_EQUAL(tokens[10].type(), FOLParse::Number);
    BOOST_CHECK_EQUAL(tokens[10].contents(), "11");
    BOOST_CHECK_EQUAL(tokens[11].type(), FOLParse::CloseBracket);
    BOOST_CHECK_EQUAL(tokens[12].type(), FOLParse::EndLine);
}
