// Copyright Liam Taylor 2016
// An example of C language classes with virtual functions.
// Who needs C++, right?

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// VTable, or virtual function table, used for dynamic dispatch.
typedef struct VTable_Struct
{
	
	// Hold a reference to the parent VTable.
	struct VTable_Struct * Parent;
	void ** Calls;
	
} VTable;

// ==================================== Person class ====================================
typedef struct Person_Struct
{
	
	// A vtable reference, used for dynamic dispatch. Should be present in any instance of a "class".
	VTable * _vtab;
	
	// Some type-specific attributes
	const char * Name;
	uint32_t Age;
	
} Person;

// Global vtable variable. Note that while globals are a bad idea in general, they're required here.
// C++ is implemented similarly.
VTable _Person_VTable;
// The list of function pointers.
void * _Person_VCallList [ 6 ];

// Definitions with the index into the vtable of each function of the class.
// Note that in this example, i'm treating every method of Person as virtual.
#define Person_VTI_Delete 0
#define Person_VTI_SayHello 1
#define Person_VTI_TalkAboutSelf 2
#define Person_VTI_Grow 3
#define Person_VTI_GetAge 4
#define Person_VTI_GetName 5
#define Person_VTI_MaxVTI_ Person_VTI_GetName

// Constructor/Allocation function. (In real life these are seperate, and the constructor is also virtual)
Person * Person_New ( uint32_t Age, const char * Name );

// Actual method implementation definitions. These are what the vtable points to.
void _Person_Delete ( Person * Instance );

void _Person_SayHello ( Person * Instance );
void _Person_TalkAboutSelf ( Person * Instance );

void _Person_Grow ( Person * Instance );

uint32_t _Person_GetAge ( Person * Instance );
const char * _Person_GetName ( Person * Instance );

// Static initilizer for the Person class. In C++, this is hidden from you in startup code, but does actually happen.
void _Person_StaticInit ()
{
	
	_Person_VTable.Parent = NULL;
	_Person_VTable.Calls = _Person_VCallList;
	
	_Person_VCallList [ Person_VTI_Delete ] = (void *) & _Person_Delete;
	_Person_VCallList [ Person_VTI_SayHello ] = (void *) & _Person_SayHello;
	_Person_VCallList [ Person_VTI_TalkAboutSelf ] = (void *) & _Person_TalkAboutSelf;
	_Person_VCallList [ Person_VTI_Grow ] = (void *) & _Person_Grow;
	_Person_VCallList [ Person_VTI_GetAge ] = (void *) & _Person_GetAge;
	_Person_VCallList [ Person_VTI_GetName ] = (void *) & _Person_GetName;
	
};

// All of these inlines are just what C++ replaces a call to a virtual with. it looks complex, but it's really just calling a function from a list.
inline void Person_Delete ( Person * Instance )
{
	
	void ( * DeletePTR ) ( Person * ) = (void ( * ) ( Person * )) ( Instance -> _vtab -> Calls [ Person_VTI_Delete ] );
	( * DeletePTR ) ( Instance );
	
};

inline void Person_SayHello ( Person * Instance )
{
	
	void ( * SayHelloPTR ) ( Person * ) = (void ( * ) ( Person * )) ( Instance -> _vtab -> Calls [ Person_VTI_SayHello ] );
	( * SayHelloPTR ) ( Instance );
	
};

inline void Person_TalkAboutSelf ( Person * Instance )
{
	
	void ( * TalkAboutSelfPTR ) ( Person * ) = (void ( * ) ( Person * )) ( Instance -> _vtab -> Calls [ Person_VTI_TalkAboutSelf ] );
	( * TalkAboutSelfPTR ) ( Instance );
	
};

inline void Person_Grow ( Person * Instance )
{
	
	void ( * GrowPTR ) ( Person * ) = (void ( * ) ( Person * )) ( Instance -> _vtab -> Calls [ Person_VTI_Grow ] );
	( * GrowPTR ) ( Instance );
	
};

inline uint32_t Person_GetAge ( Person * Instance )
{
	
	uint32_t ( * GetAgePTR ) ( Person * ) = (uint32_t ( * ) ( Person * )) ( Instance -> _vtab -> Calls [ Person_VTI_GetAge ] );
	return ( * GetAgePTR ) ( Instance );
	
};

