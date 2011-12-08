/*
 * wsentence.h
 *
 *  Created on: May 21, 2011
 *      Author: joe
 */


#ifndef WSENTENCE_H_
#define WSENTENCE_H_

#include <boost/shared_ptr.hpp>
#include "sentence.h"

class WSentence {
public:
	WSentence(const boost::shared_ptr<Sentence>& s)
		: s_(s), w_(1), hasInfWeight_(true) {};
	WSentence(const boost::shared_ptr<Sentence>& s, unsigned int w)
		: s_(s), w_(w), hasInfWeight_(false) {};
	virtual ~WSentence() {};

	bool operator==(const WSentence& b) const {return (*s_ == *b.s_ && w_ == b.w_ && hasInfWeight_ == b.hasInfWeight_);};
	boost::shared_ptr<Sentence> sentence() { return s_;}
	const boost::shared_ptr<const Sentence> sentence() const {return s_;}
	bool hasInfWeight() const {return hasInfWeight_;}
	unsigned int weight() const {return w_;};

	void setSentence(const boost::shared_ptr<Sentence>& s) {s_ = s;};
	void setWeight(unsigned int w) {w_ = w; hasInfWeight_ = false;};
	void setHasInfWeight() {w_ = 1; hasInfWeight_ = true;};

private:
	boost::shared_ptr<Sentence> s_;
	unsigned int w_;
	bool hasInfWeight_;

};

#endif /* WSENTENCE_H_ */