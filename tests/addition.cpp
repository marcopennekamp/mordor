#include "stdafx.h"
#include "CppUnitTest.h"

#include <stdio.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests {		
	
    TEST_CLASS (UnitTest1) {
	  public:
		
		TEST_METHOD (TestMethod1) {
            printf ("Hello World!");
		}
	};
}