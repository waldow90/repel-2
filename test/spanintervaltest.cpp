#define BOOST_TEST_MODULE SpanInterval 
#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif
#ifdef HAVE_BOOST_UNIT_TEST_FRAMEWORK
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#else
#include <boost/test/included/unit_test.hpp>
#endif
#include <boost/optional.hpp>
#include "../src/spaninterval.h"
#include "../src/interval.h"
#include "../src/siset.h"

#include <boost/foreach.hpp>
#include <iostream>
#include <set>

BOOST_AUTO_TEST_CASE( basic_test )
{
	{
		SpanInterval sp1(1,11,1,11);
		SpanInterval sp2(5,10,5,10);
		Interval start = sp1.start();
		Interval end = sp1.end();
		BOOST_CHECK_EQUAL(start.start(), 1);
		BOOST_CHECK_EQUAL(start.end(), 11);
		BOOST_CHECK_EQUAL(end.start(), 1);
		BOOST_CHECK_EQUAL(end.end(), 11);

		std::set<SpanInterval> removed;
		sp1.subtract(sp2,removed);

		BOOST_CHECK(removed.size() == 3);
		BOOST_CHECK(removed.find(SpanInterval(1,4,1,11)) != removed.end());
		BOOST_CHECK(removed.find(SpanInterval(11,11,11,11)) != removed.end());
		BOOST_CHECK(removed.find(SpanInterval(5,10,11,11)) != removed.end());
	}
	{
		SpanInterval sp1(1,11, 10,14);
		SpanInterval sp2(5,7, 5,9);
		std::set<SpanInterval> removed;
		sp1.subtract(sp2,removed);

		BOOST_CHECK(removed.size() == 3);
		BOOST_CHECK(removed.find(SpanInterval(1,4,10,14))!=removed.end());
		BOOST_CHECK(removed.find(SpanInterval(5,7,10,14))!=removed.end());
		BOOST_CHECK(removed.find(SpanInterval(8,11,10,14))!=removed.end());
	}

}

BOOST_AUTO_TEST_CASE( siset_test ) {
	SpanInterval sp1(1,10,1,10);
	SpanInterval sp2(8,11,8,11);
	SISet set;
	BOOST_CHECK(set.isDisjoint());
	set.add(sp1);
	BOOST_CHECK(set.isDisjoint());
	set.add(sp2);
	BOOST_CHECK(!set.isDisjoint());
	set.makeDisjoint();
	BOOST_CHECK(set.isDisjoint());

	// TODO turn this part into a test
	std::cout << "compliment of " << set.toString() << " is " << set.compliment().toString() << std::endl;
	SISet dcompliment = set.compliment().compliment();
	dcompliment.makeDisjoint();
	std::cout << "double compliment is " << dcompliment.toString() << std::endl;
}

BOOST_AUTO_TEST_CASE( sisetliq_test ) {
	SpanInterval sp1(1,10,1,10);
	SpanInterval sp2(6,11,6,11);
	SISet set(true);

	set.add(sp1);
	set.add(sp2);
	BOOST_CHECK(!set.isDisjoint());
	set.makeDisjoint();
	BOOST_CHECK(set.isDisjoint());
	std::cout << "compliment of " << set.toString() << " is " << set.compliment().toString() << std::endl;
	std::cout << "double compliment is " << set.compliment().compliment().toString() << std::endl;
}

BOOST_AUTO_TEST_CASE( spanInterval_relations ) {
	Interval maxInterval(0, 1000);
	SpanInterval sp1(1,5,6,10, maxInterval);
	boost::optional<SpanInterval> sp2 = sp1.satisfiesRelation(Interval::MEETS);
	BOOST_CHECK(sp2);
	BOOST_CHECK_EQUAL(sp2.get().toString(), "[(7, 11), (7, 1000)]");
	sp1 = SpanInterval(999, 1000, 999, 1000, maxInterval);
	sp2 = sp1.satisfiesRelation(Interval::MEETS);
	BOOST_CHECK(!sp2);

	sp1 = SpanInterval(1,4,7,9, maxInterval);
	sp2 = sp1.satisfiesRelation(Interval::OVERLAPS);
	BOOST_CHECK(sp2);
	BOOST_CHECK_EQUAL(sp2.get().toString(), "[(2, 9), (8, 1000)]");

	sp2 = sp1.satisfiesRelation(Interval::OVERLAPSI);
	BOOST_CHECK(sp2);
	BOOST_CHECK_EQUAL(sp2.get().toString(), "[(0, 3), (1, 8)]");

	sp2 = sp1.satisfiesRelation(Interval::STARTS);
	BOOST_CHECK(sp2);
	BOOST_CHECK_EQUAL(sp2.get().toString(), "[(1, 4), (8, 1000)]");

	sp2 = sp1.satisfiesRelation(Interval::STARTSI);
	BOOST_CHECK(sp2);
	BOOST_CHECK_EQUAL(sp2.get().toString(), "[(1, 4), (1, 8)]");
}
