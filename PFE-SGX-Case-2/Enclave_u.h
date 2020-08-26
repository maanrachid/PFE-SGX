#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_satus_t etc. */

#include "circuit.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print_string, (const char* str));

sgx_status_t init(sgx_enclave_id_t eid, int no_of_clients);
sgx_status_t passInput(sgx_enclave_id_t eid, char* input, size_t len);
sgx_status_t getGCircuit(sgx_enclave_id_t eid, char* input, size_t len);
sgx_status_t passGoutput(sgx_enclave_id_t eid, char* input, size_t len);
sgx_status_t passProgram(sgx_enclave_id_t eid, char* input, size_t len);
sgx_status_t getOutputFromEnclave(sgx_enclave_id_t eid, char* output_message, int client_id, size_t len);
sgx_status_t freeEnc(sgx_enclave_id_t eid);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
