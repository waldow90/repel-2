#define BOOST_TEST_MODULE Domain
#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif
#ifdef HAVE_BOOST_UNIT_TEST_FRAMEWORK
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#else
#include <boost/test/included/unit_test.hpp>
#endif
#include <sstream>
#include "testutilities.h"
#include "../src/fol/domain.h"
#include "../src/fol/fol.h"
#include "../src/fol/folparser.h"


BOOST_AUTO_TEST_CASE( sat_test )
{
	std::stringstream facts;
	facts << "P(a,b) @ [1:10]";
	facts << "Q(a,b) @ [5:15]";

	std::vector<FOLToken> tokens = FOLParse::tokenize(&facts);
	std::vector<FOL::EventPair> factvec;
	FOLParse::parseEvents(tokens.begin(), tokens.end(), factvec);

	std::vector<WSentence> formulas;

	Domain d(factvec.begin(), factvec.end(), formulas.begin(), formulas.end());
	d.setMaxInterval(Interval(0,1000));
	boost::shared_ptr<Sentence> query = boost::dynamic_pointer_cast<Sentence>(factvec.front().first);
	SISet trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[1:10]}");

	// negation
	query = getAsSentence("!P(a,b)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[(0, 0), (0, 1000)], [(1, 10), (11, 1000)], [11:1000]}");

	//lets try disjunction
	query = getAsSentence("P(a,b) v Q(a,b)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[1:10], [5:15]}");

	// a bit more complicated
	query = getAsSentence("!(P(a,b) -> Q(a,b))");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[(1, 4), (1, 10)]}");

	// liq op
	query = getAsSentence("[ !P(a,b) ]");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[0:0], [11:1000]}");

	query = getAsSentence("[ !Q(a,b) ^ P(a,b) ]");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[1:4]}");

	// lets try liq disjunction
	query = getAsSentence("[ !(P(a,b) -> Q(a,b)) ]");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[1:4]}");

	// liq conjunction
	query = getAsSentence("[ P(a,b) ^ Q(a,b) ]");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[5:10]}");

	query = getAsSentence("[ P(a,b) v Q(a,b) ]");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[1:15]}");

	// diamond op
	query = getAsSentence("<>{mi} Q(a,b)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[(0, 14), (4, 14)]}");

	query = getAsSentence("<>{s,f} Q(a,b)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[(0, 14), (5, 15)], [(5, 15), (6, 1000)]}");

	// conjunction
	query = getAsSentence("P(a,b) ; Q(a,b)");
	trueAt = d.satisfied(*query, d.defaultModel());
	trueAt.makeDisjoint();
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[(1, 10), (5, 10)], [(1, 10), (11, 15)]}");

	query = getAsSentence("P(a,b) ^ Q(a,b)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[5:10]}");
}

BOOST_AUTO_TEST_CASE( conjunctionIntervalTest ) {
	std::stringstream facts;
	facts << "A(a) @ [1:10]";
	facts << "B(b) @ [2:10]";

	std::vector<FOLToken> tokens = FOLParse::tokenize(&facts);
	std::vector<FOL::EventPair> factvec;
	FOLParse::parseEvents(tokens.begin(), tokens.end(), factvec);
	std::vector<WSentence> formulas;
	Domain d(factvec.begin(), factvec.end(), formulas.begin(), formulas.end());

	boost::shared_ptr<Sentence> query = getAsSentence("<>{mi} A(a)");
	SISet trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[1:9]}");

	query = getAsSentence("<>{mi} B(b)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[1:9]}");
}

BOOST_AUTO_TEST_CASE( conjunctionMeetsTest ) {
	std::stringstream facts;
	facts << "Q(a) @ [1:1]";
	facts << "R(a) @ [3:3]";

	std::vector<FOLToken> tokens = FOLParse::tokenize(&facts);
	std::vector<FOL::EventPair> factvec;
	FOLParse::parseEvents(tokens.begin(), tokens.end(), factvec);
	std::vector<WSentence> formulas;
	Domain d(factvec.begin(), factvec.end(), formulas.begin(), formulas.end());

	boost::shared_ptr<Sentence> query = getAsSentence("Q(a) ; R(a)");
	SISet trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{}");
}

BOOST_AUTO_TEST_CASE( conjunctionOverlapsTest ) {
	std::stringstream facts;
	facts << "Q(a) @ [1:1]";
	facts << "R(a) @ [1:2]";
	facts << "S(a) @ [2:3]";

	std::vector<FOLToken> tokens = FOLParse::tokenize(&facts);
	std::vector<FOL::EventPair> factvec;
	FOLParse::parseEvents(tokens.begin(), tokens.end(), factvec);
	std::vector<WSentence> formulas;
	Domain d(factvec.begin(), factvec.end(), formulas.begin(), formulas.end());

	boost::shared_ptr<Sentence> query = getAsSentence("R(a) ^{o} R(a)");
	SISet trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{}");

	query = getAsSentence("R(a) ^{o} Q(a)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{}");

	query = getAsSentence("Q(a) ^{o} R(a)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{}");

	query = getAsSentence("Q(a) ^{o} Q(a)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{}");

	query = getAsSentence("R(a) ^{o} S(a)");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[(1, 1), (3, 3)]}");
}

BOOST_AUTO_TEST_CASE( trueFalseTest ) {
	std::stringstream facts;
	facts << "Q(a) @ [1:3]";
	facts << "R(a) @ [3:5]";
	facts << "S(a) @ [1:3]";

	std::vector<FOLToken> tokens = FOLParse::tokenize(&facts);
	std::vector<FOL::EventPair> factvec;
	FOLParse::parseEvents(tokens.begin(), tokens.end(), factvec);
	std::vector<WSentence> formulas;
	Domain d(factvec.begin(), factvec.end(), formulas.begin(), formulas.end());

	boost::shared_ptr<Sentence> query;
	SISet trueAt;

	query = getAsSentence("true");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{[1:5]}");

	query = getAsSentence("false");
	trueAt = d.satisfied(*query, d.defaultModel());
	BOOST_CHECK_EQUAL(trueAt.toString(), "{}");

	query = getAsSentence("S(a) ; S(a)");
	trueAt = d.satisfied(*query, d.defaultModel());
	trueAt.makeDisjoint();
	//BOOST_CHECK_EQUAL(trueAt.toString(), "{}");
}



