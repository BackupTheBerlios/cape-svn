/**
 * @file TestClass.h
 * Contains the prototype for the TestClass class.
 * @author
 * Jeremy Guillory
 * @date
 * Created on September 20, 2010, 10:12 AM
 */

#ifndef _TESTCLASS_H
#define	_TESTCLASS_H


/**
 * A class to demonstrate correct commenting using Doxygen.
 *
 * Detailed description
 */
class TestClass{

public:

     ///Returns the number of items.
    int getNumberItems();

    ///Sets the number of items.
     
    void setNumberItems(int nItems);

private:
   
     ///Private members are not docuemented using Doxygen, so normal documenting
     ///procedures may be used here.
      
    int nItems_;
};


#endif	/* _TESTCLASS_H */

