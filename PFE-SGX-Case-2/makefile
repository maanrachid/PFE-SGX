SGX_I_FLAGS =  -Isgxsdk/include -Isgxsdk/include/tlibc -Isgxsdk/include/stlport 
SGX_DEBUG =  -DNDEBUG -UEDEBUG -UDEBUG
INC= inc
IPPinc = IPPSGX/external/ippcp_internal/inc
IPPPath=  /opt/intel/compilers_and_libraries_2018.3.240/linux/ippcp/lib/intel64_\and
IPP_LIB_Client = ippcp
IPP_LIB_Enclave = sgx_tcrypto





client:	server
	g++  -I$(INC) -I$(IPPinc) -c client.c -o client.o 
	g++ Encryptor.o client.o -o client  -L $(IPPPath)  -l$(IPP_LIB_Client)
	g++ gen_input.cpp -o generate_random_cases
	g++ -I$(INC) -I$(IPPinc)   -c -fpermissive  garbledcircuit.c -o garbledcircuit.o
	g++  -I$(INC) -I$(IPPinc) -c  program_owner.c -o powner.o
	g++  Encryptor.o circuit.o garbledcircuit.o reader.o powner.o -o powner  -L $(IPPPath)  -l$(IPP_LIB_Client)

server:	clean
	sgxsdk/bin/x64/sgx_edger8r --untrusted Enclave.edl --search-path ./ --search-path sgxsdk/include
	g++ -m64 -O2 -fPIC -Wno-attributes -Isgxsdk/include $(SGX_DEBUG) -c Enclave_u.c -o Enclave_u.o
	g++ -m64 -O2 -fPIC -Wno-attributes -Isgxsdk/include    $(SGX_DEBUG) -std=c++11 -c server.c -o server.o
	g++ -m64 -O2 -fPIC -Wno-attributes  $(SGX_DEBUG) -std=c++11 -c reader.c -o reader.o
	g++ -m64 -O2 -fPIC -Wno-attributes  $(SGX_DEBUG) -std=c++11 -c  circuit.c -o circuit.o
	g++ -I$(INC)  -I$(IPPinc) -std=c++11 -c  $(INC)/Encryptor.cpp -o Encryptor.o
	g++ -m64 -O2 -fPIC -Wno-attributes $(SGX_I_FLAGS) -I$(INC) -I$(IPPinc)  -nostdinc++  $(SGX_DEBUG)  -std=c++11 -c   garbledcircuit_enclave.c -o garbledcircuit_enclave.o	
	g++ Enclave_u.o circuit.o reader.o server.o   -o server -m64 -O2 -Lsgxsdk/lib64 -lsgx_urts_sim -lpthread  -lsgx_uae_service_sim
	sgxsdk/bin/x64/sgx_edger8r --trusted Enclave.edl --search-path ./ --search-path sgxsdk/include
	g++ -m64 -O2 -nostdinc -fvisibility=hidden -fpie -fstack-protector $(SGX_I_FLAGS) -I$(INC) -I$(IPPinc)  -c Enclave_t.c -o Enclave_t.o  
	g++ -m64 -O2 -nostdinc -fvisibility=hidden -fpie -fstack-protector  $(SGX_I_FLAGS)  -I$(INC) -I$(IPPinc)  -std=c++03 -nostdinc++ -c Enclave.cpp -o Enclave.o 
	g++  Encryptor.o circuit.o garbledcircuit_enclave.o  Enclave_t.o Enclave.o    -o enclave.so -g -m64 -O2 -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -Lsgxsdk/lib64 -Wl,--whole-archive -lsgx_trts_sim -Wl,--no-whole-archive -Wl,--start-group -lsgx_tstdc -lsgx_tstdcxx -lsgx_tcrypto -lsgx_tservice_sim -Wl,--end-group -Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined -Wl,-pie,-eenclave_entry -Wl,--export-dynamic -Wl,--defsym,__ImageBase=0 -Wl,--version-script=Enclave.lds
	sgxsdk/bin/x64/sgx_sign sign -key Enclave_private.pem -enclave enclave.so -out enclave.signed.so -config Enclave.config.xml

clean:
	rm -f *~ *# client clientOneByOne middle server *.o Enclave_t* Enclave_u* powner generate_random_cases