inline const char * Person_GetName ( Person * Instance )
{
	
	const char * ( * GetNamePTR ) ( Person * ) = (const char * ( * ) ( Person * )) ( Instance -> _vtab -> Calls [ Person_VTI_GetName ] );
	return ( * GetNamePTR ) ( Instance );
	
};

// ==================================== Student class ===================================

// Student here "inherets" from Person. Note that in C, a pointer to a struct is gaurenteed to point to it's first member.
typedef struct Student_Struct
{
	
	// Super contains the _vtab instance of VTable.
	Person Super;
	
	double GPA;
	
} Student;

// The Student class VTable and call list.
VTable _Student_VTable;
void * _Student_VCallList [ 7 ];

// These indexes are inhereted, since Person is the superclass of Student.
#define Student_VTI_Delete Person_VTI_Delete
#define Student_VTI_SayHello Person_VTI_SayHello
#define Student_VTI_TalkAboutSelf Person_VTI_TalkAboutSelf
#define Student_VTI_Grow Person_VTI_Grow
#define Student_VTI_GetAge Person_VTI_GetAge
#define Student_VTI_GetName Person_VTI_GetName
// Here, we start adding new functions from the Student class to the end of the list.
#define Student_VTI_GetGPA Person_VTI_MaxVTI_ + 1

// Student constructor.
Student * Student_New ( uint32_t Age, const char * Name, double GPA );

// Actual implementation definitions. The Student VTable points to these instead of the ones defined for Person, but only in the case that they override a similarly named one for the person class.
void _Student_TalkAboutSelf ( Student * Instance );

// Since this one doesn't override a Person class function, it is stored in a unique position in the Student VTable.
double _Student_GetGPA ( Student * Instance );

// Another static initilizer. When using G++, there's a function like _ctor or something that gets called before main.
void _Student_StaticInit ()
{
	
	// Record Person as our Parent class for Student
	_Student_VTable.Parent = & _Person_VTable;
	_Student_VTable.Calls = _Student_VCallList;
	
	// Note that in any case where we haven't overridden a method from the Person class, we reference the original implementation in the Student vtable
	_Student_VCallList [ Student_VTI_Delete ] = (void *) & _Person_Delete;
	_Student_VCallList [ Student_VTI_SayHello ] = (void *) & _Person_SayHello;
	_Student_VCallList [ Student_VTI_TalkAboutSelf ] = (void *) & _Student_TalkAboutSelf;
	_Student_VCallList [ Student_VTI_Grow ] = (void *) & _Person_Grow;
	_Student_VCallList [ Student_VTI_GetAge ] = (void *) & _Person_GetAge;
	_Student_VCallList [ Student_VTI_GetName ] = (void *) & _Person_GetName;
	_Student_VCallList [ Student_VTI_GetGPA ] = (void *) & _Student_GetGPA;
	
};

// These are exactly the same as the ones for Person, since they are just vtable calls. The only reason these exist here is for consistency.
inline void Student_Delete ( Student * Instance )
{
	
	void ( * DeletePTR ) ( Student * ) = (void ( * ) ( Student * )) ( Instance -> Super._vtab -> Calls [ Student_VTI_Delete ] );
	( * DeletePTR ) ( Instance );
	
}

inline void Student_SayHello ( Student * Instance )
{
	
	void ( * SayHelloPTR ) ( Student * ) = (void ( * ) ( Student * )) ( Instance -> Super._vtab -> Calls [ Student_VTI_SayHello ] );
	( * SayHelloPTR ) ( Instance );
	
};

inline void Student_TalkAboutSelf ( Student * Instance )
{
	
	void ( * TalkAboutSelfPTR ) ( Student * ) = (void ( * ) ( Student * )) ( Instance -> Super._vtab -> Calls [ Student_VTI_TalkAboutSelf ] );
	( * TalkAboutSelfPTR ) ( Instance );
	
};

inline void Student_Grow ( Student * Instance )
{
	
	void ( * GrowPTR ) ( Student * ) = (void ( * ) ( Student * )) ( Instance -> Super._vtab -> Calls [ Student_VTI_Grow ] );
	( * GrowPTR ) ( Instance );
	
};

inline uint32_t Student_GetAge ( Student * Instance )
{
	
	uint32_t ( * GetAgePTR ) ( Student * ) = (uint32_t ( * ) ( Student * )) ( Instance -> Super._vtab -> Calls [ Student_VTI_GetAge ] );
	return ( * GetAgePTR ) ( Instance );
	
};

