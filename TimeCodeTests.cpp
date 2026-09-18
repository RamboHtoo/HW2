#include <iostream>
#include <assert.h>
using namespace std;

#include "TimeCode.h"


void TestComponentsToSeconds(){
	cout << "Testing ComponentsToSeconds" << endl;
	
	// Random but "safe" inputs
	long long unsigned int t = TimeCode::ComponentsToSeconds(3, 17, 42);
	assert(t == 11862);
	
	// Check zero values and individual unit conversions.
	assert(TimeCode::ComponentsToSeconds(0, 0, 0) == 0);
	assert(TimeCode::ComponentsToSeconds(1, 0, 0) == 3600);
	assert(TimeCode::ComponentsToSeconds(0, 1, 0) == 60);
	assert(TimeCode::ComponentsToSeconds(0, 0, 1) == 1);

	// ComponentsToSeconds accepts oversized components because its job
	// is conversion; normalization occurs when the value is read back.
	assert(TimeCode::ComponentsToSeconds(0, 60, 0) == 3600);
	
	
	cout << "PASSED!" << endl << endl;
}


void TestDefaultConstructor(){
	cout << "Testing Default Constructor" << endl;
	TimeCode tc;

	assert(tc.ToString() == "0:0:0");
	
	cout << "PASSED!" << endl << endl;
}


void TestComponentConstructor(){
	cout << "Testing Component Constructor" << endl;
	TimeCode tc = TimeCode(0, 0, 0);

	assert(tc.ToString() == "0:0:0");
	
	
	// Verify that oversized minutes and seconds roll over correctly.
	TimeCode tc3 = TimeCode(3, 71, 3801);
	assert(tc3.ToString() == "5:14:21");
	
	// Boundary rollover cases
    TimeCode tc5(0, 60, 0);
    assert(tc5.ToString() == "1:0:0");

    TimeCode tc6(0, 0, 60);
    assert(tc6.ToString() == "0:1:0");

    TimeCode tc7(0, 120, 120);
    assert(tc7.ToString() == "2:2:0");


	TimeCode huge(2, 71, 234719572143ULL);
	assert(huge.ToString() == "65199884:20:3");
	
	cout << "PASSED!" << endl << endl;
}


void TestGetComponents(){
    cout << "Testing GetComponents" << endl;

    unsigned int h;
    unsigned int m;
    unsigned int s;

    TimeCode tc = TimeCode(5, 2, 18);
    tc.GetComponents(h, m, s);
    assert(h == 5 && m == 2 && s == 18);

    TimeCode zero(0, 0, 0);
    zero.GetComponents(h, m, s);
    assert(h == 0 && m == 0 && s == 0);

    // Verify constructor rollover is separated into valid components.
    TimeCode rollover(0, 60, 60);
    rollover.GetComponents(h, m, s);
    assert(h == 1 && m == 1 && s == 0);

    // Verify a larger hour value since hours have no 59 limit.
    TimeCode largeHours(187, 53, 27);
    largeHours.GetComponents(h, m, s);
    assert(h == 187 && m == 53 && s == 27);

    // Verify seconds rollover.
    TimeCode secondsRollover(0, 0, 3601);
    secondsRollover.GetComponents(h, m, s);
    assert(h == 1 && m == 0 && s == 1);

    cout << "PASSED!" << endl << endl;
}


void TestSubtract(){
	cout << "Testing Subtract" << endl;
	// Normal subtraction where the result remains positive.
	TimeCode tc1 = TimeCode(1, 0, 0);
	TimeCode tc2 = TimeCode(0, 50, 0);
	TimeCode tc3 = tc1 - tc2;
	assert(tc3.ToString() == "0:10:0");
	
	// Subtracting a larger TimeCode should throw because negative
    // TimeCodes are not allowed.
	TimeCode tc4 = TimeCode(1, 15, 45);
	try{
		TimeCode tc5 = tc1 - tc4;
		cout << "tc5: " << tc5.ToString() << endl;
		assert(false);
	}
	catch(const invalid_argument& e){
	}

	// Equal TimeCodes should subtract to exactly zero.
	TimeCode same1(2, 30, 0);
	TimeCode same2(2, 30, 0);

	assert((same1 - same2).ToString() == "0:0:0");

	// Verify subtraction across a minute boundary.
	TimeCode borrow1(1, 0, 0);
	TimeCode borrow2(0, 0, 1);

	assert((borrow1 - borrow2).ToString() == "0:59:59");
	
	cout << "PASSED!" << endl << endl;
}

void TestAdd()
{
    cout << "Testing Add" << endl;

    TimeCode tc1(1, 15, 22);
    TimeCode tc2(2, 9, 5);

    TimeCode result = tc1 + tc2;

    assert(result.ToString() == "3:24:27");

    // Verify addition automatically rolls extra seconds into minutes.
    TimeCode tc3(1, 15, 55);
    TimeCode tc4(0, 1, 25);

    assert((tc3 + tc4).ToString() == "1:17:20");

	TimeCode zero(0, 0, 0);
	TimeCode value(3, 20, 15);

	assert((value + zero).ToString() == "3:20:15");

    cout << "PASSED!" << endl << endl;
}

