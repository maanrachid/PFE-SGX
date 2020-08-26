#include "Enclave_u.h"
#include <errno.h>

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

static sgx_status_t SGX_CDECL Enclave_ocall_print_string(void* pms)
{
	ms_ocall_print_string_t* ms = SGX_CAST(ms_ocall_print_string_t*, pms);
	ocall_print_string((const char*)ms->ms_str);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[1];
} ocall_table_Enclave = {
	1,
	{
		(void*)Enclave_ocall_print_string,
	}
};
sgx_status_t init(sgx_enclave_id_t eid, int no_of_clients)
{
	sgx_status_t status;
	ms_init_t ms;
	ms.ms_no_of_clients = no_of_clients;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t passInput(sgx_enclave_id_t eid, char* input, size_t len)
{
	sgx_status_t status;
	ms_passInput_t ms;
	ms.ms_input = input;
	ms.ms_len = len;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t getGCircuit(sgx_enclave_id_t eid, char* input, size_t len)
{
	sgx_status_t status;
	ms_getGCircuit_t ms;
	ms.ms_input = input;
	ms.ms_len = len;
	status = sgx_ecall(eid, 2, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t passGoutput(sgx_enclave_id_t eid, char* input, size_t len)
{
	sgx_status_t status;
	ms_passGoutput_t ms;
	ms.ms_input = input;
	ms.ms_len = len;
	status = sgx_ecall(eid, 3, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t passProgram(sgx_enclave_id_t eid, char* input, size_t len)
{
	sgx_status_t status;
	ms_passProgram_t ms;
	ms.ms_input = input;
	ms.ms_len = len;
	status = sgx_ecall(eid, 4, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t getOutputFromEnclave(sgx_enclave_id_t eid, char* output_message, int client_id, size_t len)
{
	sgx_status_t status;
	ms_getOutputFromEnclave_t ms;
	ms.ms_output_message = output_message;
	ms.ms_client_id = client_id;
	ms.ms_len = len;
	status = sgx_ecall(eid, 5, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t freeEnc(sgx_enclave_id_t eid)
{
	sgx_status_t status;
	status = sgx_ecall(eid, 6, &ocall_table_Enclave, NULL);
	return status;
}

