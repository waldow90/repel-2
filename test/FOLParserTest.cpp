#define BOOST_TEST_MODULE FOLParser
#define BOOST_TEST_MAIN
#include "../src/config.h"
#ifdef USE_DYNAMIC_UNIT_TEST
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#else
#include <boost/test/included/unit_test.hpp>
#endif
#include <boost/assign/list_of.hpp>
#include "logic/FOLParser.h"
#include "logic/FOLLexer.h"
#include "logic/FOLToken.h"
#include "logic/ELSyntax.h"
#include "SpanInterval.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>


BOOST_AUTO_TEST_CASE( parser_test )
{
    std::istringstream stream("flying(a,theAir) @ [1,11] # this is a test of a comment \n");

    std::vector<FOLToken> tokens = FOLParse::tokenize(stream);
    boost::shared_ptr<Atom> a = FOLParse::parseGroundAtom(tokens.begin(), tokens.end());
    BOOST_CHECK_EQUAL(a->name(), "flying");
    BOOST_CHECK_EQUAL(a->arity(), 2);
}

BOOST_AUTO_TEST_CASE( interval_test )
{
    std::istringstream stream("[1,11]");
    std::vector<FOLToken> tokens = FOLParse::tokenize(stream);
    SpanInterval interval = FOLParse::parseInterval(tokens.begin(), tokens.end());
    BOOST_CHECK_EQUAL(interval.start().start(), 1);
    BOOST_CHECK_EQUAL(interval.start().finish(), 1);
    BOOST_CHECK_EQUAL(interval.finish().start(), 11);
    BOOST_CHECK_EQUAL(interval.finish().finish(), 11);
}

BOOST_AUTO_TEST_CASE( event_test) 
{
    std::istringstream stream("mrManager(georgeMichael, bananaStand) @ [10:20]");
    std::vector<FOLToken> tokens = FOLParse::tokenize(stream);
    std::vector<FOL::Event> events = FOLParse::parseEvent(tokens.begin(), tokens.end());
    BOOST_REQUIRE_EQUAL(events.size(), 1);
    FOL::Event e = events[0];
    BOOST_CHECK_EQUAL(e.atom()->name(), "mrManager");
    BOOST_CHECK_EQUAL(e.atom()->at(0).name(), "georgeMichael");
    BOOST_CHECK_EQUAL(e.atom()->at(1).name(), "bananaStand");
}

BOOST_AUTO_TEST_CASE( multipleEvent_test)
{
    std::istringstream stream("mrManager(georgeMichael, bananaStand) @ {[10:20], [25:30]}");
    std::vector<FOLToken> tokens = FOLParse::tokenize(stream);
    std::vector<FOL::Event> events = FOLParse::parseEvent(tokens.begin(), tokens.end());
    BOOST_REQUIRE_EQUAL(events.size(), 2);
    FOL::Event e = events[0];
    FOL::Event e2 = events[0];
    BOOST_CHECK_EQUAL(e.atom(), e2.atom());
    BOOST_CHECK_EQUAL(e.atom()->name(), "mrManager");
    BOOST_CHECK_EQUAL(e.atom()->at(0).name(), "georgeMichael");
    BOOST_CHECK_EQUAL(e.atom()->at(1).name(), "bananaStand");
}

BOOST_AUTO_TEST_CASE( atom_test) 
{
    std::istringstream stream("love(?cats, meowmix)");
    std::vector<FOLToken> tokens = FOLParse::tokenize(stream);
    boost::shared_ptr<Atom> a = FOLParse::parseAtom(tokens.begin(), tokens.end());
    BOOST_CHECK_EQUAL(a->name(), "love");
    BOOST_CHECK_EQUAL(a->arity(), 2);
    BOOST_CHECK_EQUAL(a->at(0).name(), "cats");
    BOOST_CHECK_EQUAL(a->at(1).name(), "meowmix");
    BOOST_CHECK(dynamic_cast<Variable*>(&a->at(0)) != NULL);
    BOOST_CHECK(dynamic_cast<Constant*>(&a->at(1)) != NULL);
}

BOOST_AUTO_TEST_CASE( static_formula_test )
{
    std::istringstream stream("p(?x,y)");   // simple atom case
    std::vector<FOLToken> tokens = FOLParse::tokenize(stream);
    boost::shared_ptr<Sentence> s = FOLParse::parseStaticFormula(tokens.begin(), tokens.end());
    boost::shared_ptr<Atom> a = boost::dynamic_pointer_cast<Atom>(s);
    BOOST_CHECK(a != NULL);
    BOOST_CHECK_EQUAL(a->name(), "p");
    BOOST_CHECK(dynamic_cast<Variable*>(&a->at(0)) != NULL);

    std::istringstream stream2("p(x) ^ q(x) -> r(x)");
    tokens = FOLParse::tokenize(stream2);
    boost::shared_ptr<Sentence> s2 = FOLParse::parseStaticFormula(tokens.begin(), tokens.end());
    BOOST_CHECK_EQUAL(s2->toString(), "!(p(x) ^ q(x)) v r(x)");

    std::istringstream stream3("p(x) ^ (q(x) v r(x))");
    tokens = FOLParse::tokenize(stream3);
    boost::shared_ptr<Sentence> s3 = FOLParse::parseStaticFormula(tokens.begin(), tokens.end());
    BOOST_CHECK_EQUAL(s3->toString(), "p(x) ^ (q(x) v r(x))");

    std::istringstream stream4("p(x) ^ q(x) v r(x) -> z(x)");
    tokens = FOLParse::tokenize(stream4);
    boost::shared_ptr<Sentence> s4 = FOLParse::parseStaticFormula(tokens.begin(), tokens.end());
    BOOST_CHECK_EQUAL(s4->toString(), "!(p(x) ^ q(x) v r(x)) v z(x)");
}

