#include "Enclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */

#include <errno.h>
#include <string.h> /* for memcpy etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)


typedef struct ms_init_t {
	int ms_no_of_clients;
} ms_init_t;

typedef struct ms_passInput_t {
	char* ms_input;
	size_t ms_len;
} ms_passInput_t;

typedef struct ms_getGCircuit_t {
	char* ms_input;
	size_t ms_len;
} ms_getGCircuit_t;

typedef struct ms_passGoutput_t {
	char* ms_input;
	size_t ms_len;
} ms_passGoutput_t;

typedef struct ms_passProgram_t {
	char* ms_input;
	size_t ms_len;
} ms_passProgram_t;

typedef struct ms_getOutputFromEnclave_t {
	char* ms_output_message;
	int ms_client_id;
	size_t ms_len;
} ms_getOutputFromEnclave_t;


typedef struct ms_ocall_print_string_t {
	char* ms_str;
} ms_ocall_print_string_t;

static sgx_status_t SGX_CDECL sgx_init(void* pms)
{
	ms_init_t* ms = SGX_CAST(ms_init_t*, pms);
	sgx_status_t status = SGX_SUCCESS;

	CHECK_REF_POINTER(pms, sizeof(ms_init_t));

	init(ms->ms_no_of_clients);


	return status;
}

static sgx_status_t SGX_CDECL sgx_passInput(void* pms)
{
	ms_passInput_t* ms = SGX_CAST(ms_passInput_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_input = ms->ms_input;
	size_t _tmp_len = ms->ms_len;
	size_t _len_input = _tmp_len;
	char* _in_input = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_passInput_t));
	CHECK_UNIQUE_POINTER(_tmp_input, _len_input);

	if (_tmp_input != NULL) {
		_in_input = (char*)malloc(_len_input);
		if (_in_input == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_input, _tmp_input, _len_input);
	}
	passInput(_in_input, _tmp_len);
err:
	if (_in_input) free(_in_input);

	return status;
}

static sgx_status_t SGX_CDECL sgx_getGCircuit(void* pms)
{
	ms_getGCircuit_t* ms = SGX_CAST(ms_getGCircuit_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_input = ms->ms_input;
	size_t _tmp_len = ms->ms_len;
	size_t _len_input = _tmp_len;
	char* _in_input = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_getGCircuit_t));
	CHECK_UNIQUE_POINTER(_tmp_input, _len_input);

	if (_tmp_input != NULL) {
		if ((_in_input = (char*)malloc(_len_input)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_input, 0, _len_input);
	}
	getGCircuit(_in_input, _tmp_len);
err:
	if (_in_input) {
		memcpy(_tmp_input, _in_input, _len_input);
		free(_in_input);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_passGoutput(void* pms)
{
	ms_passGoutput_t* ms = SGX_CAST(ms_passGoutput_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_input = ms->ms_input;
	size_t _tmp_len = ms->ms_len;
	size_t _len_input = _tmp_len;
	char* _in_input = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_passGoutput_t));
	CHECK_UNIQUE_POINTER(_tmp_input, _len_input);

	if (_tmp_input != NULL) {
		_in_input = (char*)malloc(_len_input);
		if (_in_input == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_input, _tmp_input, _len_input);
	}
	passGoutput(_in_input, _tmp_len);
err:
	if (_in_input) free(_in_input);

	return status;
}

static sgx_status_t SGX_CDECL sgx_passProgram(void* pms)
{
	ms_passProgram_t* ms = SGX_CAST(ms_passProgram_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_input = ms->ms_input;
	size_t _tmp_len = ms->ms_len;
	size_t _len_input = _tmp_len;
	char* _in_input = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_passProgram_t));
	CHECK_UNIQUE_POINTER(_tmp_input, _len_input);

	if (_tmp_input != NULL) {
		_in_input = (char*)malloc(_len_input);
		if (_in_input == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memcpy(_in_input, _tmp_input, _len_input);
	}
	passProgram(_in_input, _tmp_len);
err:
	if (_in_input) free(_in_input);

	return status;
}

static sgx_status_t SGX_CDECL sgx_getOutputFromEnclave(void* pms)
{
	ms_getOutputFromEnclave_t* ms = SGX_CAST(ms_getOutputFromEnclave_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_output_message = ms->ms_output_message;
	size_t _tmp_len = ms->ms_len;
	size_t _len_output_message = _tmp_len;
	char* _in_output_message = NULL;

	CHECK_REF_POINTER(pms, sizeof(ms_getOutputFromEnclave_t));
	CHECK_UNIQUE_POINTER(_tmp_output_message, _len_output_message);

	if (_tmp_output_message != NULL) {
		if ((_in_output_message = (char*)malloc(_len_output_message)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_output_message, 0, _len_output_message);
	}
	getOutputFromEnclave(_in_output_message, ms->ms_client_id, _tmp_len);
err:
	if (_in_output_message) {
		memcpy(_tmp_output_message, _in_output_message, _len_output_message);
		free(_in_output_message);
	}

	return status;
}

static sgx_status_t SGX_CDECL sgx_freeEnc(void* pms)
{
	sgx_status_t status = SGX_SUCCESS;
	if (pms != NULL) return SGX_ERROR_INVALID_PARAMETER;
	freeEnc();
	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[7];
} g_ecall_table = {
	7,
	{
		{(void*)(uintptr_t)sgx_init, 0},
		{(void*)(uintptr_t)sgx_passInput, 0},
		{(void*)(uintptr_t)sgx_getGCircuit, 0},
		{(void*)(uintptr_t)sgx_passGoutput, 0},
		{(void*)(uintptr_t)sgx_passProgram, 0},
		{(void*)(uintptr_t)sgx_getOutputFromEnclave, 0},
		{(void*)(uintptr_t)sgx_freeEnc, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[1][7];
} g_dyn_entry_table = {
	1,
	{
		{0, 0, 0, 0, 0, 0, 0, },
	}
};


sgx_status_t SGX_CDECL ocall_print_string(const char* str)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_str = str ? strlen(str) + 1 : 0;

	ms_ocall_print_string_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_print_string_t);
	void *__tmp = NULL;

	ocalloc_size += (str != NULL && sgx_is_within_enclave(str, _len_str)) ? _len_str : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_print_string_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_print_string_t));

	if (str != NULL && sgx_is_within_enclave(str, _len_str)) {
		ms->ms_str = (char*)__tmp;
		__tmp = (void *)((size_t)__tmp + _len_str);
		memcpy((void*)ms->ms_str, str, _len_str);
	} else if (str == NULL) {
		ms->ms_str = NULL;
	} else {
		sgx_ocfree();
		return SGX_ERROR_INVALID_PARAMETER;
	}
	
	status = sgx_ocall(0, ms);


	sgx_ocfree();
	return status;
}

