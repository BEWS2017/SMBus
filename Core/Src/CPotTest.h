/*
 * CConsoleTest.h
 *
 *  Created on: May 7, 2021
 *      Author: mnl
 */

#ifndef SRC_CPOTTEST_H_
#define SRC_CPOTTEST_H_

class CPotTest {
public:
	void run();
};

extern "C" void runPotTest() {
	CPotTest().run();
}

#endif /* SRC_CBOARDTEST_H_ */
