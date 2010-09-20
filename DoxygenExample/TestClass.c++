/**
 * @file TestClass.c++
 * This file is the implementation of the TestClass class.
 * @author
 * Jeremy Guillory
 * @date
 * Created on September 20, 2010, 10:12 AM
 */

#include "TestClass.h"

/**
 * This function return an integer which represents the total number of
 * items.
 *
 * @return
 * Integer representing the number of items.
 */
int TestClass::getNumberItems()
{
    return(nItems_);
}


/**
 * This function stores the number of items passed to it as a parameter.
 *
 * @param nItems
 * Integer representing the number of items.
 */
void TestClass::setNumberItems(int nItems)
{
    nItems_ = nItems;
}

