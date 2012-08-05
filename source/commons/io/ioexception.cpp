/******************************************************************************
 * ioexception.cpp
 * 		Definiciones de la clase de commons::io::ioexception
******************************************************************************/
#include "ioexception.h"

using namespace commons::io;
using namespace std;

ioexception::ioexception(const string &what)
: runtime_error(what) {

}
