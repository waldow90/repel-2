/*
 * ELSyntax.h
 *
 *  Created on: May 21, 2011
 *      Author: joe
 */

#ifndef EL_SYNTAX_H_
#define EL_SYNTAX_H_

#include <boost/shared_ptr.hpp>
#include <utility>
#include <vector>
#include <deque>

/* include all the elements needed for FOL sentences */
#include "syntax/Atom.h"
#include "syntax/BoolLit.h"
#include "syntax/Conjunction.h"
#include "syntax/Constant.h"
#include "syntax/DiamondOp.h"
#include "syntax/Disjunction.h"
#include "Event.h"
#include "syntax/LiquidOp.h"
#include "syntax/Negation.h"
#include "syntax/Sentence.h"
#include "syntax/Term.h"
#include "syntax/Variable.h"
#include "syntax/ELSentence.h"
#include "syntax/SentenceVisitor.h"
#include "syntax/Proposition.h"
#include "../SpanInterval.h"

/*
namespace FOL {
    typedef std::pair<boost::shared_ptr<Atom>, SpanInterval, bool> EventPair;
}
*/

inline std::vector<const Sentence*> getDisjunctionArgs(const Disjunction& d) {
    std::vector<const Sentence*> vec;

    const Disjunction *dLeft = dynamic_cast<const Disjunction *>(&*d.left());
    const Disjunction *dRight = dynamic_cast<const Disjunction *>(&*d.right());
    if (dLeft != 0) {
        std::vector<const Sentence*> vecL = getDisjunctionArgs(*dLeft);
        vec.insert(vec.end(), vecL.begin(), vecL.end());
    } else {
        vec.push_back(&*d.left());
    }

    if (dRight != 0) {
        std::vector<const Sentence*> vecR = getDisjunctionArgs(*dRight);
        vec.insert(vec.end(), vecR.begin(), vecR.end());
    } else {
        vec.push_back(&*d.right());
    }

    return vec;
};

inline std::vector<const Sentence*> getMeetsConjunctionArgs(const Conjunction& c) {
    std::vector<const Sentence*> vec;
    std::set<Interval::INTERVAL_RELATION> justMeets;
    justMeets.insert(Interval::MEETS);

    const Conjunction *cLeft = dynamic_cast<const Conjunction *>(&*c.left());
    const Conjunction *cRight = dynamic_cast<const Conjunction *>(&*c.right());
    if (cLeft != 0 && cLeft->relations() == justMeets) {
        std::vector<const Sentence*> vecL = getMeetsConjunctionArgs(*cLeft);
        vec.insert(vec.end(), vecL.begin(), vecL.end());
    } else {
        vec.push_back(&*c.left());
    }
    if (cRight != 0 && cRight->relations() == justMeets) {
        std::vector<const Sentence*> vecR = getMeetsConjunctionArgs(*cRight);
        vec.insert(vec.end(), vecR.begin(), vecR.end());
    } else {
        vec.push_back(&*c.right());
    }

    return vec;
}

inline boost::shared_ptr<Disjunction> wrapInDisjunction(const std::vector<const Sentence*>& sentences) {
    assert(sentences.size() >= 2);
    std::deque<boost::shared_ptr<Sentence> > copies;
    // first, make a copy of all terms
    for(std::vector<const Sentence*>::const_iterator it = sentences.begin(); it != sentences.end(); it++) {
        boost::shared_ptr<Sentence> copy((*it)->clone());
        copies.push_back(copy);
    }

    // wrap them in a disjunction
    boost::shared_ptr<Disjunction> dis(new Disjunction(copies[0], copies[1]));
    copies.pop_front();
    copies.pop_front();
    while (copies.size() > 0) {
        boost::shared_ptr<Disjunction> newDis(new Disjunction(dis, copies[0]));
        copies.pop_front();
        dis = newDis;
    }

    return dis;
}

inline boost::shared_ptr<Conjunction> wrapInMeetsConjunction(const std::vector<const Sentence*>& sentences) {
    assert(sentences.size() >= 2);
    std::deque<boost::shared_ptr<Sentence> > copies;
    // first, make a copy of all terms
    for(std::vector<const Sentence*>::const_iterator it = sentences.begin(); it != sentences.end(); it++) {
        boost::shared_ptr<Sentence> copy((*it)->clone());
        copies.push_back(copy);
    }

    // wrap them in a conjunction
    boost::shared_ptr<Conjunction> con(new Conjunction(copies[0], copies[1], Interval::MEETS));
    copies.pop_front();
    copies.pop_front();
    while (copies.size() > 0) {
        boost::shared_ptr<Conjunction> newCon(new Conjunction(con, copies[0], Interval::MEETS));
        copies.pop_front();
        con = newCon;
    }

    return con;
}

#endif /* EL_SYNTAX_H_ */
