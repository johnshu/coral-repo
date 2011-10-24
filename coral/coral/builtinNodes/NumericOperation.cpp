// <license>
// Copyright (C) 2011 Andrea Interguglielmi, All rights reserved.
// This file is part of the coral repository downloaded from http://code.google.com/p/coral-repo.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
// 
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// </license>

#include "NumericOperation.h"
#include "numericOperationAlgos.h"

using namespace coral;

Numeric::Type numeric_type_int = Numeric::numericTypeInt;
Numeric::Type numeric_type_int_array = Numeric::numericTypeIntArray;
Numeric::Type numeric_type_float = Numeric::numericTypeFloat;
Numeric::Type numeric_type_float_array = Numeric::numericTypeFloatArray;
Numeric::Type numeric_type_vec3 = Numeric::numericTypeVec3;
Numeric::Type numeric_type_vec3_array = Numeric::numericTypeVec3Array;
Numeric::Type numeric_type_matrix44 = Numeric::numericTypeMatrix44;
Numeric::Type numeric_type_matrix44_array = Numeric::numericTypeMatrix44Array;

#define DEFINE_NUMERIC_OPERATION(operation, typeA, typeB) \
	void NumericOperation::operation_##operation##_##typeA##_##typeB##_array_to_array(Numeric *operandA, Numeric *operandB, Numeric *result){ \
		numericOperation_##operation##ArrayToArray<typeA, typeB>(operandA->_##typeA##Values, operandB->_##typeB##Values, result->_##typeA##Values); \
		result->_size = result->_##typeA##Values.size(); \
	} \
	void NumericOperation::operation_##operation##_##typeA##_##typeB##_single_to_array(Numeric *operandA, Numeric *operandB, Numeric *result){ \
		numericOperation_##operation##SingleToArray<typeA, typeB>(operandA->_##typeA##Values, operandB->_##typeB##Values, result->_##typeA##Values); \
		result->_size = result->_##typeA##Values.size(); \
	} \
	void NumericOperation::operation_##operation##_##typeA##_##typeB##_array_to_single(Numeric *operandA, Numeric *operandB, Numeric *result){ \
		numericOperation_##operation##ArrayToSingle<typeA, typeB>(operandA->_##typeA##Values, operandB->_##typeB##Values, result->_##typeA##Values); \
		result->_size = result->_##typeA##Values.size(); \
	} \

#define DEFINE_PASSTRHOUGH_OPERATION(type) \
	void NumericOperation::operation_##type##_passThrough(Numeric *operandA, Numeric *operandB, Numeric *result){ \
		numericOperation_passThrough<type>(operandA->_##type##Values, result->_##type##Values); \
		result->_size = result->_##type##Values.size(); \
	} \