BOOST_AUTO_TEST_CASE( weighted_formula_test ) {
    std::istringstream stream("10: p(?x,y)");   // simple atom case
    std::vector<FOLToken> tokens = FOLParse::tokenize(stream);
    ELSentence form = FOLParse::parseWeightedFormula(tokens.begin(), tokens.end());

    BOOST_CHECK_EQUAL(form.weight(), 10);
    BOOST_CHECK_EQUAL(form.sentence()->toString(), "p(?x, y)");

    std::istringstream stream2("55: [ p(?x,?y) ^ q(?x,?y) -> r(?x, ?y) ]");
    tokens = FOLParse::tokenize(stream2);
    form = FOLParse::parseWeightedFormula(tokens.begin(), tokens.end());

    BOOST_CHECK_EQUAL(form.weight(), 55);
    BOOST_CHECK_EQUAL(form.sentence()->toString(), "[ !(p(?x, ?y) ^ q(?x, ?y)) v r(?x, ?y) ]");

    std::istringstream stream3("0: <>{m,mi} (p(x) ^{o} q(x) ; r(x) ; <> z(x))");
    tokens = FOLParse::tokenize(stream3);
    form = FOLParse::parseWeightedFormula(tokens.begin(), tokens.end());

    BOOST_CHECK_EQUAL(form.weight(), 0);
    BOOST_CHECK_EQUAL(form.sentence()->toString(), "<>{m, mi} (p(x) ^{o} q(x) ; r(x) ; <> z(x))");

    std::istringstream stream4("0: !p(a) v q(a)");
    tokens = FOLParse::tokenize(stream4);
    form = FOLParse::parseWeightedFormula(tokens.begin(), tokens.end());

    BOOST_CHECK_EQUAL(form.weight(), 0);
    BOOST_CHECK(boost::dynamic_pointer_cast<Disjunction>(form.sentence()));

}

BOOST_AUTO_TEST_CASE( quantified_formula_test ) {
    std::istringstream stream("10: p(?x,y) @ [1:10]");  // simple atom case
    std::vector<FOLToken> tokens = FOLParse::tokenize(stream);
    ELSentence form = FOLParse::parseWeightedFormula(tokens.begin(), tokens.end());

    BOOST_CHECK_EQUAL(form.weight(), 10);
    BOOST_CHECK_EQUAL(form.sentence()->toString(), "p(?x, y)");
    BOOST_REQUIRE(form.isQuantified());
    BOOST_CHECK_EQUAL(form.quantification().toString(), "{[1:10]}");

    std::istringstream stream2("10: p(?x, y) v q(a) @ {[1:10], [11:12], [(13, 13), (14, 15)] }");   // simple atom case
    tokens = FOLParse::tokenize(stream2);
    form = FOLParse::parseWeightedFormula(tokens.begin(), tokens.end());

    BOOST_CHECK_EQUAL(form.weight(), 10);
    BOOST_CHECK_EQUAL(form.sentence()->toString(), "p(?x, y) v q(a)");
    BOOST_REQUIRE(form.isQuantified());
    BOOST_CHECK_EQUAL(form.quantification().toString(), "{[1:10], [11:12], [(13, 13), (14, 15)]}");

}

BOOST_AUTO_TEST_CASE( type_test ) {
    std::stringstream str("type: dogs = {fred, louie, speedy}");
    std::stringstream str2("type: smells(fred, bad)");

    std::vector<FOLToken> tokens = FOLParse::tokenize(str);
    std::vector<FOLToken> tokens2 = FOLParse::tokenize(str2);
    std::map<std::string, std::set<std::string> > objTypes;
    std::map<std::string, std::vector<std::string> > predTypes;
    FOLParse::parseTypes(tokens.begin(), tokens.end(), objTypes, predTypes);
    FOLParse::parseTypes(tokens2.begin(), tokens2.end(), objTypes, predTypes);

    BOOST_REQUIRE_EQUAL(objTypes.size(), 1);
    BOOST_REQUIRE_EQUAL(objTypes.count("dogs"), 1);
    const std::set<std::string> expectedTypes = boost::assign::list_of("fred")("louie")("speedy");
    BOOST_REQUIRE_EQUAL_COLLECTIONS(objTypes["dogs"].begin(), objTypes["dogs"].end(),
            expectedTypes.begin(), expectedTypes.end());

    BOOST_REQUIRE_EQUAL(predTypes.size(), 1);
    BOOST_REQUIRE_EQUAL(predTypes.count("smells"), 1);
    const std::vector<std::string> expectedArgs = boost::assign::list_of("fred")("bad");
    BOOST_REQUIRE_EQUAL_COLLECTIONS(predTypes["smells"].begin(), predTypes["smells"].end(),
            expectedArgs.begin(), expectedArgs.end());

}
