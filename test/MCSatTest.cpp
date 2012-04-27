/*
 * mcsattest.cpp
 *
 *  Created on: Feb 12, 2012
 *      Author: selman.joe@gmail.com
 */

#define BOOST_TEST_MODULE MCSat
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/random.hpp>
#include <string>
#include "../src/inference/MCSat.h"
#include "../src/inference/MCSatSamplePerfectlyStrategy.h"
#include "../src/inference/MCSatSampleLiquidlyStrategy.h"
#include "../src/logic/syntax/Proposition.h"
#include "TestUtilities.h"

BOOST_AUTO_TEST_CASE( mcsat_test)
{
    // disable debug logging
    FileLog::globalLogLevel() = LOG_ERROR;
    std::string facts("D-P(a) @ {[1:5]}\n");
    std::string formulas("0.01: [ P(a) -> D-P(a) ] @ [1:10]\n"
            "0.01: [ D-P(a) -> P(a) ] @ [1:10]\n");

    Domain d = loadDomainWithStreams(facts, formulas);

    MCSat mcSatSolver(&d);
    mcSatSolver.setSampleStrategy(new MCSatSampleLiquidlyStrategy());
    //mcSatSolver.setBurnInIterations(10000);
    mcSatSolver.run();
    BOOST_CHECK_EQUAL(mcSatSolver.size(), MCSat::defNumSamples);
    boost::shared_ptr<Sentence> pa = getAsSentence("P(a)");
    Proposition propPa(static_cast<const Atom&>(*pa), true);
    double probPaAt11 =  mcSatSolver.estimateProbability(propPa, Interval(1,1));
    std::cout << "probPaAt11 = " << probPaAt11 << std::endl;

    std::cout << "counts:" << std::endl;
    for (unsigned int i = 1; i <= 10; i++) {
        std::cout << mcSatSolver.countProps(propPa, Interval(i,i));
        if (i != 10) std::cout << ", ";
    }
    std::cout << std::endl;
    //BOOST_CHECK_CLOSE(probPaAt11, .99, .2);
}

BOOST_AUTO_TEST_CASE( mcsatSamplePerfectlyStrategy) {
    std::string facts("D-P(a) @ {[1:25]}\n");
    std::string formulas("100: [ P(a) -> D-P(a) ] @ [1:15]\n");
    Domain d = loadDomainWithStreams(facts, formulas);
    MCSatSamplePerfectlyStrategy strategy;
    std::vector<ELSentence> sampled;
    strategy.sampleSentences(d.defaultModel(), d, sampled);
    BOOST_REQUIRE(sampled.size() == 1);
    std::cout << sampled[0].quantification().toString();

}

