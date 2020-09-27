Introduction
-------------

This is an implementation for Private function Evaluation using Intel SGX technology. 

Circuits are taken from https://homes.esat.kuleuven.be/~nsmart/MPC/  


Installation:
-------------

In this document, we assume that you have already installed Intel(R) SGX SDK. if you haven't done that yet, please install it from:

https://01.org


- cd to the directory of SGX SDK folder and run :

  make sdk_install_pkg


This will create a new bin file in linux/installer/bin


IPP should be installed for encryption and decryption
of the communication between the clients and the enclave.



Compiling SGX PFE
-----------------


- cd to PFE-SGX/PFE_SGX-Case-1 (or Case-2) folder and run: 

  ${path_to_SGX_SDK_FOLDER}/sgx_linux_x64_sdk_${version}.bin

  sgx_linux_x64_sdk_${version}.bin is the file that you have created before when installing SGX   

- Answer "yes" for question: Do you want to install in current directory?

- Copy and run the source command to set the environment variables (as indicated)

- Run make (run make clean first if necessary)



Usage
-------

Case 1) 
--------

Here we have 2 applications: a server (it is the program owner and enclave parent at the same time), and clients (which are data owners)

- To run the server:

	server <server_port> <filename> <number_of_clients>

	./server 3333 circuits/adder_32bit.txt 2



- To run a data owner

	client <server_ip> <server_port> <client_id> <input_size> <output_size> <full_input_size> [num_of_iterations] 


	./client 127.0.0.1  3333 0 32 33 64 

	OR 

	./client 127.0.0.1  3333 0 32 33 64 100


	The client IDs start from 0. So, if you have 2 clients, you have client 0 and client 1. 



Case 2 
--------


Here we have 3 applications since the program owner is different than the enclave parent:

	server <server_port_clients> <server_port_program_owner> <no_of_clients> <fullinputsize> <outputsize>

	./server 3333 3335 2 64 33



	client <server_ip> <server_port> <client_id> <input_size> <output_size> <full_input_size> [num_of_iterations]

	 ./client 127.0.0.1  3333 1 32 33 64 5



	program_owner <server_ip> <server_port> <program_file> [num_of_iterations]

	./powner 127.0.0.1 3335 circuits/adder_32bit.txt 5





Number of iterations:
----------------------

The client can run in 2 modes: iterations and interactive.

- Iterations: the data input will be taken automatically from the file "input<client_id>", so for client 0, the input file will be "input0", for client 1, the input file will be "input1" and so on. Input file should have input_size * number_of_iteration binary numbers (0 or 1).   

- if you don't use [num_of_iterations], you will use the interactive mode in which you can pass input from the command line as binary or decimal, or from a file (binary only). 


Generate random cases 
-----------------------

The client app allows you in the interactive mode to get the input data from the command line or from a file. you can generate a random string of length 32 binary numbers using 

	generate_random_cases 32 > input0.txt 

input0.txt will have a 32 numbers (ones and zeros). To generate an input for 100 iterations:


	generate_random_cases 3200 > input0.txt



Number of clients (data owners)
------------------------------
you can run as many data owners (clients) as you want, but the input size should be divisible by their count. You have only one program owner. 


Encryption & Decryption
-----------------------
Encryption is done using IPP. IPP has 2 versions: ONE for the enclave, and ONE for the untrusted code.


- IMPORTANT: make sure that all pathes for the IPP library are correct in the makefile:

IPPinc is the path of the header files (used by untrusted code)
IPPPath is the path of the library to be used (used by untrusted code)
IPP_LIB_Client is the name of the library to be used (used by untrusted code)
IPP_LIB_Enclave is the name of the library used (sgx_tcrypto)



You can also implement your own encryption and use it. For testing, we were using simple xor.





IMPORTANT!!!
--------------

Circuits are taken from 

https://homes.esat.kuleuven.be/~nsmart/MPC/
 
we added them to the folder since we needed to modify them a little in order to fit our programs (actually we modified only the top few lines in order to divide the input between clients).   



if you have any question, please email:

Maan Haj Rachid
maanhajrachid@hotmail.com


Publication
------------
Omar Abou Selo, Maan Haj Rachid, Abdullatif Shikfa, Yongge Wang, Qutaibah Malluhi, "Private Function Evaluation Using Intel’s SGX", Security and Communication Networks, vol. 2020, Article ID 3042642, 10 pages, 2020. 

https://doi.org/10.1155/2020/3042642
https://www.hindawi.com/journals/scn/2020/3042642/




 