#define SELECT_NUMERIC_OPERATION(operation, typeNameA, typeNameB) \
	if(typeA == numeric_type_##typeNameA && typeB == numeric_type_##typeNameB){ \
		_selectedOperation = &NumericOperation::operation_##operation##_##typeNameA##_##typeNameB##_array_to_single; return;} \
	if(typeA == numeric_type_##typeNameA##_array && typeB == numeric_type_##typeNameB){ \
		_selectedOperation = &NumericOperation::operation_##operation##_##typeNameA##_##typeNameB##_array_to_single; return;} \
	if(typeA == numeric_type_##typeNameA##_array && typeB == numeric_type_##typeNameB##_array){ \
		_selectedOperation = &NumericOperation::operation_##operation##_##typeNameA##_##typeNameB##_array_to_array; return;} \
	if(typeA == numeric_type_##typeNameA && typeB == numeric_type_##typeNameB##_array){ \
		_selectedOperation = &NumericOperation::operation_##operation##_##typeNameA##_##typeNameB##_single_to_array; return;} \

#define SELECT_PASSTRHOUGH_OPERATION(type) \
	if(typeA == numeric_type_##type || typeA == numeric_type_##type##_array){ \
		_selectedOperation = &NumericOperation::operation_##type##_passThrough; return;} \

#define ALLOW_NUMERIC_OPERATION(typeNameA, typeNameB) \
	if(typeA == numeric_type_##typeNameA && typeB == numeric_type_##typeNameB) \
		return true; \
	if(typeA == numeric_type_##typeNameA##_array && typeB == numeric_type_##typeNameB) \
		return true; \
	if(typeA == numeric_type_##typeNameA##_array && typeB == numeric_type_##typeNameB##_array) \
		return true; \
	if(typeA == numeric_type_##typeNameA && typeB == numeric_type_##typeNameB##_array) \
		return true; \

NumericOperation::NumericOperation():
	_selectedOperation(0){
}

bool NumericOperation::allowOperation(Operation operation, Numeric::Type typeA, Numeric::Type typeB){	
	if(operation == numericOperationAdd){
		ALLOW_NUMERIC_OPERATION(int, int);
		ALLOW_NUMERIC_OPERATION(int, float);
		ALLOW_NUMERIC_OPERATION(float, float);
		ALLOW_NUMERIC_OPERATION(float, int);
		ALLOW_NUMERIC_OPERATION(vec3, vec3);
	}
	else if(operation == numericOperationSub){
		ALLOW_NUMERIC_OPERATION(int, int);
		ALLOW_NUMERIC_OPERATION(int, float);
		ALLOW_NUMERIC_OPERATION(float, float);
		ALLOW_NUMERIC_OPERATION(float, int);
		ALLOW_NUMERIC_OPERATION(vec3, vec3);
	}
	else if(operation == numericOperationMul){
		ALLOW_NUMERIC_OPERATION(int, int);
		ALLOW_NUMERIC_OPERATION(int, float);
		ALLOW_NUMERIC_OPERATION(float, float);
		ALLOW_NUMERIC_OPERATION(float, int);
		ALLOW_NUMERIC_OPERATION(vec3, vec3);
		ALLOW_NUMERIC_OPERATION(vec3, float);
		ALLOW_NUMERIC_OPERATION(vec3, int);
		ALLOW_NUMERIC_OPERATION(vec3, matrix44);
		ALLOW_NUMERIC_OPERATION(matrix44, matrix44);
	}
	else if(operation == numericOperationDiv){
		ALLOW_NUMERIC_OPERATION(int, int);
		ALLOW_NUMERIC_OPERATION(int, float);
		ALLOW_NUMERIC_OPERATION(float, float);
		ALLOW_NUMERIC_OPERATION(float, int);
		ALLOW_NUMERIC_OPERATION(vec3, vec3);
		ALLOW_NUMERIC_OPERATION(vec3, float);
		ALLOW_NUMERIC_OPERATION(vec3, int);
	}
	
	return false;
}

void NumericOperation::selectOperands(Numeric::Type typeA, Numeric::Type typeB){
	if(typeA != Numeric::numericTypeAny){
		if(typeB == Numeric::numericTypeAny){
			SELECT_PASSTRHOUGH_OPERATION(int);
			SELECT_PASSTRHOUGH_OPERATION(float);
			SELECT_PASSTRHOUGH_OPERATION(vec3);
			SELECT_PASSTRHOUGH_OPERATION(matrix44);
		}
		else if(_operation == NumericOperation::numericOperationAdd){
			SELECT_NUMERIC_OPERATION(add, int, int);
			SELECT_NUMERIC_OPERATION(add, int, float);
			SELECT_NUMERIC_OPERATION(add, float, float);
			SELECT_NUMERIC_OPERATION(add, float, int);
			SELECT_NUMERIC_OPERATION(add, vec3, vec3);
		}
		else if(_operation == NumericOperation::numericOperationSub){
			SELECT_NUMERIC_OPERATION(sub, int, int);
			SELECT_NUMERIC_OPERATION(sub, int, float);
			SELECT_NUMERIC_OPERATION(sub, float, float);
			SELECT_NUMERIC_OPERATION(sub, float, int);
			SELECT_NUMERIC_OPERATION(sub, vec3, vec3);
		}
		else if(_operation == NumericOperation::numericOperationMul){
			SELECT_NUMERIC_OPERATION(mul, int, int);
			SELECT_NUMERIC_OPERATION(mul, int, float);
			SELECT_NUMERIC_OPERATION(mul, float, float);
			SELECT_NUMERIC_OPERATION(mul, float, int);
			SELECT_NUMERIC_OPERATION(mul, vec3, vec3);
			SELECT_NUMERIC_OPERATION(mul, vec3, float);
			SELECT_NUMERIC_OPERATION(mul, vec3, int);
			SELECT_NUMERIC_OPERATION(mul, vec3, matrix44);
			SELECT_NUMERIC_OPERATION(mul, matrix44, matrix44);
		}
		else if(_operation == NumericOperation::numericOperationDiv){
			SELECT_NUMERIC_OPERATION(div, int, int);
			SELECT_NUMERIC_OPERATION(div, int, float);
			SELECT_NUMERIC_OPERATION(div, float, float);
			SELECT_NUMERIC_OPERATION(div, float, int);
			SELECT_NUMERIC_OPERATION(div, vec3, vec3);
			SELECT_NUMERIC_OPERATION(div, vec3, float);
			SELECT_NUMERIC_OPERATION(div, vec3, int);
		}
	}
}

void NumericOperation::executeSelectedOperation(Numeric *operandA, Numeric *operandB, Numeric *out){
	if(_selectedOperation){
		(this->*_selectedOperation)(operandA, operandB, out);
	}
}

DEFINE_NUMERIC_OPERATION(add, int, int);
DEFINE_NUMERIC_OPERATION(add, int, float);
DEFINE_NUMERIC_OPERATION(add, float, float);
DEFINE_NUMERIC_OPERATION(add, float, int);
DEFINE_NUMERIC_OPERATION(add, vec3, vec3);

DEFINE_NUMERIC_OPERATION(sub, int, int);
DEFINE_NUMERIC_OPERATION(sub, int, float);
DEFINE_NUMERIC_OPERATION(sub, float, float);
DEFINE_NUMERIC_OPERATION(sub, float, int);
DEFINE_NUMERIC_OPERATION(sub, vec3, vec3);

DEFINE_NUMERIC_OPERATION(mul, int, int);
DEFINE_NUMERIC_OPERATION(mul, int, float);
DEFINE_NUMERIC_OPERATION(mul, float, float);
DEFINE_NUMERIC_OPERATION(mul, float, int);
DEFINE_NUMERIC_OPERATION(mul, vec3, vec3);
DEFINE_NUMERIC_OPERATION(mul, vec3, float);
DEFINE_NUMERIC_OPERATION(mul, vec3, int);
DEFINE_NUMERIC_OPERATION(mul, vec3, matrix44);
DEFINE_NUMERIC_OPERATION(mul, matrix44, matrix44);

DEFINE_NUMERIC_OPERATION(div, int, int);
DEFINE_NUMERIC_OPERATION(div, int, float);
DEFINE_NUMERIC_OPERATION(div, float, float);
DEFINE_NUMERIC_OPERATION(div, float, int);
DEFINE_NUMERIC_OPERATION(div, vec3, vec3);
DEFINE_NUMERIC_OPERATION(div, vec3, float);
DEFINE_NUMERIC_OPERATION(div, vec3, int);

DEFINE_PASSTRHOUGH_OPERATION(int);
DEFINE_PASSTRHOUGH_OPERATION(float);
DEFINE_PASSTRHOUGH_OPERATION(vec3);
DEFINE_PASSTRHOUGH_OPERATION(matrix44);