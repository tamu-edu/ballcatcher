#include "CObject.h"

static int allocCount0 = 0;
static int allocCount1 = 0;
static int deallocCount0 = 0;
static int deallocCount1 = 0;

CObject::CObject():x(0.0),
		   y(0.0),
		   speed(0.0),
		   caught(false),
		   fallen(false),
		   checked(false)
{
	//allocCount0++;
	//ballOrBread = 0;
}

CObject::CObject(double xval,
		 double yval,
		 double sval):x(xval),
			      y(yval),
			      speed(sval),
			      caught(false),
			      fallen(false),
			      checked(false)
{
	//allocCount1++;
	//ballOrBread = 1;
}

CObject::~CObject()
{
	//if ( ballOrBread ) {
	//	deallocCount1++;
	//	if ( deallocCount1 % 100 == 0 ) printf( "Ball + %d,  - %d\n", allocCount1, deallocCount1 );
	//}
	//else {
	//	deallocCount0++;
	//	if ( deallocCount0 % 100 == 0 ) printf( "Bread + %d,  - %d\n", allocCount0, deallocCount0 );
	//}
}