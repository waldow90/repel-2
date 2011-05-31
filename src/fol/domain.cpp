#include "domain.h"
#include "fol.h"
#include "atom.h"
#include <boost/shared_ptr.hpp>
#include <stdexcept>

bool Domain::isLiquid(const std::string& predicate) const {
	// TODO: implement this sometime!
	return true;
}

SISet Domain::satisfied(const Sentence& s, const Model& m) const {
	// start with the base case
	if (dynamic_cast<const Atom *>(&s) != 0) {
		const Atom* a = dynamic_cast<const Atom *>(&s);
		return satisfiedAtom(*a, m);
	} else if (dynamic_cast<const Negation*>(&s) != 0) {
		const Negation* n = dynamic_cast<const Negation *>(&s);
		SISet set = satisfiedNegation(*n, m);
		set.makeDisjoint();
		return set;
	} else if (dynamic_cast<const Disjunction*>(&s) != 0) {
		const Disjunction* d = dynamic_cast<const Disjunction *>(&s);
		SISet set = satisfiedDisjunction(*d, m);
		set.makeDisjoint();
		return set;
	} else if (dynamic_cast<const LiquidOp*>(&s) != 0) {
		const LiquidOp* l = dynamic_cast<const LiquidOp *>(&s);
		SISet set = liqSatisfied(*(l->sentence()), m);
		// remove liquid restriction
		set.setForceLiquid(false);
		set.makeDisjoint();
		return set;
	}
	// made it this far, unimplemented!
	std::runtime_error e("Domain::satisfied not implemented yet!");
	throw e;
}

SISet Domain::satisfiedAtom(const Atom& a, const Model& m) const {
	if (a.isGrounded()) {
		// make sure its in model
		if (m.find(a) != m.end()){
			SISet set = m.at(a);
			// force non-liquidity
			set.setForceLiquid(false);
			return set;
		} else {
			return SISet(false);
		}
	}
	// grounding out of atoms currently not implemented yet!
	std::runtime_error e("Domain::satisfiedAtom grounding out of atoms not implemented!");
	throw e;
}

SISet Domain::satisfiedNegation(const Negation& n, const Model& m) const {
	// return the compliment of the set inside the negation
	SISet sat = satisfied(*(n.sentence()), m);
	sat = sat.compliment();
	return sat;
}

SISet Domain::satisfiedDisjunction(const Disjunction& d, const Model& m) const {
	// union the left and the right
	SISet leftSat = satisfied(*(d.left()),m);
	SISet rightSat = satisfied(*(d.right()), m);
	leftSat.add(rightSat);
	return leftSat;
}

SISet Domain::liqSatisfied(const Sentence& s, const Model& m) const {
	if (dynamic_cast<const Atom *>(&s) != 0) {
		const Atom* a = dynamic_cast<const Atom *>(&s);
		return liqSatisfiedAtom(*a, m);
	} else if (dynamic_cast<const Negation *>(&s) != 0) {
		const Negation* n = dynamic_cast<const Negation *>(&s);
		return liqSatisfiedNegation(*n, m);
	} else if (dynamic_cast<const Disjunction *>(&s) != 0) {
		const Disjunction* d = dynamic_cast<const Disjunction *>(&s);
		return liqSatisfiedDisjunction(*d, m);
	}
	// made it this far, unimplemented!
	std::runtime_error e("Domain::liqSatisfied not implemented yet!");
	throw e;
}

SISet Domain::liqSatisfiedAtom(const Atom& a, const Model& m) const {
	if (!a.isGrounded()) {
		std::runtime_error e("Domain::liqSatisfiedAtom currently doesn't support variables!");
		throw e;
	}
	// make sure its in model
	if (m.find(a) != m.end()){
		SISet set = m.at(a);
		set.setForceLiquid(true);
		return set;
	} else {
		return SISet(true);
	}
}

SISet Domain::liqSatisfiedNegation(const Negation& n, const Model& m) const {
	SISet sat = liqSatisfied(*(n.sentence()), m);
	sat = sat.compliment();
	return sat;
}

SISet Domain::liqSatisfiedDisjunction(const Disjunction& d, const Model& m) const {
	SISet leftSat = liqSatisfied(*(d.left()),m);
	SISet rightSat = liqSatisfied(*(d.right()), m);
	leftSat.add(rightSat);
	return leftSat;
}