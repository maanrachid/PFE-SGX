#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */

#include "circuit.h"
#include "garbledcircuit.h"

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif


void init(int no_of_clients);
void passInput(char* input, size_t len);
void getGCircuit(char* input, size_t len);
void passGoutput(char* input, size_t len);
void passProgram(char* input, size_t len);
void getOutputFromEnclave(char* output_message, int client_id, size_t len);
void freeEnc();

sgx_status_t SGX_CDECL ocall_print_string(const char* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