void TestMultiply()
{
    cout << "Testing Multiply" << endl;
	// Verify scaling by values greater than, less than, and equal to zero.
    TimeCode tc(1, 0, 0);

    assert((tc * 2).ToString() == "2:0:0");
    assert((tc * 0.5).ToString() == "0:30:0");
    assert((tc * 0).ToString() == "0:0:0");

	// A negative multiplier should throw because TimeCodes cannot be negative.
    try
    {
        TimeCode bad = tc * -1;

        assert(false);
    }
    catch (const invalid_argument& e)
    {
    }

    cout << "PASSED!" << endl << endl;
}

void TestDivide()
{
    cout << "Testing Divide" << endl;
    TimeCode tc(2, 0, 0);

    assert((tc / 2).ToString() == "1:0:0");
    assert((tc / 4).ToString() == "0:30:0");

	// Division by zero should throw.
    try
    {
        TimeCode bad = tc / 0;

        assert(false);
    }
    catch (const invalid_argument& e)
    {
    }

	// A negative divisor should throw because it would create a negative TimeCode.
    try
    {
        TimeCode bad = tc / -2;

        assert(false);
    }
    catch (const invalid_argument& e)
    {
    }

    cout << "PASSED!" << endl << endl;
}

void TestSetMinutes()
{
	cout << "Testing SetMinutes" << endl;

	TimeCode tc = TimeCode(8, 5, 9);
	tc.SetMinutes(15); // test valid change
	assert(tc.ToString() == "8:15:9");


	try
	{
		tc.SetMinutes(80);  // test invalid change
		assert(false);
	}
	catch (const invalid_argument &e)
	{
	}

	TimeCode boundary(1, 0, 0);
	boundary.SetMinutes(59);
	assert(boundary.ToString() == "1:59:0");

	// A failed setter should leave the previous valid value unchanged.
	assert(tc.ToString() == "8:15:9");

	cout << "PASSED!" << endl << endl;
}


void TestSetHours()
{
    cout << "Testing SetHours" << endl;

    TimeCode tc(8, 5, 9);

	// Hours are not restricted to 0-59, unlike minutes and seconds.
	tc.SetHours(187);

    assert(tc.ToString() == "187:5:9");

    cout << "PASSED!" << endl << endl;
}

void TestSetSeconds()
{
    cout << "Testing SetSeconds" << endl;
	
    TimeCode tc(8, 5, 9);

    tc.SetSeconds(30);

    assert(tc.ToString() == "8:5:30");

	// 60 is invalid because setters do not roll over.
    try
    {
        tc.SetSeconds(60);
        assert(false);
    }
    catch (const invalid_argument& e)
    {
    }

	TimeCode boundary(1, 0, 0);
	// 59 is the largest valid seconds value.
	boundary.SetSeconds(59);
	assert(boundary.ToString() == "1:0:59");

    // Object should still contain the old valid value
    assert(tc.ToString() == "8:5:30");

    cout << "PASSED!" << endl << endl;
}

void TestGetters()
{
    cout << "Testing Getters" << endl;

    TimeCode tc(8, 34, 27);

    assert(tc.GetHours() == 8);
    assert(tc.GetMinutes() == 34);
    assert(tc.GetSeconds() == 27);

    cout << "PASSED!" << endl << endl;
}

// Reset should return both the displayed time and stored seconds to zero.
void TestReset()
{
    cout << "Testing Reset" << endl;

    TimeCode tc(9, 15, 40);

    tc.reset();

    assert(tc.ToString() == "0:0:0");
    assert(tc.GetTimeCodeAsSeconds() == 0);

    cout << "PASSED!" << endl << endl;
}

void TestComparisons()
{
    cout << "Testing Comparisons" << endl;

    TimeCode a(1, 0, 0);
    TimeCode b(1, 0, 0);
    TimeCode c(2, 0, 0);

	// Check equality, ordering, and equal-value boundaries for all six operators.
    assert(a == b);
    assert(!(a != b));

    assert(a < c);
    assert(a <= c);
    assert(c > a);
    assert(c >= a);

    assert(a <= b);
    assert(a >= b);

	assert(a != c);
	assert(!(a > c));
	assert(!(c < a));


    cout << "PASSED!" << endl << endl;
}

void TestGetTimeCodeAsSeconds()
{
    cout << "Testing GetTimeCodeAsSeconds" << endl;

    TimeCode tc(1, 1, 1);

    assert(tc.GetTimeCodeAsSeconds() == 3661);

    cout << "PASSED!" << endl << endl;
}

void TestCopyConstructor()
{
    cout << "Testing Copy Constructor" << endl;
	// The copied object should contain exactly the same time as the original.
    TimeCode original(4, 15, 32);
    TimeCode copy(original);

    assert(copy.ToString() == "4:15:32");
    assert(copy.GetTimeCodeAsSeconds() ==
           original.GetTimeCodeAsSeconds());

    cout << "PASSED!" << endl << endl;
}

	
int main()
{
    TestComponentsToSeconds();

    TestDefaultConstructor();
    TestComponentConstructor();

    TestGetComponents();
    TestGetters();
    TestGetTimeCodeAsSeconds();

    TestSetHours();
    TestSetMinutes();
    TestSetSeconds();

    TestReset();

    TestAdd();
    TestSubtract();
    TestMultiply();
    TestDivide();

    TestComparisons();
	TestCopyConstructor();

    cout << "PASSED ALL TESTS!!!" << endl;

    return 0;
}
