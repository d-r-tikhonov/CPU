#ifndef STACK_H
#define STACK_H

//=====================================================================================================================================

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#include "errors.h"
#include "config.h"

//=====================================================================================================================================

#define CANARY_PROTECT

//=====================================================================================================================================

#ifdef CANARY_PROTECT
	typedef uint32_t canary_t;
	const   uint32_t Canary = 0xCAFEBABE;
#endif
//=====================================================================================================================================

struct stack_t
{
	#ifdef CANARY_PROTECT
		canary_t leftCanary = 0;
	#endif

	elem_t* data    = nullptr;
	size_t size     = 0;
	size_t capacity = 0;

	#ifdef CANARY_PROTECT
		canary_t rightCanary = 0;
	#endif
};

//=====================================================================================================================================

void 	StackCtor 	(stack_t* const stk);
void 	StackPush 	(stack_t* stk, const elem_t item);
elem_t 	StackPop 	(stack_t* const stk);
void 	StackDtor 	(stack_t* const stk);
void 	StackDump 	(stack_t* const stk);

//=====================================================================================================================================

#endif