inline const char * Student_GetName ( Student * Instance )
{
	
	const char * ( * GetNamePTR ) ( Student * ) = (const char * ( * ) ( Student * )) ( Instance -> Super._vtab -> Calls [ Student_VTI_GetName ] );
	return ( * GetNamePTR ) ( Instance );
	
};

// Since we're never actually superclassing Student, it's unlikely we'll need to reference it's type-specific methods in the vtable.
// That being said, this is here for completeness. 

inline double Student_GetGPA ( Student * Instance )
{
	
	double ( * GetGPAPTR ) ( Student * ) = (double ( * ) ( Student * )) ( Instance -> Super._vtab -> Calls [ Student_VTI_GetGPA ] );
	return ( * GetGPAPTR ) ( Instance );
	
};

// ======================================== Main ========================================

// An example of a function that Takes a Student instance.
void FunctionTakingStudent ( Student * Instance );

// An example of a function that takes a Person instance
void FunctionTakingPerson ( Person * Instance );

int32_t main ( int32_t argc, const char * argv [] )
{
	
	_Person_StaticInit ();
	_Student_StaticInit ();
	
	// Create a new student object.
	Student * Liam = Student_New ( 19, "Liam", 3.8 );
	
	if ( Liam == NULL )
		return 1;
	
	FunctionTakingPerson ( (Person *) Liam );
	
	Student_Delete ( Liam );
	
	Liam = NULL;
	
	return 0;
	
};

void FunctionTakingStudent ( Student * Instance )
{
	
	// Note that because we're calling a function that is never overridden, we don't need to do it virtually. We can just use the definit implementation.
	double GPA = _Student_GetGPA ( Instance );
	printf ( "%s's GPA is %.2f\n", Instance -> Super.Name, Instance -> GPA );
	
};

void FunctionTakingPerson ( Person * Instance )
{
	
	// In this case however, Person's methods called may be overriden, so we have to call them virtually.
	Person_SayHello ( Instance );
	Person_TalkAboutSelf ( Instance );
	
};

// ================================== Person class impl ==================================

Person * Person_New ( uint32_t Age, const char * Name )
{
	
	// Allocate a new instance of Person.
	Person * Instance = (Person *) malloc ( sizeof ( Person ) );
	
	if ( Instance == NULL )
		return NULL;
	
	// Set the instances member variables.
	Instance -> Age = Age;
	Instance -> Name = Name;
	
	// Set the vtable reference to the appropriate instance.
	Instance -> _vtab = & _Person_VTable;
	
	return Instance;
	
};

// Definit implementations of functions. These are not called directly, but rather through the vtable, as they are all "virtual".
void _Person_Delete ( Person * Instance )
{
	
	free ( (void *) Instance );
	
}

void _Person_Grow ( Person * Instance )
{
	
	Instance -> Age ++;
	
};

void _Person_SayHello ( Person * Instance )
{
	
	printf ( "Hello world, I'm %s!\n", Instance -> Name );
	
};

void _Person_TalkAboutSelf ( Person * Instance )
{
	
	printf ( "I am %d years old.\n", Instance -> Age );
	
};

uint32_t _Person_GetAge ( Person * Instance )
{
	
	return Instance -> Age;
	
};

const char * _Person_GetName ( Person * Instance )
{
	
	return Instance -> Name;
	
};

// ================================== Student class impl =================================

Student * Student_New ( uint32_t Age, const char * Name, double GPA )
{
	
	// Allocate a new instance of Student
	Student * Instance = (Student *) malloc ( sizeof ( Student ) );
	
	if ( Instance == NULL )
		return NULL;
	
	// Set member variables
	Instance -> Super.Age = Age;
	Instance -> Super.Name = Name;
	Instance -> GPA = GPA;
	
	// Now we're using the student VTable, which means that it will point to overridden methods on Person.
	Instance -> Super._vtab = & _Student_VTable;
	
	return Instance;
	
};

double _Student_GetGPA ( Student * Instance )
{
	
	return Instance -> GPA;
	
};

// The overriden function. Any time the method TalkAboutSelf is called on an instance of Student, EVEN IF it is cast to Person, this is the method executed.
void _Student_TalkAboutSelf ( Student * Instance )
{
	
	printf ( "I am %d years old, and doing %s in class!\n", Instance -> Super.Age, ( Instance -> GPA > 2.6 ) ? "Well" : "Poorly" );
	
};
